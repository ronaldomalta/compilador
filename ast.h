#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
    NODE_PROGRAMA, NODE_DECL, NODE_BLOCO,
    NODE_ATRIB, NODE_IF, NODE_WHILE, NODE_REPEAT, NODE_WRITE,
    NODE_BINARIA, NODE_UNARIA,
    NODE_INTEIRO, NODE_REAL, NODE_CHAR, NODE_VARIAVEL
} TipoNode;

typedef enum {
    TIPO_INDEFINIDO, TIPO_INTEGER, TIPO_REAL, TIPO_CHAR, TIPO_BOOLEAN, TIPO_ERRO
} TipoDado;

typedef struct Node {
    TipoNode tipo_node;
    TipoDado tipo;
    char texto[MAX_LEXEMA];
    int linha;
    struct Node *f1, *f2, *f3;
    struct Node *prox;
} Node;

Node  *ast_novo(TipoNode tipo_node, const char *texto, int linha);
Node  *ast_anexar(Node *lista, Node *novo);
void ast_imprimir(Node *node, int nivel);
void ast_liberar(Node *node);

#endif