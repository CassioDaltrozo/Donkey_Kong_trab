#include <stdio.h>
#include "raylib.h"

int main(){
    printf("teste.\n");
    InitWindow(300,300,"TESTE_FINAL_MITO2");
    
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        EndDrawing();
        }
}