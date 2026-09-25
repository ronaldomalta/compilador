#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


static const struct { const char *palavra; TokenType tipo; } RESERVADAS[] = {
    {"program", TK_PROGRAM}, {"var", TK_VAR}, {"integer", TK_INTEGER},
    {"real", TK_REAL}, {"char", TK_CHAR}, {"begin", TK_BEGIN},
    {"end", TK_END}, {"if", TK_IF}, {"then", TK_THEN},
    {"else", TK_ELSE}, {"while", TK_WHILE}, {"do", TK_DO},
    {"repeat", TK_REPEAT}, {"until", TK_UNTIL}, {"write", TK_WRITE},
    {"div", TK_DIV}, {"and", TK_AND}, {"or", TK_OR}, {"not", TK_NOT},
};
#define QTD_RESERVADAS (sizeof(RESERVADAS) / sizeof(RESERVADAS[0]))


const char *token_nome(TokenType tipo) {
    for (size_t i = 0; i < QTD_RESERVADAS; i++)
        if (RESERVADAS[i].tipo == tipo) return RESERVADAS[i].palavra;

    switch (tipo) {
        case TK_IDENTIFICADOR:   return "IDENTIFICADOR";
        case TK_INTEIRO_LITERAL: return "INTEIRO_LITERAL";
        case TK_REAL_LITERAL:    return "REAL_LITERAL";
        case TK_CHAR_LITERAL:    return "CHAR_LITERAL";
        case TK_MENOR:           return "MENOR";
        case TK_MAIOR:           return "MAIOR";
        case TK_MENOR_IGUAL:     return "MENOR_IGUAL";
        case TK_MAIOR_IGUAL:     return "MAIOR_IGUAL";
        case TK_IGUAL:           return "IGUAL";
        case TK_DIFERENTE:       return "DIFERENTE";
        case TK_MAIS:            return "MAIS";
        case TK_MENOS:           return "MENOS";
        case TK_MULT:            return "MULT";
        case TK_BARRA:           return "BARRA";
        case TK_ATRIBUICAO:      return "ATRIBUICAO";
        case TK_ABRE_PAR:        return "ABRE_PAR";
        case TK_FECHA_PAR:       return "FECHA_PAR";
        case TK_VIRGULA:         return "VIRGULA";
        case TK_PONTO_VIRGULA:   return "PONTO_VIRGULA";
        case TK_PONTO:           return "PONTO";
        case TK_DOIS_PONTOS:     return "DOIS_PONTOS";
        case TK_EOF:             return "EOF";
        case TK_ERRO:            return "ERRO";
        default:                 return "DESCONHECIDO";
    }
}


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


static char atual(Lexer *lx) {
    return lx->fonte[lx->pos];   
}

static char avancar(Lexer *lx) {
    char c = lx->fonte[lx->pos++];
    if (c == '\n') { lx->linha++; lx->coluna = 1; }
    else lx->coluna++;
    return c;
}


static Token ler_identificador(Lexer *lx) {
    Token t = {TK_IDENTIFICADOR, "", lx->linha, lx->coluna};
    int i = 0;

    while (isalnum((unsigned char)atual(lx)) || atual(lx) == '_') {
        char c = avancar(lx);
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = c;
    }
    t.lexema[i] = '\0';

    for (size_t k = 0; k < QTD_RESERVADAS; k++)
        if (strcmp(t.lexema, RESERVADAS[k].palavra) == 0)
            t.tipo = RESERVADAS[k].tipo;

    return t;
}


static Token ler_numero(Lexer *lx) {
    Token t = {TK_INTEIRO_LITERAL, "", lx->linha, lx->coluna};
    int i = 0;

    while (isdigit((unsigned char)atual(lx))) {
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = avancar(lx);
        else avancar(lx);
    }

    
    if (atual(lx) == '.' && isdigit((unsigned char)lx->fonte[lx->pos + 1])) {
        t.tipo = TK_REAL_LITERAL;
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = avancar(lx); // Consome o '.'

        while (isdigit((unsigned char)atual(lx))) {
            if (i < MAX_LEXEMA - 1) t.lexema[i++] = avancar(lx);
            else avancar(lx);
        }
    }

    t.lexema[i] = '\0';
    return t;
}


static Token ler_caractere(Lexer *lx) {
    Token t = {TK_CHAR_LITERAL, "", lx->linha, lx->coluna};
    int i = 0;

    t.lexema[i++] = avancar(lx); 

    if (atual(lx) != '\'' && atual(lx) != '\0') {
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = avancar(lx);
    }

    if (atual(lx) == '\'') {
        if (i < MAX_LEXEMA - 1) t.lexema[i++] = avancar(lx); // Consome aspas simples de fechamento
    } else {
        printf("Erro léxico no caracter [%c]\n", atual(lx));
        t.tipo = TK_ERRO;
    }

    t.lexema[i] = '\0';
    return t;
}


Token lexer_proximo_token(Lexer *lx) {
    
    while (atual(lx) == ' ' || atual(lx) == '\t' || atual(lx) == '\r' || atual(lx) == '\n') {
        avancar(lx);
    }

    int linha = lx->linha;
    int coluna = lx->coluna;
    char c = atual(lx);

    if (c == '\0') {
        Token t = {TK_EOF, "EOF", linha, coluna};
        return t;
    }

    if (isalpha((unsigned char)c) || c == '_')
        return ler_identificador(lx);

    if (isdigit((unsigned char)c))
        return ler_numero(lx);

    if (c == '\'')
        return ler_caractere(lx);

    
    switch (c) {
        case '<':
            avancar(lx);
            if (atual(lx) == '=') { avancar(lx); return (Token){TK_MENOR_IGUAL, "<=", linha, coluna}; }
            if (atual(lx) == '>') { avancar(lx); return (Token){TK_DIFERENTE, "<>", linha, coluna}; }
            return (Token){TK_MENOR, "<", linha, coluna};

        case '>':
            avancar(lx);
            if (atual(lx) == '=') { avancar(lx); return (Token){TK_MAIOR_IGUAL, ">=", linha, coluna}; }
            return (Token){TK_MAIOR, ">", linha, coluna};

        case '=':
            avancar(lx);
            return (Token){TK_IGUAL, "=", linha, coluna};

        case ':':
            avancar(lx);
            if (atual(lx) == '=') { avancar(lx); return (Token){TK_ATRIBUICAO, ":=", linha, coluna}; }
            return (Token){TK_DOIS_PONTOS, ":", linha, coluna};

        case '+':
            avancar(lx);
            return (Token){TK_MAIS, "+", linha, coluna};

        case '-':
            avancar(lx);
            return (Token){TK_MENOS, "-", linha, coluna};

        case '*':
            avancar(lx);
            return (Token){TK_MULT, "*", linha, coluna};

        case '/':
            avancar(lx);
            return (Token){TK_BARRA, "/", linha, coluna};

        case '(':
            avancar(lx);
            return (Token){TK_ABRE_PAR, "(", linha, coluna};

        case ')':
            avancar(lx);
            return (Token){TK_FECHA_PAR, ")", linha, coluna};

        case ',':
            avancar(lx);
            return (Token){TK_VIRGULA, ",", linha, coluna};

        case ';':
            avancar(lx);
            return (Token){TK_PONTO_VIRGULA, ";", linha, coluna};

        case '.':
            avancar(lx);
            return (Token){TK_PONTO, ".", linha, coluna};

        default:
            
            c = avancar(lx);
            printf("Erro léxico no caracter [%c]\n", c);
            Token t = {TK_ERRO, "", linha, coluna};
            t.lexema[0] = c;
            t.lexema[1] = '\0';
            return t;
    }
}