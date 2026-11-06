#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lx;
    Token token;
} Parser;


void parser_iniciar(Parser *p, Lexer *lx);

/* Analisa o programa e devolve a raiz da AST (NODE_PROGRAMA).
   Em caso de erro de sintaxe, mostra a mensagem e encerra com exit(1). */
Node *parser_analisar(Parser *p);

#endif
