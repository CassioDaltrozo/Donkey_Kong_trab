@echo off
:: Volta para a raiz do projeto antes de compilar
cd ..

echo Compilando para Windows...
gcc src/*.c -Wall -std=c99 -Iinclude -Llib/win64 -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o DonkeyKong.exe

echo Processo concluido!
pause
