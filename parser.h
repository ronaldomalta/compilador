#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "tabela_simbolos.h"
#include "verificador_escopo.h"

/* O parser faz a analise sintatica e, ao mesmo tempo, as verificacoes
   semanticas (sem AST, como pede a Parte 2):
     - declaracoes      -> tabela de simbolos + verificador de escopo
     - expressoes       -> analise_semantica (tipo de cada expressao)
     - comandos         -> analise_comandos  (atribuicao e condicoes)

   Erro lexico ou sintatico: mostra a mensagem e encerra com exit(1).
   Erro semantico: mostra a mensagem e continua, para achar todos os erros. */

typedef struct {
    Lexer *lx;
    Token token;
    TabelaSimbolos ts;
    VerificadorEscopo ve;
} Parser;

void parser_iniciar(Parser *p, Lexer *lx);

/* Analisa o programa inteiro e devolve o total de erros semanticos. */
int parser_analisar(Parser *p);

#endif
