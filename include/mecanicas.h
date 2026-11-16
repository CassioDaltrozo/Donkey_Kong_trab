#ifndef MECANICAS_H
#define MECANICAS_H

#include "raylib.h"
#include "settings.h"

void update_player(PlayerSettings *player, GameSettings settings, Sound som,  float time);
void update_inimigo(Enemy *inimigo, GameSettings settings, float time, char mapa[LINHAS][COLUNAS]);
void checa_colisoes(char mapa[LINHAS][COLUNAS], PlayerSettings *player, Enemy inimigos[], int qnt_inim);

//FUNCAO DO MACACO
int achaChao(char mapa[LINHAS][COLUNAS], int col, int linha_inicial, int max_descida);
void macaco(Monkey *monkey, PlayerSettings *player, Banana bananas[], char mapa[LINHAS][COLUNAS], float time, Sound somMorte);

#endif