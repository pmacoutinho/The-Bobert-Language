// parser.c
#include <stdio.h>
#include <stdlib.h>
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

ASTNode* parse() {
    advance(); // Load first token
    return statement();
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
    if (current.type == TOKEN_INT) {
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

ASTNode* statement() {
    if (current.type == TOKEN_IDENTIFIER) {
        char* name = strdup(current.lexeme);
        advance();

        if (current.type == TOKEN_ASSIGN) {
            expect(TOKEN_ASSIGN);
            ASTNode* varName = new_identifier_node(name);

            ASTNode* varValue;
            if (current.type == TOKEN_LCURLY) {
                varValue = object(); // Only allow object as assignment value
            } else {
                varValue = expr();
            }
            return new_assignment_node(varName, varValue);
        } else {    // If it's a function declaration
            ASTNode* blockName = new_identifier_node(name);
            ASTNode* blockContent = parse();
            return new_block_node(blockName, blockContent);
        }
    } else {
        return expr();
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
