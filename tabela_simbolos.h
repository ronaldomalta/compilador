#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include "token.h"

#define MAX_SIMBOLOS 100

typedef enum {
    TIPO_INDEFINIDO,
    TIPO_INTEGER,
    TIPO_REAL,
    TIPO_CHAR,
    TIPO_BOOLEAN,
    TIPO_ERRO
} TipoDado;

typedef struct {
    char nome[MAX_LEXEMA];
    TipoDado tipo;
    int escopo;
    int linha;
} Simbolo;

typedef struct {
    Simbolo simbolos[MAX_SIMBOLOS];
    int topo;
    int escopo_atual;
} TabelaSimbolos;

void ts_inicializar(TabelaSimbolos *ts);
void ts_entrar_escopo(TabelaSimbolos *ts);
void ts_sair_escopo(TabelaSimbolos *ts);

int ts_inserir(
    TabelaSimbolos *ts,
    const char *nome,
    TipoDado tipo,
    int linha
);

Simbolo *ts_buscar(TabelaSimbolos *ts, const char *nome);
void ts_imprimir(TabelaSimbolos *ts);

TipoDado tipo_do_token(TokenType tk);
const char *nome_tipo(TipoDado tipo);

#endif