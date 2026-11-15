#ifndef MECANICAS_H
#define MECANICAS_H

#include "raylib.h"
#include "settings.h"

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define RENDER_SIZE 32.0f
#define TAM_TILE 32.0f
#define COLUNAS 30

void update_player(PlayerSettings *player, PlayerState *estado_player, GameSettings settings, Sound som,  float time);
void update_inimigo(Enemy *inimigo, GameSettings settings, float time, char mapa[LINHAS][COLUNAS]);
void desenha_mapa(char mapa[LINHAS][COLUNAS], Texture2D chao, Texture2D escada, Texture2D ru);
void checa_colisoes(char mapa[LINHAS][COLUNAS], PlayerSettings *player, PlayerState *estado_player, Enemy inimigos[], int qnt_inim);

#endif