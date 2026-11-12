#include <stdio.h>
#include <stdarg.h>
#include "analise_comandos.h"

static int erros = 0;

static void erro(int linha, const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Erro semantico [Linha %d]: ", linha);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    erros++;
}

int sc_atribuicao(VerificadorEscopo *ve, const char *nome_var,
                  TipoDado tipo_expr, int linha) {

    Simbolo *s = ve_usar(ve, nome_var, linha);
    if (s == NULL) return 0;

    if (tipo_expr == TIPO_ERRO || s->tipo == TIPO_ERRO) return 0;

    if (s->tipo != tipo_expr) {
        erro(linha, "Atribuicao incompativel: '%s' e %s, mas a expressao e %s.",
             nome_var, nome_tipo(s->tipo), nome_tipo(tipo_expr));
        return 0;
    }
    return 1;
}

int sc_condicao(TipoDado tipo_cond, const char *comando, int linha) {
    if (tipo_cond == TIPO_ERRO) return 0;

    if (tipo_cond != TIPO_BOOLEAN) {
        erro(linha, "A condicao do '%s' deve ser boolean, mas e %s.",
             comando, nome_tipo(tipo_cond));
        return 0;
    }
    return 1;
}

int sc_escrita(TipoDado tipo_expr, int linha) {
    (void)linha;
    if (tipo_expr == TIPO_ERRO) return 0;
    return 1;
}

int sc_total_erros(void) { return erros; }
void sc_zerar_erros(void) { erros = 0; }
