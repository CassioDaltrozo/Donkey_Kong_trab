#include <stdio.h>
#include "raylib.h"

int main(){
    printf("teste.\n");
    InitWindow(300,300,"TESTE2222");
    
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        EndDrawing();
        }
}