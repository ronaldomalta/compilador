#include <stdio.h>
#include <stdlib.h>
#include "analise_semantica.h"

TipoDado verificar_expressao_aritmetica(TipoDado esq, TipoDado dir, char operador) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) {
        return TIPO_ERRO;
    }

    if (esq == TIPO_INT && dir == TIPO_INT) {
        return TIPO_INT;
    }

    if ((esq == TIPO_INT || esq == TIPO_FLOAT) && (dir == TIPO_INT || dir == TIPO_FLOAT)) {
        return TIPO_FLOAT;
    }

    printf("Erro Semântico: Operação aritmética '%c' inválida entre tipos incompatíveis.\n", operador);
    return TIPO_ERRO;
}

TipoDado verificar_expressao_relacional(TipoDado esq, TipoDado dir, char *operador) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) {
        return TIPO_ERRO;
    }

    if ((esq == TIPO_INT || esq == TIPO_FLOAT) && (dir == TIPO_INT || dir == TIPO_FLOAT)) {
        return TIPO_BOOL;
    }

    if (esq == TIPO_BOOL && dir == TIPO_BOOL && (operador[0] == '=' || operador[0] == '!')) {
        return TIPO_BOOL;
    }

    printf("Erro Semântico: Operação relacional '%s' não suportada para os tipos informados.\n", operador);
    return TIPO_ERRO;
}

TipoDado verificar_expressao_logica(TipoDado esq, TipoDado dir, char *operador) {
    if (esq == TIPO_ERRO || dir == TIPO_ERRO) {
        return TIPO_ERRO;
    }

    if (esq == TIPO_BOOL && dir == TIPO_BOOL) {
        return TIPO_BOOL;
    }

    printf("Erro Semântico: Operação lógica '%s' exige operandos do tipo booleano.\n", operador);
    return TIPO_ERRO;
}

TipoDado verificar_menos_unario(TipoDado tipo_operando) {
    if (tipo_operando == TIPO_ERRO) {
        return TIPO_ERRO;
    }

    if (tipo_operando == TIPO_INT || tipo_operando == TIPO_FLOAT) {
        return tipo_operando;
    }

    printf("Erro Semântico: Menos unário aplicado a tipo inválido (esperado numérico).\n");
    return TIPO_ERRO;
}

void reportar_erro_tipos_incompativeis(TipoDado esq, TipoDado dir, int linha) {
    printf("Erro Semântico na linha %d: Tipos incompatíveis na expressão.\n", linha);
}