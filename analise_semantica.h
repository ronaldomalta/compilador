#ifndef ANALISE_SEMANTICA_H
#define ANALISE_SEMANTICA_H

/* Analise semantica das expressoes (integrante 3).
   Usa o TipoDado da tabela de simbolos (integrante 1), para que todos os
   modulos falem a mesma lingua de tipos.

   Cada funcao recebe o(s) tipo(s) do(s) operando(s), o token do operador e a
   linha, e devolve o tipo do resultado. Se a operacao for invalida, mostra o
   erro e devolve TIPO_ERRO. Se um operando ja for TIPO_ERRO, o erro ja foi
   mostrado antes: a funcao so repassa TIPO_ERRO, sem mensagem repetida. */

#include "token.h"
#include "tabela_simbolos.h"

/* Tipo de um literal: inteiro, real ou char. */
TipoDado tipo_literal(TokenType tk);

/* +  -  *  /  div */
TipoDado verificar_expressao_aritmetica(TipoDado esq, TipoDado dir, TokenType op, int linha);

/* =  <>  <  <=  >  >= */
TipoDado verificar_expressao_relacional(TipoDado esq, TipoDado dir, TokenType op, int linha);

/* and  or */
TipoDado verificar_expressao_logica(TipoDado esq, TipoDado dir, TokenType op, int linha);

/* not */
TipoDado verificar_not(TipoDado tipo_operando, int linha);

/* - unario */
TipoDado verificar_menos_unario(TipoDado tipo_operando, int linha);

int  se_total_erros(void);
void se_zerar_erros(void);

#endif
