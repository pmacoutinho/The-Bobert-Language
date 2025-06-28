// main.c
#include <stdio.h>
#include "../include/token.h"

// Declare this globally for simplicity
extern const char* source;
extern Token get_next_token();

int main() {
    /*
        math = {
            x = 3 + 42 * (s - 7)
            y = 4 / [1, 2, 3]
        }
    */
    source = "math = {\nx = 3 + 42 * (s - 7)\ny = 4 / [1, 2, 3]\n}";

    Token token;
    do {
        token = get_next_token();
        printf("Token: %-15s Lexeme: '%s' Line: %d\n",
            (char*[]){
                "EOF", "INT", "IDENTIFIER", "PLUS", "MINUS", 
                "STAR", "SLASH", "ASSIGN", "LPAREN", "RPAREN", 
                "LCURLY", "RCURLY", "LBRACKET", "RBRACKET", 
                "COMA", "UNKNOWN"
            }[token.type],
            token.lexeme,
            token.line
        );
    } while (token.type != TOKEN_EOF);

    return 0;
}
