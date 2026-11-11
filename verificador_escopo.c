#include <stdio.h>
#include <string.h>
#include "verificador_escopo.h"

void ve_inicializar(VerificadorEscopo *ve, TabelaSimbolos *ts) {
    ve->ts = ts;
    ve->erros_redeclaracao = 0;
    ve->erros_nao_declarada = 0;
    ve->erros_outros = 0;
}

void ve_entrar_escopo(VerificadorEscopo *ve) { ts_entrar_escopo(ve->ts); }
void ve_sair_escopo(VerificadorEscopo *ve)   { ts_sair_escopo(ve->ts); }

/* Busca somente no escopo atual (a tabela e uma pilha ordenada por escopo). */
static Simbolo *buscar_local(VerificadorEscopo *ve, const char *nome) {
    TabelaSimbolos *ts = ve->ts;
    for (int i = ts->topo - 1; i >= 0; i--) {
        if (ts->simbolos[i].escopo < ts->escopo_atual) break;
        if (strcmp(ts->simbolos[i].nome, nome) == 0)
            return &ts->simbolos[i];
    }
    return NULL;
}

int ve_declarar(VerificadorEscopo *ve, const char *nome, TipoDado tipo, int linha) {
    Simbolo *existente = buscar_local(ve, nome);
    if (existente != NULL) {
        fprintf(stderr,
                "Erro semantico [Linha %d]: Variavel '%s' ja declarada neste escopo (primeira declaracao na linha %d).\n",
                linha, nome, existente->linha);
        ve->erros_redeclaracao++;
        return 0;
    }

    if (ve->ts->topo >= MAX_SIMBOLOS) {
        fprintf(stderr,
                "Erro [Linha %d]: Tabela de simbolos cheia ao declarar '%s'.\n",
                linha, nome);
        ve->erros_outros++;
        return 0;
    }

    return ts_inserir(ve->ts, nome, tipo, linha);
}

Simbolo *ve_usar(VerificadorEscopo *ve, const char *nome, int linha) {
    Simbolo *s = ts_buscar(ve->ts, nome);
    if (s == NULL) {
        fprintf(stderr,
                "Erro semantico [Linha %d]: Variavel '%s' nao declarada.\n",
                linha, nome);
        ve->erros_nao_declarada++;
    }
    return s;
}

int ve_total_erros(const VerificadorEscopo *ve) {
    return ve->erros_redeclaracao + ve->erros_nao_declarada + ve->erros_outros;
}

void ve_imprimir_resumo(const VerificadorEscopo *ve) {
    printf("\n=== Verificacao de escopo ===\n");
    printf("Redeclaracoes:        %d\n", ve->erros_redeclaracao);
    printf("Nao declaradas:       %d\n", ve->erros_nao_declarada);
    if (ve->erros_outros > 0)
        printf("Outros:               %d\n", ve->erros_outros);
    printf("Total de erros:       %d\n", ve_total_erros(ve));
}
