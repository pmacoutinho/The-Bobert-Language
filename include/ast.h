// ast.h
#ifndef AST_H
#define AST_H

#include <string.h>
#include <llvm-c/Core.h>

typedef enum {
    AST_EXTERN,
    AST_IDENTIFIER,
    AST_ASSIGN,
    AST_NUMBER,
    AST_BINARY,
    AST_OBJECT, 
    AST_BLOCK,
    AST_PROTO,
    AST_FUNC,
    AST_CALL
} ASTNodeType;

typedef struct ASTNode ASTNode; // Forward declaration
typedef LLVMValueRef (*CodegenFunc)(ASTNode*);

typedef struct {
    struct ASTNode** data;  // array of pointers to ASTNode
    size_t size;     // current number of elements
    size_t capacity; // allocated size
} ASTNodeArray;

typedef struct ASTNode {
    ASTNodeType type;
    CodegenFunc codegen;
    union {
        struct {                // For AST_NUMBER
            int value;
        };
        struct {                // For AST_BINARY
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        };
        struct {                // For AST_IDENTIFIER AND AST_EXTERN
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
        struct {                // For AST_PROTO
            char* funcName;
            ASTNodeArray* args;
        };
        struct {                // For AST_FUNC
            ASTNode* prototype;
            struct ASTNode* body;
        };
        struct {                // For AST_EXTERN
            struct ASTNode* call;
        };
        struct {                // For AST_CALL
            char* callee;
        };
    };
} ASTNode;

ASTNode* new_number_node(int value);
ASTNode* new_binary_node(char op, ASTNode* left, ASTNode* right);
ASTNode* new_extern_node(ASTNode* call);
ASTNode* new_identifier_node(char* name);
ASTNode* new_assignment_node(ASTNode* varName, ASTNode* varValue);
ASTNode* new_object_node(ASTNode** statements, int count);
ASTNode* new_block_node(ASTNode** statements, int count);
ASTNode* new_prototype_node(char* funcName, ASTNodeArray* args);
ASTNode* new_func_node(ASTNode* prototype, ASTNode *body);
ASTNode* new_call_node(char* callee, ASTNodeArray* args);
ASTNodeArray* initASTNodeArray();
void pushASTNode(ASTNodeArray* arr, ASTNode* expr);
void freeASTNodeArray(ASTNodeArray* arr);
void print_ast(ASTNode* node, int depth);

#endif
