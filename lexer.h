#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include "token.h"

typedef struct {
    char  *fonte;
    size_t tamanho;
    size_t pos;
    int    linha;
    int    coluna;
} Lexer;

int   lexer_iniciar(Lexer *lx, const char *caminho);
void  lexer_liberar(Lexer *lx);
Token lexer_proximo_token(Lexer *lx);

#endif