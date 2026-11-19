#ifndef MECANICAS_H
#define MECANICAS_H

#include "raylib.h"
#include "settings.h"

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define RENDER_SIZE 32.0f
#define TAM_TILE 32.0f
#define COLUNAS 30

void update_player(PlayerSettings *player, GameSettings settings, Sound som,  float time);
void update_inimigo(Enemy *inimigo, GameSettings settings, float time, char mapa[LINHAS][COLUNAS]);
void checa_colisoes(char mapa[LINHAS][COLUNAS], PlayerSettings *player, Enemy inimigos[], int qnt_inim);
void banana_macaco(Monkey *monkey, float time, PlayerSettings *player, Banana bananas[], char mapa[LINHAS][COLUNAS], Sound somMorte);
int achaChao(char mapa[LINHAS][COLUNAS], int col, int linha_inicial, int max_descida);


#endif