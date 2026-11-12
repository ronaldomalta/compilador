#include <stdio.h>
#include <string.h>
#include "tabela_simbolos.h"

void ts_inicializar(TabelaSimbolos *ts) {
    ts->topo = 0;
    ts->escopo_atual = 0;
}

void ts_entrar_escopo(TabelaSimbolos *ts) {
    ts->escopo_atual++;
}

void ts_sair_escopo(TabelaSimbolos *ts) {
    
    while (ts->topo > 0 && ts->simbolos[ts->topo - 1].escopo == ts->escopo_atual) {
        ts->topo--;
    }
    if (ts->escopo_atual > 0) {
        ts->escopo_atual--;
    }
}

int ts_inserir(TabelaSimbolos *ts, const char *nome, TipoDado tipo, int linha) {
    
    for (int i = ts->topo - 1; i >= 0; i--) {
        if (ts->simbolos[i].escopo < ts->escopo_atual) break;
        if (strcmp(ts->simbolos[i].nome, nome) == 0) {
            fprintf(stderr, "Erro semantico [Linha %d]: Variavel '%s' ja declarada neste escopo.\n", linha, nome);
            return 0; 
        }
    }

    if (ts->topo >= MAX_SIMBOLOS) {
        fprintf(stderr, "Erro: Tabela de simbolos cheia.\n");
        return 0;
    }

    strncpy(ts->simbolos[ts->topo].nome, nome, MAX_LEXEMA - 1);
    ts->simbolos[ts->topo].nome[MAX_LEXEMA - 1] = '\0';
    ts->simbolos[ts->topo].tipo = tipo;
    ts->simbolos[ts->topo].escopo = ts->escopo_atual;
    ts->simbolos[ts->topo].linha = linha;
    ts->topo++;

    return 1;
}

Simbolo* ts_buscar(TabelaSimbolos *ts, const char *nome) {
    
    for (int i = ts->topo - 1; i >= 0; i--) {
        if (strcmp(ts->simbolos[i].nome, nome) == 0) {
            return &ts->simbolos[i];
        }
    }
    return NULL; 
}

void ts_imprimir(TabelaSimbolos *ts) {
    for (int i = 0; i < ts->topo; i++) {
        printf("%s : %s (escopo %d, linha %d)\n",
               ts->simbolos[i].nome,
               nome_tipo(ts->simbolos[i].tipo),
               ts->simbolos[i].escopo,
               ts->simbolos[i].linha);
    }
}

TipoDado tipo_do_token(TokenType tk) {
    switch (tk) {
        case TK_INTEGER: return TIPO_INTEGER;
        case TK_REAL:    return TIPO_REAL;
        case TK_CHAR:    return TIPO_CHAR;
        default:         return TIPO_ERRO;
    }
}

const char *nome_tipo(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEGER: return "integer";
        case TIPO_REAL:    return "real";
        case TIPO_CHAR:    return "char";
        case TIPO_BOOLEAN: return "boolean";
        case TIPO_ERRO:    return "erro";
        default:           return "indefinido";
    }
}