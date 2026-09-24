#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// ---------- Palavras reservadas ---------- 
static const struct { const char *palavra; TokenType tipo; } RESERVADAS[] = {
    {"program", TK_PROGRAM}, {"var", TK_VAR}, {"integer", TK_INTEGER},
    {"real", TK_REAL}, {"char", TK_CHAR}, {"begin", TK_BEGIN},
    {"end", TK_END}, {"if", TK_IF}, {"then", TK_THEN},
    {"else", TK_ELSE}, {"while", TK_WHILE}, {"do", TK_DO},
    {"repeat", TK_REPEAT}, {"until", TK_UNTIL}, {"write", TK_WRITE},
    {"div", TK_DIV}, {"and", TK_AND}, {"or", TK_OR}, {"not", TK_NOT},
};
#define QTD_RESERVADAS (sizeof(RESERVADAS) / sizeof(RESERVADAS[0]))

// Nome do token: reaproveita a tabela de reservadas 
const char *token_nome(TokenType tipo) {
    for (size_t i = 0; i < QTD_RESERVADAS; i++)
        if (RESERVADAS[i].tipo == tipo) return RESERVADAS[i].palavra;
    if (tipo == TK_IDENTIFICADOR) return "IDENTIFICADOR";
    if (tipo == TK_EOF) return "EOF";
    return "DESCONHECIDO";
}

// ---------- Ler o arquivo ---------- 
int lexer_iniciar(Lexer *lx, const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (!f) { perror(caminho); return 0; }

    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    rewind(f);

    lx->fonte = malloc(n + 1);
    lx->tamanho = fread(lx->fonte, 1, n, f);
    lx->fonte[lx->tamanho] = '\0';
    fclose(f);

    lx->pos = 0;
    lx->linha = lx->coluna = 1;
    return 1;
}

void lexer_liberar(Lexer *lx) {
    free(lx->fonte);
}

// ---------- Andar caractere a caractere ---------- 
static char atual(Lexer *lx) {
    return lx->fonte[lx->pos];   
}

static char avancar(Lexer *lx) {
    char c = lx->fonte[lx->pos++];
    if (c == '\n') { lx->linha++; lx->coluna = 1; }
    else lx->coluna++;
    return c;
}

// ---------- Identificador ou palavra reservada ---------- 
static Token ler_identificador(Lexer *lx) {
    Token t = {TK_IDENTIFICADOR, "", lx->linha, lx->coluna};
    int i = 0;

    /* letra (letra | digito)*  — letra = [a-zA-Z_] */
    while (isalnum((unsigned char)atual(lx)) || atual(lx) == '_') {
        char c = avancar(lx);
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = c;
    }
    t.lexema[i] = '\0';

    // strcmp diferencia maiusculas: "begin" eh reservada, "Begin" nao 
    for (size_t k = 0; k < QTD_RESERVADAS; k++)
        if (strcmp(t.lexema, RESERVADAS[k].palavra) == 0)
            t.tipo = RESERVADAS[k].tipo;

    return t;
}

// ---------- Funcao principal ---------- 
Token lexer_proximo_token(Lexer *lx) {
    // Pessoa 2: ignorar espacos, \n, \t, \r 

    char c = atual(lx);

    if (c == '\0') {
        Token t = {TK_EOF, "EOF", lx->linha, lx->coluna};
        return t;
    }

    if (isalpha((unsigned char)c) || c == '_')
        return ler_identificador(lx);

    // Pessoa 2: numeros, chars, operadores, simbolos e erro lexico 
}