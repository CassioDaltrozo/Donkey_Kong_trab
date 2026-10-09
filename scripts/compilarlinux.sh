#!/bin/bash

# Garante que o script vá para a raiz do projeto independente de onde for chamado
cd "$(dirname "$0")/.."

echo "Compilando para Linux..."
gcc src/*.c -Wall -std=c99 -Iinclude -Llib/linux64 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o DonkeyKong
chmod +x DonkeyKong

echo "Processo concluido!"
