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

static Node *programa(Parser *p);
static Node *decl_var(Parser *p);
static Node *lista_id(Parser *p);
static TipoDado tipo(Parser *p);
static Node *cmd_comp(Parser *p);
static Node *comandos(Parser *p);
static Node *comando(Parser *p);
static Node *atribuicao(Parser *p);
static Node *decisao(Parser *p);
static Node *iteracao(Parser *p);
static Node *escrita(Parser *p);
static Node *expr(Parser *p);
static Node *expr_logica(Parser *p);
static Node *expr_relacional(Parser *p);
static Node *expr_aditiva(Parser *p);
static Node *expr_multiplicativa(Parser *p);
static Node *expr_basica(Parser *p);

/* Cria um no de operacao binaria a partir do token do operador (atual) e
   avanca. Como o laco de cada nivel usa o no anterior como filho esquerdo,
   a arvore fica associativa a esquerda, como pede o enunciado. */
static Node *nova_binaria(Parser *p, Node *esq) {
    Node *op = ast_novo(NODE_BINARIA, p->token.lexema, p->token.linha);
    avancar(p);
    op->f1 = esq;
    return op;
}

/* PROGRAMA: texto=nome, f1=lista de DECL, f2=BLOCO */
static Node *programa(Parser *p) {
    consumir(p, TK_PROGRAM, "Esperado 'program'");

    Node *prog = ast_novo(NODE_PROGRAMA, p->token.lexema, p->token.linha);
    consumir(p, TK_IDENTIFICADOR, "Esperado nome do programa");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    prog->f1 = decl_var(p);
    prog->f2 = cmd_comp(p);

    consumir(p, TK_PONTO, "Esperado '.' no fim");
    return prog;
}

static Node *decl_var(Parser *p) {
    Node *lista = NULL;

    consumir(p, TK_VAR, "Esperado 'var'");
    while (p->token.tipo == TK_IDENTIFICADOR) {
        Node *grupo = lista_id(p);
        consumir(p, TK_DOIS_PONTOS, "Esperado ':'");

        TipoDado t = tipo(p);
        for (Node *d = grupo; d; d = d->prox) d->tipo = t;

        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
        lista = ast_anexar(lista, grupo);
    }
    return lista;
}

/* DECL: texto=nome, tipo=tipo declarado. Um no por variavel, mesmo em
   "a, b : integer;". Declaracoes duplicadas ficam na lista: quem as
   detecta e a tabela de simbolos. */
static Node *lista_id(Parser *p) {
    Node *lista = ast_novo(NODE_DECL, p->token.lexema, p->token.linha);
    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");

    while (p->token.tipo == TK_VIRGULA) {
        avancar(p);
        Node *d = ast_novo(NODE_DECL, p->token.lexema, p->token.linha);
        consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
        lista = ast_anexar(lista, d);
    }
    return lista;
}

static TipoDado tipo(Parser *p) {
    TipoDado t = TIPO_INDEFINIDO;

    switch (p->token.tipo) {
        case TK_INTEGER: t = TIPO_INTEGER; break;
        case TK_REAL:    t = TIPO_REAL;    break;
        case TK_CHAR:    t = TIPO_CHAR;    break;
        default:         erro(p, "Esperado tipo valido");
    }
    avancar(p);
    return t;
}

/* BLOCO: f1=lista de comandos (ligados por prox) */
static Node *cmd_comp(Parser *p) {
    Node *bloco = ast_novo(NODE_BLOCO, "", p->token.linha);

    consumir(p, TK_BEGIN, "Esperado 'begin'");
    bloco->f1 = comandos(p);
    consumir(p, TK_END, "Esperado 'end'");
    return bloco;
}

static Node *comandos(Parser *p) {
    Node *lista = NULL;

    while (p->token.tipo == TK_BEGIN ||
           p->token.tipo == TK_IDENTIFICADOR ||
           p->token.tipo == TK_WHILE ||
           p->token.tipo == TK_REPEAT ||
           p->token.tipo == TK_IF ||
           p->token.tipo == TK_WRITE) {

        lista = ast_anexar(lista, comando(p));
    }
    return lista;
}

static Node *comando(Parser *p) {
    Node *c = NULL;

    switch (p->token.tipo) {

        case TK_BEGIN:
            c = cmd_comp(p);
            consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
            break;

        case TK_IDENTIFICADOR:
            c = atribuicao(p);
            break;

        case TK_WHILE:
        case TK_REPEAT:
            c = iteracao(p);
            break;

        case TK_IF:
            c = decisao(p);
            break;

        case TK_WRITE:
            c = escrita(p);
            break;

        default:
            erro(p, "Comando invalido");
    }
    return c;
}

/* ATRIB: texto=variavel, f1=expressao */
static Node *atribuicao(Parser *p) {
    Node *a = ast_novo(NODE_ATRIB, p->token.lexema, p->token.linha);

    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    consumir(p, TK_ATRIBUICAO, "Esperado ':='");
    a->f1 = expr(p);
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
    return a;
}

/* IF: f1=condicao, f2=then, f3=else (ou NULL) */
static Node *decisao(Parser *p) {
    Node *d = ast_novo(NODE_IF, "", p->token.linha);

    consumir(p, TK_IF, "Esperado 'if'");
    d->f1 = expr(p);
    consumir(p, TK_THEN, "Esperado 'then'");
    d->f2 = comando(p);

    if (p->token.tipo == TK_ELSE) {
        avancar(p);
        d->f3 = comando(p);
    }
    return d;
}

/* WHILE:  f1=condicao, f2=corpo
   REPEAT: f1=corpo,    f2=condicao */
static Node *iteracao(Parser *p) {
    Node *n = NULL;

    if (p->token.tipo == TK_WHILE) {
        n = ast_novo(NODE_WHILE, "", p->token.linha);
        avancar(p);
        n->f1 = expr(p);
        consumir(p, TK_DO, "Esperado 'do'");
        n->f2 = comando(p);

    } else if (p->token.tipo == TK_REPEAT) {
        n = ast_novo(NODE_REPEAT, "", p->token.linha);
        avancar(p);
        n->f1 = comando(p);
        consumir(p, TK_UNTIL, "Esperado 'until'");
        n->f2 = expr(p);
        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    } else {
        erro(p, "Esperado 'while' ou 'repeat'");
    }
    return n;
}

/* WRITE: f1=expressao */
static Node *escrita(Parser *p) {
    Node *w = ast_novo(NODE_WRITE, "", p->token.linha);

    consumir(p, TK_WRITE, "Esperado 'write'");
    consumir(p, TK_ABRE_PAR, "Esperado '('");
    w->f1 = expr(p);
    consumir(p, TK_FECHA_PAR, "Esperado ')'");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");
    return w;
}

static Node *expr(Parser *p) {
    return expr_logica(p);
}

/* Cada nivel abaixo chama o nivel de precedencia maior, entao quem esta mais
   "fundo" na recursao fica mais perto das folhas e e avaliado primeiro.

   BINARIA: texto=operador, f1=esquerda, f2=direita */

/* Nivel 4 do enunciado: or e and tem a MESMA precedencia. */
static Node *expr_logica(Parser *p) {
    Node *esq = expr_relacional(p);

    while (p->token.tipo == TK_OR || p->token.tipo == TK_AND) {
        Node *op = nova_binaria(p, esq);
        op->f2 = expr_relacional(p);
        esq = op;
    }
    return esq;
}

/* Nivel 3: =, <>, <, <=, >, >= */
static Node *expr_relacional(Parser *p) {
    Node *esq = expr_aditiva(p);

    while (p->token.tipo == TK_IGUAL ||
           p->token.tipo == TK_DIFERENTE ||
           p->token.tipo == TK_MENOR ||
           p->token.tipo == TK_MENOR_IGUAL ||
           p->token.tipo == TK_MAIOR ||
           p->token.tipo == TK_MAIOR_IGUAL) {

        Node *op = nova_binaria(p, esq);
        op->f2 = expr_aditiva(p);
        esq = op;
    }
    return esq;
}

/* Nivel 2: +, - */
static Node *expr_aditiva(Parser *p) {
    Node *esq = expr_multiplicativa(p);

    while (p->token.tipo == TK_MAIS || p->token.tipo == TK_MENOS) {
        Node *op = nova_binaria(p, esq);
        op->f2 = expr_multiplicativa(p);
        esq = op;
    }
    return esq;
}

/* Nivel 1: *, /, div */
static Node *expr_multiplicativa(Parser *p) {
    Node *esq = expr_basica(p);

    while (p->token.tipo == TK_MULT ||
           p->token.tipo == TK_BARRA ||
           p->token.tipo == TK_DIV) {

        Node *op = nova_binaria(p, esq);
        op->f2 = expr_basica(p);
        esq = op;
    }
    return esq;
}

/* Literais, variaveis, parenteses e operadores unarios.
   UNARIA: texto="-" ou "not", f1=operando.

   O unario pega so a expressao basica seguinte, nao a expressao inteira:
   "not a and b" e (not a) and b, e "-a * b" e (-a) * b. */
static Node *expr_basica(Parser *p) {
    Node *n = NULL;

    switch (p->token.tipo) {
        case TK_IDENTIFICADOR:
            n = ast_novo(NODE_VARIAVEL, p->token.lexema, p->token.linha);
            avancar(p);
            break;

        case TK_INTEIRO_LITERAL:
            n = ast_novo(NODE_INTEIRO, p->token.lexema, p->token.linha);
            avancar(p);
            break;

        case TK_REAL_LITERAL:
            n = ast_novo(NODE_REAL, p->token.lexema, p->token.linha);
            avancar(p);
            break;

        case TK_CHAR_LITERAL:
            n = ast_novo(NODE_CHAR, p->token.lexema, p->token.linha);
            avancar(p);
            break;

        case TK_ABRE_PAR:
            avancar(p);
            n = expr(p);
            consumir(p, TK_FECHA_PAR, "Esperado ')'");
            break;

        case TK_NOT:
        case TK_MENOS:
            n = ast_novo(NODE_UNARIA, p->token.lexema, p->token.linha);
            avancar(p);
            n->f1 = expr_basica(p);
            break;

        default:
            erro(p, "Expressao invalida");
    }
    return n;
}

void parser_iniciar(Parser *p, Lexer *lx) {
    p->lx = lx;
    avancar(p);
}

Node *parser_analisar(Parser *p) {
    Node *raiz = programa(p);
    if (p->token.tipo != TK_EOF) erro(p, "Codigo apos fim do programa");
    return raiz;
}
