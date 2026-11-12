#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "analise_semantica.h"
#include "analise_comandos.h"

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
static void decl_var(Parser *p);
static TipoDado tipo(Parser *p);
static void cmd_comp(Parser *p);
static void comandos(Parser *p);
static void comando(Parser *p);
static void atribuicao(Parser *p);
static void decisao(Parser *p);
static void iteracao(Parser *p);
static void escrita(Parser *p);
static TipoDado expr(Parser *p);
static TipoDado expr_logica(Parser *p);
static TipoDado expr_relacional(Parser *p);
static TipoDado expr_aditiva(Parser *p);
static TipoDado expr_multiplicativa(Parser *p);
static TipoDado expr_basica(Parser *p);

static void programa(Parser *p) {
    consumir(p, TK_PROGRAM, "Esperado 'program'");
    consumir(p, TK_IDENTIFICADOR, "Esperado nome do programa");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    ve_entrar_escopo(&p->ve);
    decl_var(p);
    cmd_comp(p);
    consumir(p, TK_PONTO, "Esperado '.' no fim");
}

/* Guarda os nomes de "a, b, c" ate ler o tipo, e so entao declara cada um.
   A redeclaracao e detectada pelo verificador de escopo (integrante 2). */
#define MAX_GRUPO 64

static void decl_var(Parser *p) {
    consumir(p, TK_VAR, "Esperado 'var'");

    while (p->token.tipo == TK_IDENTIFICADOR) {
        char nomes[MAX_GRUPO][MAX_LEXEMA];
        int linhas[MAX_GRUPO];
        int n = 0;

        for (;;) {
            if (p->token.tipo == TK_IDENTIFICADOR && n < MAX_GRUPO) {
                strcpy(nomes[n], p->token.lexema);
                linhas[n] = p->token.linha;
                n++;
            }
            consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
            if (p->token.tipo != TK_VIRGULA) break;
            avancar(p);
        }

        consumir(p, TK_DOIS_PONTOS, "Esperado ':'");
        TipoDado t = tipo(p);
        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

        for (int i = 0; i < n; i++)
            ve_declarar(&p->ve, nomes[i], t, linhas[i]);
    }
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

/* variavel := expressao ;
   Verifica: variavel declarada e tipo compativel (integrante 4). */
static void atribuicao(Parser *p) {
    char nome[MAX_LEXEMA];
    int linha = p->token.linha;
    strcpy(nome, p->token.lexema);

    consumir(p, TK_IDENTIFICADOR, "Esperado identificador");
    consumir(p, TK_ATRIBUICAO, "Esperado ':='");
    TipoDado t = expr(p);
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    sc_atribuicao(&p->ve, nome, t, linha);
}

/* if condicao then comando [else comando]
   A condicao precisa ser boolean (integrante 4). */
static void decisao(Parser *p) {
    int linha = p->token.linha;

    consumir(p, TK_IF, "Esperado 'if'");
    TipoDado cond = expr(p);
    sc_condicao(cond, "if", linha);
    consumir(p, TK_THEN, "Esperado 'then'");
    comando(p);

    if (p->token.tipo == TK_ELSE) {
        avancar(p);
        comando(p);
    }
}

/* while condicao do comando
   repeat comando until condicao ;
   A condicao precisa ser boolean (integrante 4). */
static void iteracao(Parser *p) {
    if (p->token.tipo == TK_WHILE) {
        int linha = p->token.linha;
        avancar(p);
        TipoDado cond = expr(p);
        sc_condicao(cond, "while", linha);
        consumir(p, TK_DO, "Esperado 'do'");
        comando(p);

    } else if (p->token.tipo == TK_REPEAT) {
        avancar(p);
        comando(p);
        int linha = p->token.linha;
        consumir(p, TK_UNTIL, "Esperado 'until'");
        TipoDado cond = expr(p);
        sc_condicao(cond, "repeat", linha);
        consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    } else {
        erro(p, "Esperado 'while' ou 'repeat'");
    }
}

static void escrita(Parser *p) {
    int linha = p->token.linha;

    consumir(p, TK_WRITE, "Esperado 'write'");
    consumir(p, TK_ABRE_PAR, "Esperado '('");
    TipoDado t = expr(p);
    consumir(p, TK_FECHA_PAR, "Esperado ')'");
    consumir(p, TK_PONTO_VIRGULA, "Esperado ';'");

    sc_escrita(t, linha);
}

/* Expressoes: cada nivel devolve o TIPO do resultado, calculado pela
   analise semantica das expressoes (integrante 3).
   Cada nivel chama o de precedencia maior; o laco deixa a associatividade
   a esquerda, como pede o enunciado. */

static TipoDado expr(Parser *p) {
    return expr_logica(p);
}

/* Nivel 4: or e and tem a MESMA precedencia. */
static TipoDado expr_logica(Parser *p) {
    TipoDado esq = expr_relacional(p);

    while (p->token.tipo == TK_OR || p->token.tipo == TK_AND) {
        TokenType op = p->token.tipo;
        int linha = p->token.linha;
        avancar(p);
        TipoDado dir = expr_relacional(p);
        esq = verificar_expressao_logica(esq, dir, op, linha);
    }
    return esq;
}

/* Nivel 3: =, <>, <, <=, >, >= */
static TipoDado expr_relacional(Parser *p) {
    TipoDado esq = expr_aditiva(p);

    while (p->token.tipo == TK_IGUAL ||
           p->token.tipo == TK_DIFERENTE ||
           p->token.tipo == TK_MENOR ||
           p->token.tipo == TK_MENOR_IGUAL ||
           p->token.tipo == TK_MAIOR ||
           p->token.tipo == TK_MAIOR_IGUAL) {

        TokenType op = p->token.tipo;
        int linha = p->token.linha;
        avancar(p);
        TipoDado dir = expr_aditiva(p);
        esq = verificar_expressao_relacional(esq, dir, op, linha);
    }
    return esq;
}

/* Nivel 2: +, - */
static TipoDado expr_aditiva(Parser *p) {
    TipoDado esq = expr_multiplicativa(p);

    while (p->token.tipo == TK_MAIS || p->token.tipo == TK_MENOS) {
        TokenType op = p->token.tipo;
        int linha = p->token.linha;
        avancar(p);
        TipoDado dir = expr_multiplicativa(p);
        esq = verificar_expressao_aritmetica(esq, dir, op, linha);
    }
    return esq;
}

/* Nivel 1: *, /, div */
static TipoDado expr_multiplicativa(Parser *p) {
    TipoDado esq = expr_basica(p);

    while (p->token.tipo == TK_MULT ||
           p->token.tipo == TK_BARRA ||
           p->token.tipo == TK_DIV) {

        TokenType op = p->token.tipo;
        int linha = p->token.linha;
        avancar(p);
        TipoDado dir = expr_basica(p);
        esq = verificar_expressao_aritmetica(esq, dir, op, linha);
    }
    return esq;
}

/* Literais, variaveis, parenteses e operadores unarios.
   O unario pega so a expressao basica seguinte:
   "not a and b" e (not a) and b, e "-a * b" e (-a) * b. */
static TipoDado expr_basica(Parser *p) {
    TipoDado t = TIPO_ERRO;
    int linha = p->token.linha;

    switch (p->token.tipo) {
        case TK_IDENTIFICADOR: {
            Simbolo *s = ve_usar(&p->ve, p->token.lexema, linha);
            t = s ? s->tipo : TIPO_ERRO;
            avancar(p);
            break;
        }

        case TK_INTEIRO_LITERAL:
        case TK_REAL_LITERAL:
        case TK_CHAR_LITERAL:
            t = tipo_literal(p->token.tipo);
            avancar(p);
            break;

        case TK_ABRE_PAR:
            avancar(p);
            t = expr(p);
            consumir(p, TK_FECHA_PAR, "Esperado ')'");
            break;

        case TK_NOT:
            avancar(p);
            t = verificar_not(expr_basica(p), linha);
            break;

        case TK_MENOS:
            avancar(p);
            t = verificar_menos_unario(expr_basica(p), linha);
            break;

        default:
            erro(p, "Expressao invalida");
    }
    return t;
}

void parser_iniciar(Parser *p, Lexer *lx) {
    p->lx = lx;
    ts_inicializar(&p->ts);
    ve_inicializar(&p->ve, &p->ts);
    se_zerar_erros();
    sc_zerar_erros();
    avancar(p);
}

int parser_analisar(Parser *p) {
    programa(p);
    if (p->token.tipo != TK_EOF) erro(p, "Codigo apos fim do programa");
    return ve_total_erros(&p->ve) + se_total_erros() + sc_total_erros();
}
