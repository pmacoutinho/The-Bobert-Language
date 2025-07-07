// ast_codegen.c
#include "../include/ast.h"
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

LLVMContextRef TheContext = NULL;
LLVMModuleRef TheModule = NULL;
LLVMBuilderRef Builder = NULL;
static LLVMValueRef *NamedValues = NULL;
static size_t NamedValuesCount = 0;

void InitializeLLVM() {
    TheContext = LLVMContextCreate();
    TheModule = LLVMModuleCreateWithNameInContext("my cool jit", TheContext);
    Builder = LLVMCreateBuilderInContext(TheContext);
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
}

void LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
}

LLVMValueRef LogErrorV(const char *Str) {
    LogError(Str);
    return NULL;
}

// Helper function to add a named value
void AddNamedValue(const char *name, LLVMValueRef value) {
    NamedValues = realloc(NamedValues, (NamedValuesCount + 1) * sizeof(LLVMValueRef));
    NamedValues[NamedValuesCount++] = value;
    // Note: This simple implementation doesn't handle name collisions like the C++ map
}

// Helper function to find a named value
LLVMValueRef FindNamedValue(const char *name) {
    for (size_t i = 0; i < NamedValuesCount; i++) {
        if (strcmp(LLVMGetValueName(NamedValues[i]), name) == 0) {
            return NamedValues[i];
        }
    }
    return NULL;
}

LLVMValueRef number_codegen(ASTNode *node) {
    // Your AST uses int value
    return LLVMConstReal(LLVMDoubleTypeInContext(TheContext), (double)node->value);
}

LLVMValueRef identifier_codegen(ASTNode *node) {
    LLVMValueRef V = FindNamedValue(node->name);
    if (!V)
        return LogErrorV("Unknown variable name");
    // Load the value from the pointer
    return LLVMBuildLoad2(Builder, LLVMDoubleTypeInContext(TheContext), V, node->name);
}

LLVMValueRef binary_codegen(ASTNode *node) {
    LLVMValueRef L = node->left->codegen(node->left);
    LLVMValueRef R = node->right->codegen(node->right);
    if (!L || !R)
        return NULL;

    switch (node->op) {
    case '+':
        return LLVMBuildFAdd(Builder, L, R, "addtmp");
    case '-':
        return LLVMBuildFSub(Builder, L, R, "subtmp");
    case '*':
        return LLVMBuildFMul(Builder, L, R, "multmp");
    // case '<': {
    //     LLVMValueRef cmp = LLVMBuildFCmp(Builder, LLVMRealULT, L, R, "cmptmp");
    //     return LLVMBuildUIToFP(Builder, cmp, LLVMDoubleTypeInContext(TheContext), "booltmp");
    // }
    default:
        return LogErrorV("invalid binary operator");
    }
}

LLVMValueRef assignment_codegen(ASTNode *node) {
    // Generate code for the right-hand side expression
    LLVMValueRef val = node->varValue->codegen(node->varValue);
    if (!val) return NULL;
    
    // Check if variable already exists
    LLVMValueRef variable = FindNamedValue(node->varName->name);
    
    if (!variable) {
        // Get current function
        LLVMBasicBlockRef current_block = LLVMGetInsertBlock(Builder);
        LLVMValueRef function = LLVMGetBasicBlockParent(current_block);
        
        // Create a builder for the entry block
        LLVMBuilderRef entry_builder = LLVMCreateBuilderInContext(TheContext);
        LLVMBasicBlockRef entry_block = LLVMGetEntryBasicBlock(function);
        LLVMPositionBuilderAtEnd(entry_builder, entry_block);
        
        // Allocate space for the variable in the entry block
        variable = LLVMBuildAlloca(entry_builder, 
                                 LLVMDoubleTypeInContext(TheContext),
                                 node->varName->name);
        
        // Clean up the entry block builder
        LLVMDisposeBuilder(entry_builder);
        
        // Add to named values
        AddNamedValue(node->varName->name, variable);
    }
    
    // Store the value
    LLVMBuildStore(Builder, val, variable);
    
    return val;
}

LLVMValueRef block_codegen(ASTNode *node) {
    // First process any extern declarations
    for (int i = 0; i < node->count; i++) {
        if (node->statements[i]->type == AST_EXTERN) {
            // For extern, we just need to generate the prototype
            if (node->statements[i]->call && node->statements[i]->call->codegen) {
                node->statements[i]->call->codegen(node->statements[i]->call);
            }
            continue;
        }
    }

    LLVMTypeRef double_type = LLVMDoubleTypeInContext(TheContext);
    LLVMTypeRef func_type = LLVMFunctionType(double_type, NULL, 0, 0);
    LLVMValueRef func = LLVMAddFunction(TheModule, "__anon_expr", func_type);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(func, "entry");
    LLVMPositionBuilderAtEnd(Builder, entry);

    // Generate code for non-extern statements
    LLVMValueRef last = NULL;
    for (int i = 0; i < node->count; i++) {
        if (node->statements[i]->type != AST_EXTERN) {
            last = node->statements[i]->codegen(node->statements[i]);
            if (!last) {
                LLVMDeleteFunction(func);
                return NULL;
            }
        }
    }

    if (!last) {
        last = LLVMConstReal(LLVMDoubleTypeInContext(TheContext), 0.0);
    } else if (LLVMGetTypeKind(LLVMTypeOf(last)) != LLVMDoubleTypeKind) {
        last = LLVMBuildLoad2(Builder, LLVMDoubleTypeInContext(TheContext), last, "loadtmp");
    }

    LLVMBuildRet(Builder, last);
    LLVMVerifyFunction(func, LLVMAbortProcessAction);
    return func;
}

LLVMValueRef call_codegen(ASTNode *node) {
    // Look up the name in the global module table
    LLVMValueRef CalleeF = LLVMGetNamedFunction(TheModule, node->callee);
    if (!CalleeF)
        return LogErrorV("Unknown function referenced");

    // Check argument count
    unsigned num_args = LLVMCountParams(CalleeF);
    if (num_args != node->args->size)
        return LogErrorV("Incorrect # arguments passed");

    // Generate code for each argument
    LLVMValueRef *ArgsV = malloc(node->args->size * sizeof(LLVMValueRef));
    for (size_t i = 0; i < node->args->size; i++) {
        ArgsV[i] = node->args->data[i]->codegen(node->args->data[i]);
        if (!ArgsV[i]) {
            free(ArgsV);
            return NULL;
        }
    }

    LLVMValueRef call = LLVMBuildCall2(Builder, 
                                      LLVMGetElementType(LLVMTypeOf(CalleeF)),
                                      CalleeF, 
                                      ArgsV, 
                                      node->args->size, 
                                      "calltmp");
    free(ArgsV);
    return call;
}

LLVMValueRef prototype_codegen(ASTNode *node) {
    const char* funcName = node->funcName;
    size_t argCount = node->args ? node->args->size : 0;

    LLVMTypeRef *ParamTypes = malloc(argCount * sizeof(LLVMTypeRef));
    for (size_t i = 0; i < argCount; i++) {
        ParamTypes[i] = LLVMDoubleTypeInContext(TheContext);
    }

    LLVMTypeRef RetType = LLVMDoubleTypeInContext(TheContext);
    LLVMTypeRef FuncType = LLVMFunctionType(RetType, ParamTypes, argCount, 0);
    free(ParamTypes);

    LLVMValueRef F = LLVMAddFunction(TheModule, funcName, FuncType);

    // Set names for all arguments
    LLVMValueRef Param;
    LLVMGetParams(F, &Param);
    for (size_t i = 0; i < argCount; i++) {
        ASTNode *argNode = node->args->data[i];
        LLVMSetValueName(Param, argNode->name);
        Param = LLVMGetNextParam(Param);
    }

    return F;
}

LLVMValueRef func_codegen(ASTNode *node) {
    const char* funcName = node->prototype->funcName;
    LLVMValueRef TheFunction = LLVMGetNamedFunction(TheModule, funcName);

    if (!TheFunction)
        TheFunction = prototype_codegen(node);

    if (!TheFunction)
        return NULL;

    LLVMBasicBlockRef BB = LLVMAppendBasicBlockInContext(TheContext, TheFunction, "entry");
    LLVMPositionBuilderAtEnd(Builder, BB);

    // Clear named values and add function arguments
    NamedValuesCount = 0;
    if (NamedValues) {
        free(NamedValues);
        NamedValues = NULL;
    }

    LLVMValueRef Param;
    LLVMGetParams(TheFunction, &Param);
    size_t argCount = node->args ? node->args->size : 0;
    for (size_t i = 0; i < argCount; i++) {
        ASTNode *argNode = node->args->data[i];
        AddNamedValue(argNode->name, Param);
        Param = LLVMGetNextParam(Param);
    }

    if (LLVMValueRef RetVal = node->body->codegen(node->body)) {
        LLVMBuildRet(Builder, RetVal);

        LLVMVerifyFunction(TheFunction, LLVMAbortProcessAction);

        return TheFunction;
    }

    LLVMDeleteFunction(TheFunction);
    return NULL;
}
