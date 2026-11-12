#include <stdio.h>
#include <stdarg.h>
#include "analise_semantica.h"

/* Regras de tipos das expressoes:

   Aritmeticas
     + - *    integer com integer -> integer
              integer/real com real -> real
     /        dois numericos -> real (em Pascal a barra sempre da real)
     div      so integer com integer -> integer
     char ou boolean em operacao aritmetica -> erro

   Relacionais (= <> < <= > >=) -> boolean
     numerico com numerico (integer e real podem se misturar)
     char com char
     boolean com boolean, so com = e <>

   Logicas
     and, or  boolean com boolean -> boolean
     not      boolean -> boolean

   Menos unario
     integer -> integer, real -> real; qualquer outro tipo -> erro */

static int erros = 0;

static void erro(int linha, const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Erro semantico [Linha %d]: ", linha);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    erros++;
}

static int numerico(TipoDado t) {
    return t == TIPO_INTEGER || t == TIPO_REAL;
}

static const char *op_texto(TokenType op) {
    switch (op) {
        case TK_MAIS:        return "+";
        case TK_MENOS:       return "-";
        case TK_MULT:        return "*";
        case TK_BARRA:       return "/";
        case TK_DIV:         return "div";
        case TK_IGUAL:       return "=";
        case TK_DIFERENTE:   return "<>";
        case TK_MENOR:       return "<";
        case TK_MENOR_IGUAL: return "<=";
        case TK_MAIOR:       return ">";
        case TK_MAIOR_IGUAL: return ">=";
        case TK_AND:         return "and";
        case TK_OR:          return "or";
        case TK_NOT:         return "not";
        default:             return "?";
    }
}

static TipoDado incompativel(TipoDado esq, TipoDado dir, TokenType op, int linha) {
    erro(linha, "Operacao '%s' invalida entre %s e %s.",
         op_texto(op), nome_tipo(esq), nome_tipo(dir));
    return TIPO_ERRO;
}

TipoDado tipo_literal(TokenType tk) {
    switch (tk) {
        case TK_INTEIRO_LITERAL: return TIPO_INTEGER;
        case TK_REAL_LITERAL:    return TIPO_REAL;
        case TK_CHAR_LITERAL:    return TIPO_CHAR;
        default:                 return TIPO_ERRO;
    }
}

TipoDado verificar_expressao_aritmetica(TipoDado esq, TipoDado dir, TokenType op, int linha) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) return TIPO_ERRO;

    if (!numerico(esq) || !numerico(dir))
        return incompativel(esq, dir, op, linha);

    if (op == TK_DIV) {
        if (esq == TIPO_INTEGER && dir == TIPO_INTEGER) return TIPO_INTEGER;
        return incompativel(esq, dir, op, linha);
    }

    if (op == TK_BARRA) return TIPO_REAL;

    if (esq == TIPO_INTEGER && dir == TIPO_INTEGER) return TIPO_INTEGER;
    return TIPO_REAL;
}

TipoDado verificar_expressao_relacional(TipoDado esq, TipoDado dir, TokenType op, int linha) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) return TIPO_ERRO;

    if (numerico(esq) && numerico(dir)) return TIPO_BOOLEAN;
    if (esq == TIPO_CHAR && dir == TIPO_CHAR) return TIPO_BOOLEAN;
    if (esq == TIPO_BOOLEAN && dir == TIPO_BOOLEAN &&
        (op == TK_IGUAL || op == TK_DIFERENTE))
        return TIPO_BOOLEAN;

    return incompativel(esq, dir, op, linha);
}

TipoDado verificar_expressao_logica(TipoDado esq, TipoDado dir, TokenType op, int linha) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) return TIPO_ERRO;

    if (esq == TIPO_BOOLEAN && dir == TIPO_BOOLEAN) return TIPO_BOOLEAN;

    erro(linha, "Operacao '%s' exige dois boolean, mas recebeu %s e %s.",
         op_texto(op), nome_tipo(esq), nome_tipo(dir));
    return TIPO_ERRO;
}

TipoDado verificar_not(TipoDado tipo_operando, int linha) {
    if (tipo_operando == TIPO_ERRO) return TIPO_ERRO;
    if (tipo_operando == TIPO_BOOLEAN) return TIPO_BOOLEAN;

    erro(linha, "Operador 'not' exige boolean, mas recebeu %s.", nome_tipo(tipo_operando));
    return TIPO_ERRO;
}

TipoDado verificar_menos_unario(TipoDado tipo_operando, int linha) {
    if (tipo_operando == TIPO_ERRO) return TIPO_ERRO;
    if (numerico(tipo_operando)) return tipo_operando;

    erro(linha, "Menos unario exige integer ou real, mas recebeu %s.", nome_tipo(tipo_operando));
    return TIPO_ERRO;
}

int  se_total_erros(void) { return erros; }
void se_zerar_erros(void) { erros = 0; }
