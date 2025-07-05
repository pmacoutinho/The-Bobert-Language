// main.c
#include <stdio.h>
#include "../include/token.h"
#include "../include/ast.h"

extern const char* source;
extern ASTNode* parse();
extern void print_ast(ASTNode* node, int depth);

int main() {
    /*Possible tests:
        extern other\nfunc(y, z) {\n math = { x = z + 3 * (1 - 2) / y} \n}\nfunc2(x, y, z) {z = y + x}
        
        fib(x) { fib(x-1)+fib(x-2) }
    */
    source = "fib(x) { fib(x-1) + fib(x-2) }";
    ASTNode* tree = parse();
    print_ast(tree, 0);
    return 0;
}
