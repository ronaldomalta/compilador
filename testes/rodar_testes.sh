#!/usr/bin/env bash
# Compila o compilador e roda os testes. Rode na raiz do projeto:
#   bash testes/rodar_testes.sh
# No Windows, use o Git Bash.

BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

FONTES="lexer.c parser.c tabela_simbolos.c verificador_escopo.c analise_semantica.c analise_comandos.c"
gcc -Wall -Wextra -std=c11 -I. -o "$BUILD/comp" main.c $FONTES || exit 1
gcc -Wall -Wextra -std=c11 -I. -o "$BUILD/teste_comandos" testes/teste_comandos.c \
    tabela_simbolos.c verificador_escopo.c analise_comandos.c || exit 1

passou=0
falhou=0

ok()    { passou=$((passou + 1)); }
falha() { falhou=$((falhou + 1)); echo "FALHOU: $1"; [ -n "$2" ] && echo "$2"; }

# Teste de unidade da analise dos comandos (integrante 4).
if "$BUILD/teste_comandos" > /dev/null 2>&1; then ok; else falha "testes/teste_comandos.c" "$("$BUILD/teste_comandos" 2>&1 | grep FALHOU)"; fi

# Programas validos: precisam ser aceitos sem nenhum erro.
for f in testes/validos/*.pas exemplo1.pas exemplo2.pas teste_final.pas teste_parser.pas; do
    saida=$("$BUILD/comp" "$f" 2>&1)
    codigo=$?
    saida=$(printf '%s' "$saida" | tr -d '\r')

    if [ "$codigo" -ne 0 ]; then
        falha "$f" "$saida"
    else
        ok
    fi
done

# Programas invalidos (lexicos, sintaticos e semanticos): precisam terminar
# com erro e com exatamente as mensagens do arquivo .esperado.
for f in testes/invalidos/*.pas; do
    saida=$("$BUILD/comp" "$f" 2>&1)
    codigo=$?
    saida=$(printf '%s' "$saida" | tr -d '\r')
    esperado=$(tr -d '\r' < "${f%.pas}.esperado")

    if [ "$codigo" -eq 0 ] || [ "$saida" != "$esperado" ]; then
        falha "$f" "esperado: $esperado | obtido: $saida (saida $codigo)"
    else
        ok
    fi
done

echo "Passaram: $passou | Falharam: $falhou"
[ "$falhou" -eq 0 ]
