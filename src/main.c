#include "raylib.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>

#define TAM_TILE 35.0f
#define COLUNAS 30
#define LINHAS 30

// Tamanho de cada frame dentro do ficheiro da imagem original (ex: 256x256 por frame)
#define FRAME_WIDTH 256.0f
#define FRAME_HEIGHT 256.0f

// Tamanho visual e de colisão no ecrã (igual ao bloquinho do mapa: 32x32)
#define RENDER_SIZE 32.0f


// Assinaturas de funções
int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS]);

int main(){
    char mapa[LINHAS][COLUNAS];

    if (carregaMapa("maps/mapa1.txt", mapa) == 0)
    {
        printf("Erro ao carregar o mapa!\n");
        return 0;
    }

    GameSettings settings = {
        .screen_width = (int)(COLUNAS * TAM_TILE),
        .screen_height = (int)(LINHAS * TAM_TILE),
        .target_fps = 60,
        .gravity = 1200.0f,
    };

    PlayerSettings player = {
        .alive = true,
        .is_grounded = true,
        .velocity = {0.0f, 0.0f},
        .speed = 200.0f,
        .jump_force = -330.0f,
    };

    Enemy exam = {
        .velocity = {0.0f, 0.0f},
        .speed = 80.0f,
        .is_grounded = false,
        .alive = true
    };

    Direction exam_direction = LEFT;

    // Posicionamento inicial com base na matriz do mapa
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (mapa[i][j] == 'P')
            {
                player.position = (Vector2){j * TAM_TILE, i * TAM_TILE};
                mapa[i][j] = ' '; 
            }
            else if (mapa[i][j] == 'E')
            {
                exam.position = (Vector2){j * TAM_TILE, i * TAM_TILE};
                mapa[i][j] = ' ';
            }
        }
    }

    // Inicialização das Animações
    Animation anim_idle   = { .first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 0.15f, .duration_left = 0.15f, .type = REPEATING };
    Animation anim_run    = { .first_frame = 4, .last_frame = 7, .current_frame = 4, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING };
    Animation anim_jump   = { .first_frame = 8, .last_frame = 11, .current_frame = 11, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT };
    Animation anim_attack = { .first_frame = 12, .last_frame = 15, .current_frame = 12, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT };

    PlayerState current_state = IDLE;
    PlayerState previous_state = IDLE;
    Direction player_direction = RIGHT;
    Animation *current_anim = &anim_idle;

    InitWindow(settings.screen_width, settings.screen_height, "Donkey Kong do RU");
    SetTargetFPS(settings.target_fps);

    Texture2D player_text = LoadTexture("assets/jogador/jogador.png");
    Texture2D exam_text = LoadTexture("assets/inimigos/inimigoteste.png");

    while(!WindowShouldClose()){
        float delta_time = GetFrameTime();
        previous_state = current_state;

        // 1. Lógica e Controles do Jogador
        if (player.alive) {
            player.velocity.x = 0.0f;

            if (!player.is_grounded) {
                player.velocity.y += settings.gravity * delta_time;
            }

            if (IsKeyPressed(KEY_J)) {
                current_state = ATTACKING;
            } 

            if (IsKeyDown(KEY_SPACE) && player.is_grounded) {
                player.velocity.y = player.jump_force;
                player.is_grounded = false;
                current_state = JUMPING;
            } 

            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
                if (player.is_grounded) current_state = RUNNING;
                    if (IsKeyDown(KEY_A)){
                        player.velocity.x = -player.speed;
                        player_direction = LEFT;
                    }
                    if (IsKeyDown(KEY_D)){
                        player.velocity.x = player.speed;
                        player_direction = RIGHT;
                    }
                } else {
                    if (player.is_grounded && current_state == RUNNING) {
                        current_state = IDLE;
                    }
                }

            if ((current_state != ATTACKING || current_anim->current_frame == current_anim->last_frame) && player.is_grounded && player.velocity.x == 0.0f) {
                current_state = IDLE;
                }
            }
            else{
                if(IsKeyPressed(KEY_R))
                    player.alive = true;
            }

        player.position.x += player.velocity.x * delta_time;
        player.position.y += player.velocity.y * delta_time;

        // Hitboxes proporcionais ao tamanho de 32x32
        Rectangle player_hitbox = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};
        Rectangle exam_hitbox = {0};

        // 2. Colisão do Jogador com o mapa ('Z')
        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                if (mapa[i][j] == 'Z') {
                    Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};
                    if (CheckCollisionRecs(player_hitbox, tileRect)) {
                        if (player.velocity.y >= 0) {
                            player.position.y = (i * TAM_TILE) - RENDER_SIZE; 
                            player.velocity.y = 0.0f;
                            player.is_grounded = true;
                        }
                    }
                }
                if (mapa[i][j] == ' ') {
                    Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};
                    if (CheckCollisionRecs(player_hitbox, tileRect)) {
                        if (player.velocity.y == 0) {
                            player.is_grounded = false;
                        }
                    }
                }
            }
        }

        // 3. Lógica e Colisão do Inimigo com o mapa
        if (exam.alive) {
            exam.velocity.x = exam.speed * exam_direction;

            if (!exam.is_grounded) {
                exam.velocity.y += settings.gravity * delta_time;
            }

            exam.position.x += exam.velocity.x * delta_time;
            exam.position.y += exam.velocity.y * delta_time;

            exam_hitbox = (Rectangle){exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE};

            for (int i = 0; i < LINHAS; i++) {
                for (int j = 0; j < COLUNAS; j++) {
                    if (mapa[i][j] == 'Z') {
                        Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};
                        if (CheckCollisionRecs(exam_hitbox, tileRect)) {
                            if (exam.velocity.y >= 0) {
                                exam.position.y = (i * TAM_TILE) - RENDER_SIZE;
                                exam.velocity.y = 0.0f;
                                exam.is_grounded = true;
                            }
                        }
                        else
                            exam.is_grounded = false;
                    }
                }
            }

            if (exam.position.x <= 0 || exam.position.x >= (COLUNAS * TAM_TILE - RENDER_SIZE)) {
                exam_direction *= -1;
            }

            if (player.alive && CheckCollisionRecs(player_hitbox, exam_hitbox)) {
                if (current_state == ATTACKING) {
                    exam.alive = false;
                } else {
                    player.alive = false;
                }
            }
        }

        switch (current_state) {
            case IDLE:      
                current_anim = &anim_idle; break;
            case RUNNING:   
                current_anim = &anim_run; break;
            case JUMPING:   
                current_anim = &anim_jump; break;
            case ATTACKING: 
                current_anim = &anim_attack; break;
        }

        if (player.alive) {
            if (current_state != previous_state) {
                reset_animation(current_anim);
            }
            animation_update(current_anim);
        }

        // --- RENDERIZAÇÃO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha os elementos do mapa
        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                if (mapa[i][j] == 'Z') {
                    DrawRectangle((int)(j * TAM_TILE), (int)(i * TAM_TILE), (int)TAM_TILE, (int)TAM_TILE, GRAY);
                } else if (mapa[i][j] == 'S' || mapa[i][j] == 'H' || mapa[i][j] == 'D') {
                    DrawRectangle((int)(j * TAM_TILE), (int)(i * TAM_TILE), (int)TAM_TILE, (int)TAM_TILE, GREEN);
                } else if (mapa[i][j] == 'F') {
                    DrawRectangle((int)(j * TAM_TILE), (int)(i * TAM_TILE), (int)TAM_TILE, (int)TAM_TILE, YELLOW);
                }
            }
        }

        // Desenha o Jogador
        if (player.alive) {
            Rectangle player_frame = animation_frame(current_anim, 4);
            if (player_direction == LEFT) {
                player_frame.width = -player_frame.width;
            }
            DrawTexturePro(
                player_text, 
                player_frame,   
                (Rectangle){player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE},
                (Vector2){0.0f, 0.0f},
                0.0f, 
                WHITE
            );
            DrawRectangleLines((int)player_hitbox.x, (int)player_hitbox.y, (int)player_hitbox.width, (int)player_hitbox.height, GREEN);
        } else {
            DrawText("VOCE MORREU! Pressione R para reiniciar", 150, 450, 20, RED);
        }

        // Desenha o Inimigo
        if (exam.alive) {
            Rectangle exam_frame = {0.0f, 0.0f, (float)exam_text.width, (float)exam_text.height};
            if (exam_direction == LEFT) {
                exam_frame.width = -exam_frame.width;
            }
            DrawTexturePro(
                exam_text,
                exam_frame,
                (Rectangle){exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE},
                (Vector2){0.0f, 0.0f},
                0.0f,
                WHITE
            );
            DrawRectangleLines((int)exam_hitbox.x, (int)exam_hitbox.y, (int)exam_hitbox.width, (int)exam_hitbox.height, RED);
        }

        EndDrawing();
    }

    UnloadTexture(player_text);
    UnloadTexture(exam_text);
    CloseWindow();
    return 0;
}


int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS])
{
    FILE *arq;
    char linha[COLUNAS + 2];
    int i, j;

    arq = fopen(nomeArquivo, "r");
    if (arq == NULL)
    {
        return 0;
    }

    for (i = 0; i < LINHAS; i++)
    {
        for (j = 0; j < COLUNAS; j++)
        {
            mapa[i][j] = ' ';
        }

        if (fgets(linha, COLUNAS + 2, arq) == NULL)
        {
            linha[0] = '\0';
        }

        for (j = 0; j < COLUNAS; j++)
        {
            if (linha[j] == '\0' || linha[j] == '\n')
            {
                break;
            }
            else
            {
                mapa[i][j] = linha[j];
            }
        }
    }

    fclose(arq);
    return 1;
}

