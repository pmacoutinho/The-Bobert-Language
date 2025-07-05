// ast.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/ast.h"
#include "../include/codegen.h"

ASTNode* new_number_node(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->value = value;
    node->codegen = number_codegen;
    return node;
}

ASTNode* new_binary_node(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY;
    node->op = op;
    node->left = left;
    node->right = right;
    node->codegen = binary_codegen;
    return node;
}

ASTNode* new_extern_node(char *name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_EXTERN;
    node->name = name;
    return node;
}

ASTNode* new_identifier_node(char *name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->name = name;
    node->codegen = identifier_codegen;
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

ASTNode* new_block_node(ASTNode** statements, int count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->statements = statements;
    node->count = count;
    return node;
}

ASTNode* new_func_node(ASTNode* funcName, ASTNodeArray* args, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_FUNC;
    node->funcName = funcName;
    node->args = args;      
    node->body = body;
    node->codegen = func_codegen;
    return node;
}

ASTNode* new_call_node(char* callee, ASTNodeArray* args) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_CALL;
    node->callee = callee;
    node->args = args;      
    node->codegen = call_codegen;
    return node;
}

ASTNodeArray* initASTNodeArray() {
    ASTNodeArray *arr = malloc(sizeof(ASTNodeArray));
    arr->size = 0;
    arr->capacity = 4; // initial capacity
    arr->data = malloc(sizeof(ASTNode*) * arr->capacity);
    return arr;
}

void pushASTNode(ASTNodeArray* arr, ASTNode* expr) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = realloc(arr->data, sizeof(ASTNode*) * arr->capacity);
    }
    arr->data[arr->size++] = expr;
}

void freeASTNodeArray(ASTNodeArray* arr) {
    for (size_t i = 0; i < arr->size; ++i) {
        free(arr->data[i]);  // free each ASTNode
    }
    free(arr->data);         // free the array itself
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
        case AST_EXTERN:
            printf("Extern: %s\n", node->name);
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
        case AST_BLOCK:
            printf("Block:\n");
            for (int i = 0; i < node->count; i++)
                print_ast(node->statements[i], depth + 1);
            break;
        case AST_FUNC:
            printf("Function:\n");
            print_ast(node->funcName, depth + 1);
            printf("  Parameters:\n");
            for (size_t i = 0; i < node->args->size; ++i) {
                print_ast(node->args->data[i], depth + 2);
            }
            print_ast(node->body, depth + 1);
            break;
        case AST_CALL:
            printf("Call: %s\n", node->callee);
            printf("      Arguments:\n");
            for (size_t i = 0; i < node->args->size; ++i) {
                print_ast(node->args->data[i], depth + 2);
            }
            break;
        default:
            printf("Unknown node type\n");
    }
}
