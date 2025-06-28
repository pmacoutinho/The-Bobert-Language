// ast.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/ast.h"

ASTNode* new_number_node(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->value = value;
    return node;
}

ASTNode* new_binary_node(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY;
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* new_identifier_node(char *name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->name = name;
    return node;
}

ASTNode* new_assignment_node(ASTNode* varName, ASTNode* varValue) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN;
    node->assign = '=';
    node->varName = varName;
    node->varValue = varValue;
    return node;
}

ASTNode* new_object_node(ASTNode** statements, int count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_OBJECT;
    node->statements = statements;
    node->count = count;
    return node;
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    switch (node->type) {
        case AST_NUMBER:
            printf("Number: %d\n", node->value);
            break;
        case AST_BINARY:
            printf("Binary: %c\n", node->op);
            print_ast(node->left, depth + 1);
            print_ast(node->right, depth + 1);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->name);
            break;
        case AST_ASSIGN:
            printf("Assignment: %c\n", node->assign);
            print_ast(node->varName, depth + 1);
            print_ast(node->varValue, depth + 1);
            break;
        case AST_OBJECT:
            printf("Object:\n");
            for (int i = 0; i < node->count; i++)
                print_ast(node->statements[i], depth + 1);
            break;
        default:
            printf("Unknown node type\n");
    }
}
