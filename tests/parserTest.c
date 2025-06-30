// main.c
#include <stdio.h>
#include "../include/token.h"
#include "../include/ast.h"

extern const char* source;
extern ASTNode* parse();
extern void print_ast(ASTNode* node, int depth);

int main() {
    source = "func(y, z) { math = { x = z + 3 * (1 - 2) / y} }";
    ASTNode* tree = parse();
    print_ast(tree, 0);
    return 0;
}
