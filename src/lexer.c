// lexer.c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../include/token.h"

const char* source;
int pos = 0;
int line = 1;

char peek() {
    return source[pos];
}

char advance() {
    return source[pos++];
}

void skip_whitespace() {
    while (isspace(peek())) {
        if (peek() == '\n') line++;
        advance();
    }
}

Token make_token(TokenType type, const char* start, int length) {
    Token token;
    token.type = type;
    strncpy(token.lexeme, start, length);
    token.lexeme[length] = '\0';
    token.line = line;
    return token;
}

Token get_next_token() {
    skip_whitespace();
    char c = peek();

    if (c == '\0') return make_token(TOKEN_EOF, "", 0);

    if (isalpha(c)) {
        const char* start = &source[pos];
        while (isalnum(peek())) advance();
        return make_token(TOKEN_IDENTIFIER, start, &source[pos] - start);
    }

    if (isdigit(c)) {
        const char* start = &source[pos];
        while (isdigit(peek())) advance();
        return make_token(TOKEN_INT, start, &source[pos] - start);
    }

    switch (advance()) {
        case '+': return make_token(TOKEN_PLUS, "+", 1);
        case '-': return make_token(TOKEN_MINUS, "-", 1);
        case '*': return make_token(TOKEN_STAR, "*", 1);
        case '/': return make_token(TOKEN_SLASH, "/", 1);
        case '=': return make_token(TOKEN_ASSIGN, "=", 1);
        case '(': return make_token(TOKEN_LPAREN, "(", 1);
        case ')': return make_token(TOKEN_RPAREN, ")", 1);
        case '{': return make_token(TOKEN_LCURLY, "{", 1);
        case '}': return make_token(TOKEN_RCURLY, "}", 1);
        case '[': return make_token(TOKEN_LBRACKET, "[", 1);
        case ']': return make_token(TOKEN_RBRACKET, "]", 1);
        case ',': return make_token(TOKEN_COMA, ",", 1);
        default: {
            char unknown = c;
            return make_token(TOKEN_UNKNOWN, &unknown, 1);
        }
    }
}
