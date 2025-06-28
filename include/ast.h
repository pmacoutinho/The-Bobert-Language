// ast.h
#ifndef AST_H
#define AST_H

#include <string.h>

typedef enum {
    AST_IDENTIFIER,
    AST_ASSIGN,
    AST_NUMBER,
    AST_BINARY,
    AST_BLOCK
} ASTNodeType;

typedef struct ASTNode ASTNode; // Add this line before the struct definition

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {                // For AST_NUMBER
            int value;
        };
        struct {                // For AST_BINARY
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        };
        struct {                // For AST_IDENTIFIER
            char* name;
        };
        struct {                // For AST_ASSIGN
            struct ASTNode* varName;
            char assign;
            struct ASTNode* varValue;
        };
        struct {                // For AST_BLOCK
            ASTNode** statements; 
            int count; 
        }; 
    };
} ASTNode;

ASTNode* new_number_node(int value);
ASTNode* new_binary_node(char op, ASTNode* left, ASTNode* right);
ASTNode* new_identifier_node(char* name);
ASTNode* new_assignment_node(ASTNode* varName, ASTNode* varValue);
ASTNode* new_block_node(ASTNode** statements, int count);
void print_ast(ASTNode* node, int depth);

#endif
