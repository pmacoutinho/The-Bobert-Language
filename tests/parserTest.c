// main.c
#include <stdio.h>
#include "../include/token.h"
#include "../include/ast.h"

extern const char* source;
extern ASTNode* parse();
extern void print_ast(ASTNode* node, int depth);

int main() {
    source = "math = { x = 5 + 3 * (1 - 2) / 4 }";
    ASTNode* tree = parse();
    print_ast(tree, 0);
    return 0;
}
