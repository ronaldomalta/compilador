#ifndef ANALISE_SEMANTICA_H
#define ANALISE_SEMANTICA_H

#include "tabela_simbolos.h"
#include "verificador_escopo.h"

typedef enum {
    TIPO_ERRO = -1,
    TIPO_INT = 0,
    TIPO_FLOAT,
    TIPO_BOOL,
    TIPO_VAZIO
} TipoDado;

TipoDado verificar_expressao_aritmetica(TipoDado esq, TipoDado dir, char operador);
TipoDado verificar_expressao_relacional(TipoDado esq, TipoDado dir, char *operador);
TipoDado verificar_expressao_logica(TipoDado esq, TipoDado dir, char *operador);
TipoDado verificar_menos_unario(TipoDado tipo_operando);
void reportar_erro_tipos_incompativeis(TipoDado esq, TipoDado dir, int linha);

#endif