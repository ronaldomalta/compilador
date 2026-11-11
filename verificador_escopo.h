#ifndef VERIFICADOR_ESCOPO_H
#define VERIFICADOR_ESCOPO_H

#include "tabela_simbolos.h"

typedef struct {
    TabelaSimbolos *ts;
    int erros_redeclaracao;
    int erros_nao_declarada;
    int erros_outros;
} VerificadorEscopo;

void ve_inicializar(VerificadorEscopo *ve, TabelaSimbolos *ts);
void ve_entrar_escopo(VerificadorEscopo *ve);
void ve_sair_escopo(VerificadorEscopo *ve);

int ve_declarar(VerificadorEscopo *ve, const char *nome, TipoDado tipo, int linha);
Simbolo *ve_usar(VerificadorEscopo *ve, const char *nome, int linha);

int  ve_total_erros(const VerificadorEscopo *ve);
void ve_imprimir_resumo(const VerificadorEscopo *ve);

#endif
