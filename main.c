#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

void criar_arquivo_teste(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "w");
    if (!f) return;

    
    fputs("program TesteLexer;\n", f);
    fputs("var\n", f);
    fputs("   x, y : integer;\n", f);
    fputs("   val : real;\n", f);
    fputs("   ch : char;\n", f);
    fputs("begin\n", f);
    fputs("   x := 42 + 10 - 5 * 2 / 1;\n", f);
    fputs("   val := 3.1415;\n", f);
    fputs("   ch := 'a';\n", f);
    fputs("   if (x <= 100) and (y >= 50) or (x <> y) then\n", f);
    fputs("      x := x div 2;\n", f);
    fputs("   if not (x = y) then write(x);\n", f);
    fputs("   @ # $\n", f); 
    fputs("end.\n", f);

    fclose(f);
}

int main(void) {
    const char *arquivo = "teste_tokens.txt";
    criar_arquivo_teste(arquivo);

    Lexer lx;
    if (!lexer_iniciar(&lx, arquivo)) {
        fprintf(stderr, "Erro ao abrir o arquivo de teste.\n");
        return 1;
    }

    printf("--- INICIANDO ANÁLISE LÉXICA ---\n\n");

    Token t;
    do {
        t = lexer_proximo_token(&lx);
        printf("[Linha %2d, Col %2d] Token: %-15s Lexema: '%s'\n",
               t.linha, t.coluna, token_nome(t.tipo), t.lexema);
    } while (t.tipo != TK_EOF);

    lexer_liberar(&lx);
    printf("\n--- ANÁLISE LÉXICA CONCLUÍDA ---\n");

    return 0;
}