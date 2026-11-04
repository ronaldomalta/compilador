#include <stdio.h>
#include <string.h>
#include "escopo_ast.h"

void analisar_escopos_e_declaracoes(Node *node, TabelaSimbolos *ts) {
    if (!node) return;

    int gerou_escopo = 0;
    

    if (node->tipo_node == NODE_BLOCO || node->tipo_node == NODE_PROGRAMA) {
        ts_entrar_escopo(ts);
        gerou_escopo = 1;
    }

    
    if (node->tipo_node == NODE_DECL) {
        ts_inserir(ts, node->texto, node->tipo, node->linha);
    }

   
    if (node->tipo_node == NODE_VARIAVEL) {
        Simbolo *s = ts_buscar(ts, node->texto);
        if (!s) {
            fprintf(stderr, "Erro semantico [Linha %d]: Variavel '%s' nao declarada.\n", node->linha, node->texto);
            node->tipo = TIPO_ERRO;
        } else {
            node->tipo = s->tipo; 
        }
    }

    
    analisar_escopos_e_declaracoes(node->f1, ts);
    analisar_escopos_e_declaracoes(node->f2, ts);
    analisar_escopos_e_declaracoes(node->f3, ts);

   
    if (gerou_escopo) {
        ts_sair_escopo(ts);
    }

    
    analisar_escopos_e_declaracoes(node->prox, ts);
}