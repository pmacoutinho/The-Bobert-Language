// codegen.h
#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/ast.h"
#include <llvm-c/Core.h>

// Global LLVM context variables
extern LLVMContextRef TheContext;
extern LLVMModuleRef TheModule;
extern LLVMBuilderRef Builder;

// Function declarations
void InitializeLLVM();
void LogError(const char *Str);
LLVMValueRef LogErrorV(const char *Str);

// Code generation functions for AST nodes
LLVMValueRef number_codegen(ASTNode *node);
LLVMValueRef identifier_codegen(ASTNode *node);
LLVMValueRef binary_codegen(ASTNode *node);
LLVMValueRef assignment_codegen(ASTNode *node);
LLVMValueRef call_codegen(ASTNode *node);
LLVMValueRef prototype_codegen(ASTNode *node);
LLVMValueRef func_codegen(ASTNode *node);
LLVMValueRef block_codegen(ASTNode *node);
LLVMValueRef extern_codegen(ASTNode *node);

// Helper functions
void AddNamedValue(const char *name, LLVMValueRef value);
LLVMValueRef FindNamedValue(const char *name);

#endif // CODEGEN_H