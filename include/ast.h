// ast.h
#ifndef AST_H
#define AST_H

#include <string.h>

typedef enum {
    AST_IDENTIFIER,
    AST_ASSIGN,
    AST_NUMBER,
    AST_BINARY,
    AST_OBJECT, 
    AST_BLOCK,
    AST_FUNC
} ASTNodeType;

typedef struct ASTNode ASTNode; // Forward declaration

typedef struct {
    struct ASTNode** data;  // array of pointers to ExprAST
    size_t size;     // current number of elements
    size_t capacity; // allocated size
} ASTNodeArray;

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
        struct {                // For AST_OBJECT and AST_BLOCK
            struct ASTNode** statements; 
            int count; 
        };
        struct {                // For AST_FUNC
            struct ASTNode* funcName;
            ASTNodeArray* args;
            struct ASTNode* body;
        };
    };
} ASTNode;

ASTNode* new_number_node(int value);
ASTNode* new_binary_node(char op, ASTNode* left, ASTNode* right);
ASTNode* new_identifier_node(char* name);
ASTNode* new_assignment_node(ASTNode* varName, ASTNode* varValue);
ASTNode* new_object_node(ASTNode** statements, int count);
ASTNode* new_block_node(ASTNode** statements, int count);
ASTNode* new_func_node(ASTNode* funcName, ASTNodeArray* args, ASTNode *body);
ASTNodeArray* initASTNodeArray();
void pushExprAST(ASTNodeArray* arr, ASTNode* expr);
void freeASTNodeArray(ASTNodeArray* arr);
void print_ast(ASTNode* node, int depth);

#endif
