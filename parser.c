#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

static void erro(Parser *p, const char *msg) {
    (void)msg;
    printf("Erro de sintaxe no token [%s]\n", p->token.lexema);
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
static void expr(Parser *p);
static void expr_logica(Parser *p);
static void expr_relacional(Parser *p);
static void expr_aditiva(Parser *p);
static void expr_multiplicativa(Parser *p);
static void expr_basica(Parser *p);

static void atribuicao(Parser *p);
static void decisao(Parser *p);
static void iteracao(Parser *p);
static void escrita(Parser *p);

static void programa(Parser *p) {
    consumir(p, TK_PROGRAM, "Esperado 'program'");
    consumir(p, TK_IDENTIFICADOR, "Esperado nome do programa");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
    bloco(p);
    consumir(p, TK_PONTO, "Esperado '.' no fim");
}

static void bloco(Parser *p) {
    decl_var(p);
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
    while (p->token.tipo == TK_BEGIN ||
           p->token.tipo == TK_IDENTIFICADOR ||
           p->token.tipo == TK_WHILE ||
           p->token.tipo == TK_REPEAT ||
           p->token.tipo == TK_IF ||
           p->token.tipo == TK_WRITE) {

        comando(p);
    }
}
static void comando(Parser *p) {
    switch (p->token.tipo) {

        case TK_BEGIN:
            cmd_comp(p);
            consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
            break;

        case TK_IDENTIFICADOR:
            atribuicao(p);
            break;

        case TK_WHILE:
        case TK_REPEAT:
            iteracao(p);
            break;

        case TK_IF:
            decisao(p);
            break;

        case TK_WRITE:
            escrita(p);
            break;

        default:
            erro(p, "Comando invalido");
    }
}
static void atribuicao(Parser *p) {
    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    consumir(p, TK_ATRIBUICAO, "Esperado ':='");
    expr(p);
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
}

static void decisao(Parser *p) {
    consumir(p, TK_IF, "Esperado 'if'");
    expr(p);
    consumir(p, TK_THEN, "Esperado 'then'");
    comando(p);

    if (p->token.tipo == TK_ELSE) {
        avancar(p);
        comando(p);
    }
}
static void iteracao(Parser *p) {
    if (p->token.tipo == TK_WHILE) {
        avancar(p);
        expr(p);
        consumir(p, TK_DO, "Esperado 'do'");
        comando(p);

    } else if (p->token.tipo == TK_REPEAT) {
        avancar(p);
        comando(p);
        consumir(p, TK_UNTIL, "Esperado 'until'");
        expr(p);
        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    } else {
        erro(p, "Esperado 'while' ou 'repeat'");
    }
}

static void escrita(Parser *p) {
    consumir(p, TK_WRITE, "Esperado 'write'");
    consumir(p, TK_ABRE_PAR, "Esperado '('");
    expr(p);
    consumir(p, TK_FECHA_PAR, "Esperado ')'");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
}

static void expr(Parser *p) {
    expr_logica(p);
}

static void expr_logica(Parser *p) {
    expr_relacional(p);

    while (p->token.tipo == TK_OR || p->token.tipo == TK_AND) {
        avancar(p);
        expr_relacional(p);
    }
}

static void expr_relacional(Parser *p) {
    expr_aditiva(p);

    while (p->token.tipo == TK_IGUAL ||
           p->token.tipo == TK_DIFERENTE ||
           p->token.tipo == TK_MENOR ||
           p->token.tipo == TK_MENOR_IGUAL ||
           p->token.tipo == TK_MAIOR ||
           p->token.tipo == TK_MAIOR_IGUAL) {

        avancar(p);
        expr_aditiva(p);
    }
}

static void expr_aditiva(Parser *p) {
    expr_multiplicativa(p);

    while (p->token.tipo == TK_MAIS || p->token.tipo == TK_MENOS) {
        avancar(p);
        expr_multiplicativa(p);
    }
}

static void expr_multiplicativa(Parser *p) {
    expr_basica(p);

    while (p->token.tipo == TK_MULT ||
           p->token.tipo == TK_BARRA ||
           p->token.tipo == TK_DIV) {

        avancar(p);
        expr_basica(p);
    }
}

static void expr_basica(Parser *p) {
    TokenType t = p->token.tipo;

    if (t == TK_IDENTIFICADOR ||
        t == TK_INTEIRO_LITERAL ||
        t == TK_REAL_LITERAL ||
        t == TK_CHAR_LITERAL) {

        avancar(p);

    } else if (t == TK_ABRE_PAR) {

        avancar(p);
        expr(p);
        consumir(p, TK_FECHA_PAR, "Esperado ')'");

    } else if (t == TK_NOT) {

        avancar(p);
        expr(p);

    } else {

        erro(p, "Expressao invalida");
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
