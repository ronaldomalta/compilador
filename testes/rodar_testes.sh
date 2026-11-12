#!/usr/bin/env bash
# Compila o compilador e roda os testes. Rode na raiz do projeto:
#   bash testes/rodar_testes.sh
# No Windows, use o Git Bash.

BUILD=$(mktemp -d)
trap 'rm -rf "$BUILD"' EXIT

gcc -Wall -Wextra -std=c11 -o "$BUILD/comp" main.c lexer.c parser.c ast.c || exit 1

passou=0
falhou=0

ok()    { passou=$((passou + 1)); }
falha() { falhou=$((falhou + 1)); echo "FALHOU: $1"; [ -n "$2" ] && echo "$2"; }

# Programas validos: precisam ser aceitos. Se houver testes/esperado/<nome>.txt,
# a arvore impressa tambem precisa ser identica.
for f in testes/validos/*.pas exemplo1.pas exemplo2.pas teste_final.pas teste_parser.pas; do
    saida=$("$BUILD/comp" "$f")
    codigo=$?
    saida=$(printf '%s' "$saida" | tr -d '\r')
    esperado="testes/esperado/$(basename "$f" .pas).txt"

    if [ "$codigo" -ne 0 ]; then
        falha "$f" "$saida"
    elif [ -f "$esperado" ] && [ "$saida" != "$(tr -d '\r' < "$esperado")" ]; then
        falha "$f" "$(diff <(echo "$saida") <(tr -d '\r' < "$esperado"))"
    else
        ok
    fi
done

# Programas invalidos: precisam terminar com erro e com a mensagem esperada.
for f in testes/invalidos/*.pas; do
    saida=$("$BUILD/comp" "$f")
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
