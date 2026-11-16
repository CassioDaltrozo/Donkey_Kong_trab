#ifndef CONFIGURACOES_H
#define CONFIGURACOES_H

#include "raylib.h"
#include "animations.h"

#define LINHAS 30
#define COLUNAS 30
#define MAX_BANANAS 5

#define TAM_TILE 32.0f
#define RENDER_SIZE 32.0f

#define MAX_INIM 10

#define FRAME_WIDTH 256.0f
#define FRAME_HEIGHT 256.0f

// Hitbox da banana
#define BANANA_LARGURA 10.0f
#define BANANA_ALTURA 6.0f

#define MAX_FASES 3

#define RENDER_SIZE 32.0f
#define TAM_TILE 32.0f
#define COLUNAS 30

// subida na escada
#define SUBIR_ESCADA -450.0f

#define DESCER_ESCADA (2 * RENDER_SIZE)

// Chance de 0 a 100 do inimigo usar a escada
#define CHANCE_ESCADA 70
// Tempo antes do inimigo usar outra escada
#define COOLDOWN_ESCADA 1.5f

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

typedef struct PlayerSettings
{
    float speed;
    float jump_force;
    Vector2 velocity;
    Vector2 position;
    Rectangle hitbox;
    bool is_grounded;
    bool alive;
    bool power_up;
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

typedef struct Ru
{
    Rectangle hitbox;
    int linha_ru;
} Ru;


// ASSINA FUNCAO QUE CARREGA O MAPA
int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS]);
void posicoes_mapa(PlayerSettings *player, Enemy exams[], int *inim_qntd, char mapa[LINHAS][COLUNAS], Monkey *monkey, Ru *ru);
void desenha_mapa(char mapa[LINHAS][COLUNAS], Texture2D chao, Texture2D escada, Texture2D ru);


#endif