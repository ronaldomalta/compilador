#ifndef TOKEN_H
#define TOKEN_H

#define MAX_LEXEMA 256

typedef enum {
    /* Palavras reservadas */
    TK_PROGRAM, TK_VAR, TK_INTEGER, TK_REAL, TK_CHAR,
    TK_BEGIN, TK_END, TK_IF, TK_THEN, TK_ELSE,
    TK_WHILE, TK_DO, TK_REPEAT, TK_UNTIL, TK_WRITE,
    TK_DIV, TK_AND, TK_OR, TK_NOT,

    /* Identificador e literais */
    TK_IDENTIFICADOR,
    TK_INTEIRO_LITERAL, TK_REAL_LITERAL, TK_CHAR_LITERAL,

    /* Operadores */
    TK_MENOR, TK_MAIOR, TK_MENOR_IGUAL, TK_MAIOR_IGUAL,
    TK_IGUAL, TK_DIFERENTE,
    TK_MAIS, TK_MENOS, TK_MULT, TK_BARRA,
    TK_ATRIBUICAO,

    /* Simbolos */
    TK_ABRE_PAR, TK_FECHA_PAR, TK_VIRGULA,
    TK_PONTO_VIRGULA, TK_PONTO, TK_DOIS_PONTOS,

    /* Controle */
    TK_EOF, TK_ERRO,
    TK_QUANTIDADE
} TokenType;

typedef struct {
    TokenType tipo;
    char lexema[MAX_LEXEMA];
    int linha;
    int coluna;
} Token;

const char *token_nome(TokenType tipo);

#endif