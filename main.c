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

    /* Erros lexicos e sintaticos encerram o programa dentro do parser.
       Se chegou aqui, a sintaxe esta correta; falta ver a semantica. */
    int erros = parser_analisar(&parser);

    lexer_liberar(&lx);

    fflush(stderr);
    if (erros > 0) {
        printf("Programa com %d erro(s) semantico(s).\n", erros);
        return 1;
    }

    printf("Programa correto: sem erros lexicos, sintaticos ou semanticos.\n");
    return 0;
}
