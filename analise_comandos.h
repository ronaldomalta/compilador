#ifndef ANALISE_COMANDOS_H
#define ANALISE_COMANDOS_H

#include "tabela_simbolos.h"
#include "verificador_escopo.h"

int sc_atribuicao(VerificadorEscopo *ve, const char *nome_var,
                  TipoDado tipo_expr, int linha);

int sc_condicao(TipoDado tipo_cond, const char *comando, int linha);

int sc_escrita(TipoDado tipo_expr, int linha);

int  sc_total_erros(void);
void sc_zerar_erros(void);

#endif
