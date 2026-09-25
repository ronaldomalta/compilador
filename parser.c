#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

static void erro(Parser *p, const char *msg) {
    printf("Erro Sintatico [%d:%d]: %s ('%s')\n", p->token.linha, p->token.coluna, msg, p->token.lexema);
    exit(1);
}

static void avancar(Parser *p) { p->token = lexer_proximo_token(p->lx); }

static void consumir(Parser *p, TokenType t, const char *m) {
    if (p->token.tipo == t) avancar(p);
    else erro(p, m);
}

static void programa(Parser *p);
static void bloco(Parser *p);
static void decl_var(Parser *p);
static void lista_id(Parser *p);
static void tipo(Parser *p);
static void cmd_comp(Parser *p);
static void comandos(Parser *p);
static void comando(Parser *p);
static void atrib(Parser *p);
static void expr(Parser *p);
static void expr_simples(Parser *p);
static void termo(Parser *p);
static void fator(Parser *p);

static void programa(Parser *p) {
    consumir(p, TK_PROGRAM, "Esperado 'program'");
    consumir(p, TK_IDENTIFICADOR, "Esperado nome do programa");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
    bloco(p);
    consumir(p, TK_PONTO, "Esperado '.' no fim");
}

static void bloco(Parser *p) {
    if (p->token.tipo == TK_VAR) decl_var(p);
    cmd_comp(p);
}

static void decl_var(Parser *p) {
    consumir(p, TK_VAR, "Esperado 'var'");
    while (p->token.tipo == TK_IDENTIFICADOR) {
        lista_id(p);
        consumir(p, TK_DOIS_PONTOS, "Esperado ':'");
        tipo(p);
        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
    }
}

static void lista_id(Parser *p) {
    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    while (p->token.tipo == TK_VIRGULA) {
        avancar(p);
        consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    }
}

static void tipo(Parser *p) {
    if (p->token.tipo == TK_INTEGER || p->token.tipo == TK_REAL || p->token.tipo == TK_CHAR) avancar(p);
    else erro(p, "Esperado tipo valido");
}

static void cmd_comp(Parser *p) {
    consumir(p, TK_BEGIN, "Esperado 'begin'");
    comandos(p);
    consumir(p, TK_END, "Esperado 'end'");
}

static void comandos(Parser *p) {
    comando(p);
    while (p->token.tipo == TK_PONTO_VIRGULA) {
        avancar(p);
        if (p->token.tipo != TK_END && p->token.tipo != TK_UNTIL) comando(p);
    }
}

static void comando(Parser *p) {
    switch (p->token.tipo) {
        case TK_IDENTIFICADOR: atrib(p); break;
        case TK_IF:
            avancar(p); expr(p);
            consumir(p, TK_THEN, "Esperado 'then'"); comando(p);
            if (p->token.tipo == TK_ELSE) { avancar(p); comando(p); }
            break;
        case TK_WHILE:
            avancar(p); expr(p);
            consumir(p, TK_DO, "Esperado 'do'"); comando(p);
            break;
        case TK_REPEAT:
            avancar(p); comandos(p);
            consumir(p, TK_UNTIL, "Esperado 'until'"); expr(p);
            break;
        case TK_WRITE:
            avancar(p); consumir(p, TK_ABRE_PAR, "Esperado '('");
            expr(p);
            while (p->token.tipo == TK_VIRGULA) { avancar(p); expr(p); }
            consumir(p, TK_FECHA_PAR, "Esperado ')'");
            break;
        case TK_BEGIN: cmd_comp(p); break;
        default: break;
    }
}

static void atrib(Parser *p) {
    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    consumir(p, TK_ATRIBUICAO, "Esperado ':='");
    expr(p);
}

static void expr(Parser *p) {
    expr_simples(p);
    TokenType t = p->token.tipo;
    if (t == TK_IGUAL || t == TK_DIFERENTE || t == TK_MENOR || t == TK_MENOR_IGUAL || t == TK_MAIOR || t == TK_MAIOR_IGUAL) {
        avancar(p);
        expr_simples(p);
    }
}

static void expr_simples(Parser *p) {
    if (p->token.tipo == TK_MAIS || p->token.tipo == TK_MENOS) avancar(p);
    termo(p);
    while (p->token.tipo == TK_MAIS || p->token.tipo == TK_MENOS || p->token.tipo == TK_OR) {
        avancar(p);
        termo(p);
    }
}

static void termo(Parser *p) {
    fator(p);
    while (p->token.tipo == TK_MULT || p->token.tipo == TK_BARRA || p->token.tipo == TK_DIV || p->token.tipo == TK_AND) {
        avancar(p);
        fator(p);
    }
}

static void fator(Parser *p) {
    TokenType t = p->token.tipo;
    if (t == TK_IDENTIFICADOR || t == TK_INTEIRO_LITERAL || t == TK_REAL_LITERAL || t == TK_CHAR_LITERAL) {
        avancar(p);
    } else if (t == TK_ABRE_PAR) {
        avancar(p); expr(p);
        consumir(p, TK_FECHA_PAR, "Esperado ')'");
    } else if (t == TK_NOT) {
        avancar(p); fator(p);
    } else {
        erro(p, "Fator invalido");
    }
}

void parser_iniciar(Parser *p, Lexer *lx) {
    p->lx = lx;
    avancar(p);
}

int parser_analisar(Parser *p) {
    programa(p);
    if (p->token.tipo != TK_EOF) erro(p, "Codigo apos fim do programa");
    return 1;
}