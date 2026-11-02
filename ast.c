#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node *ast_novo(TipoNode tipo_node, const char *texto, int linha) {
    Node *node = calloc(1, sizeof(Node));
    if (!node) { fprintf(stderr, "Sem memoria\n"); exit(1); }
    node->tipo_node = tipo_node;
    node->tipo = TIPO_INDEFINIDO;
    node->linha = linha;
    if (texto) {
        strncpy(node->texto, texto, MAX_LEXEMA - 1);
    }
    return node;
}

Node *ast_anexar(Node *lista, Node *novo) {
    if (!lista) return novo;
    Node *p = lista;
    while (p->prox) p = p->prox;
    p->prox = novo;
    return lista;
}

static const char *NOMES_NODE[] = {
    "Programa", "Var", "Bloco",
    "Atribuicao", "If", "While", "Repeat", "Write",
    "Op", "Unario",
    "Inteiro", "Real", "Char", "Variavel"
};

static const char *NOMES_TIPO[] = {
    "", "integer", "real", "char", "boolean", "erro"
};

void ast_imprimir(Node *node, int nivel) {
    for (; node; node = node->prox) {
        for (int i = 0; i < nivel; i++) printf("  ");
        printf("%s %s", NOMES_NODE[node->tipo_node], node->texto);
        if (node->tipo != TIPO_INDEFINIDO) printf(" : %s", NOMES_TIPO[node->tipo]);
        printf("\n");
        ast_imprimir(node->f1, nivel + 1);
        ast_imprimir(node->f2, nivel + 1);
        ast_imprimir(node->f3, nivel + 1);
    }
}

void ast_liberar(Node *node) {
    while (node) {
        Node *prox = node->prox;
        ast_liberar(node->f1);
        ast_liberar(node->f2);
        ast_liberar(node->f3);
        free(node);
        node = prox;
    }
}