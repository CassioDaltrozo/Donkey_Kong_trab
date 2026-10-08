#!/bin/bash
# Volta para a raiz do projeto antes de compilar
cd ..

echo "Compilando para Linux..."
gcc src/*.c -Wall -std=c99 -Iinclude -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o DonkeyKong
chmod +x DonkeyKong

echo "Processo concluido!"
