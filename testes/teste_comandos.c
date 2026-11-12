#include <stdio.h>
#include "analise_comandos.h"

static int falhas = 0;

static void confere(const char *caso, int obtido, int esperado) {
    if (obtido != esperado) {
        printf("FALHOU: %s (esperado %d, obtido %d)\n", caso, esperado, obtido);
        falhas++;
    } else {
        printf("ok     %s\n", caso);
    }
}

int main(void) {
    TabelaSimbolos ts;
    VerificadorEscopo ve;
    ts_inicializar(&ts);
    ve_inicializar(&ve, &ts);
    ve_entrar_escopo(&ve);

    ve_declarar(&ve, "x", TIPO_INTEGER, 2);
    ve_declarar(&ve, "r", TIPO_REAL, 3);
    ve_declarar(&ve, "c", TIPO_CHAR, 4);

    printf("--- Atribuicoes corretas ---\n");
    confere("x := 1",       sc_atribuicao(&ve, "x", TIPO_INTEGER, 10), 1);
    confere("r := 2.5",     sc_atribuicao(&ve, "r", TIPO_REAL, 11), 1);
    confere("c := 'a'",     sc_atribuicao(&ve, "c", TIPO_CHAR, 12), 1);

    printf("--- Atribuicoes com erro (mensagem esperada) ---\n");
    confere("r := 1 (sem conversao)",   sc_atribuicao(&ve, "r", TIPO_INTEGER, 20), 0);
    confere("x := 2.5",                 sc_atribuicao(&ve, "x", TIPO_REAL, 21), 0);
    confere("c := 1",                   sc_atribuicao(&ve, "c", TIPO_INTEGER, 22), 0);
    confere("x := a > b (boolean)",     sc_atribuicao(&ve, "x", TIPO_BOOLEAN, 23), 0);
    confere("z := 1 (z nao declarada)", sc_atribuicao(&ve, "z", TIPO_INTEGER, 24), 0);

    printf("--- Erro que veio da expressao (sem mensagem nova) ---\n");
    confere("x := <expressao com erro>", sc_atribuicao(&ve, "x", TIPO_ERRO, 30), 0);

    printf("--- Condicoes ---\n");
    confere("if x > 1 then",      sc_condicao(TIPO_BOOLEAN, "if", 40), 1);
    confere("while x < 3 do",     sc_condicao(TIPO_BOOLEAN, "while", 41), 1);
    confere("until x <= 0",       sc_condicao(TIPO_BOOLEAN, "repeat", 42), 1);
    confere("if x then",          sc_condicao(TIPO_INTEGER, "if", 43), 0);
    confere("while r do",         sc_condicao(TIPO_REAL, "while", 44), 0);
    confere("until c",            sc_condicao(TIPO_CHAR, "repeat", 45), 0);
    confere("if <expr com erro>", sc_condicao(TIPO_ERRO, "if", 46), 0);

    printf("--- write ---\n");
    confere("write(x)",               sc_escrita(TIPO_INTEGER, 50), 1);
    confere("write('a')",             sc_escrita(TIPO_CHAR, 51), 1);
    confere("write(<expr com erro>)", sc_escrita(TIPO_ERRO, 52), 0);

    printf("--- Contagem de erros ---\n");
    confere("erros dos comandos = 7",      sc_total_erros(), 7);
    confere("nao declaradas (escopo) = 1", ve.erros_nao_declarada, 1);

    ve_sair_escopo(&ve);

    printf("\n%s (%d falha(s))\n", falhas ? "TESTE FALHOU" : "TODOS OS CASOS PASSARAM", falhas);
    return falhas ? 1 : 0;
}
