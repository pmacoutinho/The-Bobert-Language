#include "../include/codegen.h"
#include "../include/token.h"
#include "../include/ast.h"

#include <stdio.h>
#include <stdlib.h>

extern const char* source;
extern ASTNode* parse();
extern void InitializeLLVM();

int main() {
    InitializeLLVM();

    source = "fib(x) { fib(x-1)+fib(x-2) }";
    ASTNode* tree = parse();

    if (tree) {
        printf("AST:\n");
        print_ast(tree, 0);
        
        if (tree->codegen) {
            LLVMValueRef result = tree->codegen(tree);
            if (result) {
                char *ir = LLVMPrintModuleToString(TheModule);
                printf("\nGenerated IR:\n%s\n", ir);
                LLVMDisposeMessage(ir);
            } else {
                printf("Code generation failed\n");
            }
        } else {
            printf("No codegen function for root node\n");
        }
    }

    // Cleanup
    if (tree) {
        // I need to implement proper AST cleanup functions
        // For now, just free the top node (incomplete cleanup)
        free(tree);
    }
    if (TheModule) LLVMDisposeModule(TheModule);
    if (Builder) LLVMDisposeBuilder(Builder);
    if (TheContext) LLVMContextDispose(TheContext);

    return 0;
}