#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }

    Lexer lx;

    if (!lexer_iniciar(&lx, argv[1])) {
        fprintf(stderr, "Erro ao abrir o arquivo '%s'.\n", argv[1]);
        return 1;
    }

    Parser parser;
    parser_iniciar(&parser, &lx);

    parser_analisar(&parser);

    printf("Programa sintaticamente correto!\n");

    lexer_liberar(&lx);

    return 0;
}