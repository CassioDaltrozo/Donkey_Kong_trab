#include "raylib.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>

#define COLUNAS 30
#define LINHAS 30

// Tamanho de cada frame dentro do ficheiro da imagem original (ex: 256x256 por frame)
#define FRAME_WIDTH 256.0f
#define FRAME_HEIGHT 256.0f

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define TAM_TILE 32.0f
#define RENDER_SIZE 32.0f

int main(){
    //RENDERIZAÇÃO BÁSICA DO MAPA 
    char mapa[LINHAS][COLUNAS];

    if (carregaMapa("maps/mapa1.txt", mapa) == 0)
    {
        printf("Erro ao carregar o mapa!\n");
        return 0;
    }

    //DEFINIÇÃO DE DADOS DO JOGO, DO JOGADOR E DO INIMIGO 1
    GameSettings settings = {
        .screen_width = (int)(COLUNAS * TAM_TILE),
        .screen_height = (int)(LINHAS * TAM_TILE),
        .target_fps = 60,
        .gravity = 1200.0f,
    };

    GameState estado_jogo = MENU;

    PlayerSettings player = {
        .alive = true,
        .is_grounded = true,
        .velocity = {0.0f, 0.0f},
        .speed = 100.0f,
        .jump_force = -320.0f,
    };

    Enemy exam = {
        .velocity = {0.0f, 0.0f},
        .speed = 200.0f,
        .is_grounded = false,
        .alive = true
    };


    // Posicionamento inicial do player e do inimigo com base na matriz do mapa
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

    //INICIALIZAÇÃO DOS ESTADOS DE ANIMAÇÃO E DAS DIREÇÕES
    PlayerState current_state = IDLE;
    PlayerState previous_state = IDLE;
    Direction player_direction = RIGHT;
    Direction exam_direction = LEFT;
    Animation *current_anim = &anim_idle;

    //INICIALIZAÇÃO DA JANELA, AUDIO, FPS E TEXTURAS
    InitWindow(settings.screen_width, settings.screen_height, "Donkey Kong do RU");
    InitAudioDevice();
    SetTargetFPS(settings.target_fps);

    Texture2D player_text = LoadTexture("assets/jogador/jogador.png");
    Texture2D ladder_text = LoadTexture("assets/mapa/escada2.png");
    Texture2D exam_text = LoadTexture("assets/inimigos/inimigoteste.png");
    Texture2D grama_text = LoadTexture("assets/mapa/grama2.png");
    Texture2D ru_text = LoadTexture("assets/mapa/ru.png");
    Sound jumpSound = LoadSound("assets/sons/jump.mp3");
    Sound somMorte = LoadSound("assets/sons/sommorte.mp3");
    Music musiquinhaTeste = LoadMusicStream("assets/musica/musicafundo.mp3");

    PlayMusicStream(musiquinhaTeste);

    //LOOP PRINCIPAL
    while(!WindowShouldClose()){

        // Hitboxes proporcionais ao tamanho de 32x32
        Rectangle player_hitbox = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};
        Rectangle exam_hitbox = {0};

        switch(estado_jogo){
            case MENU:
                if (IsKeyPressed(KEY_ENTER)) estado_jogo = GAME;
                break;
            case GAME:
                if(IsKeyPressed(KEY_TAB)) estado_jogo = PAUSE; 

                      UpdateMusicStream(musiquinhaTeste);

                float delta_time = GetFrameTime();
                previous_state = current_state;

                // 1. Lógica, Controles e posição do Jogador
                if (player.alive) {
                    player.velocity.x = 0.0f;

                    if (!player.is_grounded) {
                        player.velocity.y += settings.gravity * delta_time;
                    }

                    if (IsKeyPressed(KEY_J)) {
                        current_state = ATTACKING;
                    } 

                    if (IsKeyPressed(KEY_J)) {
                        current_state = ATTACKING;
                    }

                    if(player.position.x < 0)
                        player.position.x = 0;

                    if(player.position.x > (COLUNAS * TAM_TILE - RENDER_SIZE))
                        player.position.x = (COLUNAS * TAM_TILE - RENDER_SIZE);

                    if (IsKeyDown(KEY_SPACE) && player.is_grounded) {
                        player.velocity.y = player.jump_force;
                        player.is_grounded = false;
                        current_state = JUMPING;
                        PlaySound(jumpSound);
                    } 

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
                        if (player.is_grounded) 
                            current_state = RUNNING;
                        
                        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
                            player.velocity.x = -player.speed;
                            player_direction = LEFT;
                        }
                        
                        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
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

                // 2. Lógica e Colisão do Inimigo com o mapa
                if (exam.alive) {
                    exam.velocity.x = exam.speed * exam_direction;

                    if (!exam.is_grounded) {
                        exam.velocity.y += settings.gravity * delta_time;
                    }

                    exam.position.x += exam.velocity.x * delta_time;
                    exam.position.y += exam.velocity.y * delta_time;

                    exam_hitbox = (Rectangle){exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE};

                    if (exam.position.x <= 0 || exam.position.x >= (COLUNAS * TAM_TILE - RENDER_SIZE)) {
                        exam_direction *= -1;
                    }
                }

                // 3. Colisão do Jogador e do inimigo com o mapa ('Z'), com o vento (' ') e com a escada ('S')
                for (int i = 0; i < LINHAS; i++) {
                    for (int j = 0; j < COLUNAS; j++) {
                        Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};

                        if (mapa[i][j] == 'Z') {
                            //COLISAO DO PLAYER COM O CHAO
                            if (CheckCollisionRecs(player_hitbox, tileRect)) {
                                if (player.velocity.y >= 0) {
                                    player.position.y = (i * TAM_TILE) - RENDER_SIZE; 
                                    player.velocity.y = 0.0f;
                                    player.is_grounded = true;
                                }
                            }
                        
                            //COLISAO DO INIMIGO COM O CHAO
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
                        //bug aleatorio do player corrigido
                        if (mapa[i][j] == ' ') {
                            if (CheckCollisionRecs(player_hitbox, tileRect) && player.velocity.y == 0) {
                                player.is_grounded = false;
                            }
                        }

                        //subindo escada com player no s
                        if (mapa[i][j] == 'S'){
                            if (CheckCollisionRecs(player_hitbox, tileRect)) {
                                if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.velocity.x == 0) {
                                    player.velocity.y = -450.0f;
                                    player.is_grounded = false;
                                    current_state = JUMPING;
                                }
                            }
                        }

                        //descendo escada com player no d
                        if (mapa[i][j] == 'D'){
                            if (CheckCollisionRecs(player_hitbox, tileRect)) {
                                if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && player.velocity.x == 0) {
                                    player.position.y += 2 * RENDER_SIZE;
                                    current_state = JUMPING;
                                }
                            }
                        }
                    }
                }

                //TESTA SE O JOGADOR ESTÁ VIVO OU NÃO
                if (player.alive && CheckCollisionRecs(player_hitbox, exam_hitbox)) {
                    if (current_state == ATTACKING) {
                        exam.alive = false;
                        PlaySound(somMorte);
                    } else {
                        player.alive = false;
                        PlaySound(somMorte);
                        }
                    }

                //TROCA ESTADO DE ANIMAÇÃO DO JOGADOR
                switch (current_state) {
                    case IDLE:      
                        current_anim = &anim_idle;
                        break;
                    case RUNNING:   
                        current_anim = &anim_run;
                        break;
                    case JUMPING:   
                        current_anim = &anim_jump;
                        break;
                    case ATTACKING: 
                        current_anim = &anim_attack;
                        break;
                }

                if (player.alive) {
                    if (current_state != previous_state) {
                        reset_animation(current_anim);
                    }
                    animation_update(current_anim);
                }

                break;
                
            case PAUSE:
                if (IsKeyPressed(KEY_ENTER)) estado_jogo = GAME;
                else if (IsKeyPressed(KEY_Q)) estado_jogo = MENU; 

                break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(estado_jogo){
            case MENU:
                DrawText("Aperte ENTER ou ESC", COLUNAS * TAM_TILE / 2, LINHAS * TAM_TILE / 2, 20, RED); 
                break;
            case GAME:
                        // Desenha os elementos do mapa
                Vector2 origem = {0.0f, 0.0f};
                float rotacao = 0.0f;

                for (int i = 0; i < LINHAS; i++) {
                    for (int j = 0; j < COLUNAS; j++) {
                        Rectangle posicao_telha = {j * TAM_TILE, i * TAM_TILE, RENDER_SIZE, RENDER_SIZE};

                        if (mapa[i][j] == 'Z') {  //DESENHA CHAO
                            Rectangle grama_frame = {0.0f, 0.0f, grama_text.width, grama_text.height};
                            DrawTexturePro(grama_text, grama_frame, posicao_telha, origem, rotacao, WHITE);

                        }
                        else if (mapa[i][j] == 'S' || mapa[i][j] == 'H' || mapa[i][j] == 'D') { //DESENHA ESCADA
                            Rectangle ladder_frame = {0.0f, 0.0f, ladder_text.width, ladder_text.height};
                            DrawTexturePro(ladder_text, ladder_frame, posicao_telha, origem, rotacao, WHITE);

                        }
                        else if (mapa[i][j] == 'F') { //DESENHA O RU
                            Rectangle ru_frame = {0.0f, 0.0f, ru_text.width, ru_text.height};
                            DrawTexturePro(ru_text, ru_frame, posicao_telha, origem, rotacao, YELLOW);
                        }
                    }
                }

                // Desenha o Jogador
                if (player.alive) {
                    Rectangle player_frame = animation_frame(current_anim, 4);
                    Rectangle desenho_player = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};

                    //ATULIZA DIRECAO DO PLAYER NA SPRITE (inverte)
                    if (player_direction == LEFT) {
                        player_frame.width = -player_frame.width;
                    }

                    //DESENHA O PLAYER E HITBOX PARA TESTES
                    DrawTexturePro(player_text, player_frame, desenho_player, origem, rotacao, WHITE);
                    DrawRectangleLines((int)player_hitbox.x, (int)player_hitbox.y, (int)player_hitbox.width, (int)player_hitbox.height, GREEN);
                }
                else{
                    DrawText("VOCE MORREU! Pressione R para reiniciar", 150, 450, 20, RED);
                }

                // Desenha o Inimigo
                if (exam.alive) {
                    Rectangle exam_frame = {0.0f, 0.0f, (float)exam_text.width, (float)exam_text.height};
                    Rectangle desenho_exam = {exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE};

                    //ATUALIZA DIRECAO DO INIMIGO NA SPRITE(inverte)
                    if (exam_direction == LEFT) {
                        exam_frame.width = -exam_frame.width;
                    }

                    //DESENHA O INIMIGO E HITBOX PARA TESTES
                    DrawTexturePro(exam_text, exam_frame, desenho_exam, origem, rotacao, WHITE);
                    DrawRectangleLines((int)exam_hitbox.x, (int)exam_hitbox.y, (int)exam_hitbox.width, (int)exam_hitbox.height, RED);
                }
                        

                break;
            case PAUSE:
                DrawText("Aperte ENTER ou Q", COLUNAS * TAM_TILE / 2, LINHAS * TAM_TILE / 2, 20, RED); 
                break;
        }
        EndDrawing();
    }

    //DESCARREGA MUSICA, TEXTURAS, FECHA AUDIO E JANELA
    UnloadMusicStream(musiquinhaTeste);
    UnloadSound(jumpSound);
    UnloadSound(somMorte);
    UnloadTexture(ladder_text);
    UnloadTexture(player_text);
    UnloadTexture(exam_text);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}


