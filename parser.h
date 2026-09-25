#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    Lexer *lx;
    Token  token;
} Parser;

void parser_iniciar(Parser *p, Lexer *lx);
void parser_avancar(Parser *p);

#endif