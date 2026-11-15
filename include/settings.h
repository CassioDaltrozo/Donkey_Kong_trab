#ifndef CONFIGURACOES_H
#define CONFIGURACOES_H

#include "raylib.h"

#define LINHAS 30
#define COLUNAS 30
#define MAX_BANANAS 5

// BASE DE DADOS DE CONFIGURACOES DO JOGO, DO JOGADOR, DO INIMIGO, E DAS ANIMAÇOES

typedef struct GameSettings
{
    int screen_width;
    int screen_height;
    int target_fps;
    float gravity;
} GameSettings;

typedef enum GameState
{
    MENU,
    GAME,
    PAUSE,
} GameState;

typedef enum Direction
{
    LEFT = -1,
    RIGHT = 1,
} Direction;

typedef enum PlayerState
{
    IDLE,
    RUNNING,
    JUMPING,
    ATTACKING
} PlayerState;
typedef struct PlayerSettings
{
    float speed;
    float jump_force;
    Vector2 velocity;
    Vector2 position;
    Rectangle hitbox;
    bool is_grounded;
    bool alive;
    Direction direcao;
    PlayerState estado_player;

} PlayerSettings;

typedef struct Enemy
{
    Vector2 position;
    Vector2 velocity;
    Rectangle hitbox;
    float cooldown;
    float speed;
    bool is_grounded;
    bool na_escada;
    bool alive;
    Direction direcao;

} Enemy;

typedef struct Banana
{
    Vector2 position;
    Rectangle hitbox;
    float lifetime;
    bool active;
} Banana;

typedef struct Monkey
{
    Vector2 position;
    Rectangle hitbox;
    float throw_cooldown;
    float throw_interval;
} Monkey;

typedef enum AnimationType
{
    REPEATING = 1,
    ONESHOT = 2,
} AnimationType;

typedef struct Animation
{
    int first_frame;
    int last_frame;
    int current_frame;
    int step;
    float speed;
    float duration_left;
    AnimationType type;
} Animation;

// ASSINA FUNCOES DE ANIMACAO DE SPRITE
void animation_update(Animation *self);
void reset_animation(Animation *self);
Rectangle animation_frame(Animation *self, int num_frames_per_row);

// ASSINA FUNCAO QUE CARREGA O MAPA
int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS]);

// ASSINA FUNCAO QUE PROCURA O CHAO EM UMA COLUNA DO MAPA
int achaChao(char mapa[LINHAS][COLUNAS], int col, int linha_inicial, int max_descida);

#endif