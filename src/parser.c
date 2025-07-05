// parser.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/codegen.h"
#include "../include/token.h"
#include "../include/ast.h"

extern Token get_next_token();
Token current;

void static advance() {
    current = get_next_token();
}

void expect(TokenType type) {
    if (current.type != type) {
        fprintf(stderr, "Syntax error: expected token type %d, got %d\n", type, current.type);
        exit(1);
    }
    advance();
}

// Forward declarations
ASTNode* expr();
ASTNode* term();
ASTNode* factor();
ASTNode* statement();
ASTNode* object();
ASTNode* block();
ASTNodeArray* new_args();

ASTNode* parse() {
    advance(); // Load first token
    ASTNode** statements = NULL;
    int count = 0, capacity = 0;
    while (current.type != TOKEN_EOF) {
        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 4;
            statements = realloc(statements, capacity * sizeof(ASTNode*));
        }
        statements[count++] = statement();
    }
    return new_block_node(statements, count);
}


ASTNode* statement() {
    if (current.type == TOKEN_EXTERN) {
        advance();
        char* name = strdup(current.lexeme);
        advance();
        return new_extern_node(name);
    }
    
    if (current.type == TOKEN_IDENTIFIER) { 
        char* name = strdup(current.lexeme);
        advance();

        if (current.type == TOKEN_ASSIGN) {      // Assignment
            expect(TOKEN_ASSIGN);
            ASTNode* varName = new_identifier_node(name);
            ASTNode* varValue;
            if (current.type == TOKEN_LCURLY) {
                varValue = object();
            } else {
                varValue = expr();
            }
            return new_assignment_node(varName, varValue);
        } else if (current.type == TOKEN_LPAREN) {      // Function or Call
            expect(TOKEN_LPAREN);
            ASTNodeArray* args = new_args();
            
            if (current.type == TOKEN_LCURLY) {  // Function definition
                ASTNode* funcName = new_identifier_node(name);
                ASTNode* blockContent = block();
                return new_func_node(funcName, args, blockContent);
            } else {                            // Function call
                return new_call_node(name, args);
            }
        }
    }
   
    // For everything else, just parse as expression
    return expr();
}

ASTNode* expr() {
    ASTNode* node = term();
    while (current.type == TOKEN_PLUS || current.type == TOKEN_MINUS) {
        char op = current.lexeme[0];
        advance();
        node = new_binary_node(op, node, term());
    }
    return node;
}

ASTNode* term() {
    ASTNode* node = factor();
    while (current.type == TOKEN_STAR || current.type == TOKEN_SLASH) {
        char op = current.lexeme[0];
        advance();
        node = new_binary_node(op, node, factor());
    }
    return node;
}

ASTNode* factor() {
    if (current.type == TOKEN_NUM) {
        int value = atoi(current.lexeme);
        advance();
        return new_number_node(value);

    } else if (current.type == TOKEN_IDENTIFIER) {
        char* name = strdup(current.lexeme);
        advance();
        return new_identifier_node(name);

    } else if (current.type == TOKEN_LPAREN) {
        advance();
        ASTNode* node = expr();
        expect(TOKEN_RPAREN);
        return node;

    } else {
        fprintf(stderr, "Syntax error: unexpected token '%s'\n", current.lexeme);
        exit(1);
    }
}

ASTNode* object() {
    expect(TOKEN_LCURLY);
    ASTNode** statements = NULL;
    int count = 0, capacity = 0;
    while (current.type != TOKEN_RCURLY && current.type != TOKEN_EOF) {
        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 4;
            statements = realloc(statements, capacity * sizeof(ASTNode*));
        }
        statements[count++] = statement();
    }
    expect(TOKEN_RCURLY);
    return new_object_node(statements, count);
}

ASTNode* block() {
    expect(TOKEN_LCURLY);
    ASTNode** statements = NULL;
    int count = 0, capacity = 0;
    while (current.type != TOKEN_RCURLY && current.type != TOKEN_EOF) {
        if (count == capacity) {
            capacity = capacity ? capacity * 2 : 4;
            statements = realloc(statements, capacity * sizeof(ASTNode*));
        }
        statements[count++] = statement();
    }
    expect(TOKEN_RCURLY);
    return new_block_node(statements, count);
}

ASTNodeArray* new_args() {
    ASTNodeArray *args = initASTNodeArray();

    while (current.type != TOKEN_RPAREN) {
        if (current.type == TOKEN_IDENTIFIER || current.type == TOKEN_NUM) {
            pushASTNode(args, expr());
        } else if (current.type == TOKEN_COMA) {
            advance();
        }
    }

    advance();
    return args;
}
