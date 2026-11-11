#ifndef MECANICAS_H
#define MECANICAS_H

#include "raylib.h"
#include "settings.h"

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define RENDER_SIZE 32.0f
#define TAM_TILE 32.0f
#define COLUNAS 30

void update_player(PlayerSettings *player, PlayerState *estado_player, GameSettings settings, Direction *direc, Sound som,  float time);
void update_inimigo(Enemy *inimigo, GameSettings settings, Direction *direc, float time);
void desenha_mapa(char mapa[LINHAS][COLUNAS], Texture2D chao, Texture2D escada, Texture2D ru);

#endif