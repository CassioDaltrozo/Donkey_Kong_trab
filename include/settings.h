#ifndef CONFIGURACOES_H
#define CONFIGURACOES_H

#include "raylib.h"

#define LINHAS 30
#define COLUNAS 30

typedef struct GameSettings{
    int screen_width;
    int screen_height;
    int target_fps;
    float gravity;
} GameSettings;

typedef struct PlayerSettings{
    float speed;
    float jump_force;
    Vector2 velocity;
    Vector2 position;
    bool is_grounded;
    bool alive;
} PlayerSettings;

typedef enum PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    ATTACKING
} PlayerState;

typedef enum Direction{
    LEFT = -1,
    RIGHT = 1,
} Direction;

typedef enum AnimationType{
    REPEATING = 1,
    ONESHOT = 2, 
} AnimationType;

typedef struct Animation{
    int first_frame;
    int last_frame;
    int current_frame;
    int step;
    float speed;
    float duration_left;
    AnimationType type;
} Animation;

typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool is_grounded;
    bool alive;
} Enemy;

void animation_update(Animation *self);
void reset_animation(Animation *self);
Rectangle animation_frame(Animation *self, int num_frames_per_row);
int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS]);


#endif