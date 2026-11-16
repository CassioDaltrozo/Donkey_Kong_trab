#include "mecanicas.h"
#include "raylib.h"
#include "settings.h"

// subida na escada
#define SUBIR_ESCADA -450.0f

#define DESCER_ESCADA (2 * RENDER_SIZE)

// Chance de 0 a 100 do inimigo usar a escada
#define CHANCE_ESCADA 70
// Tempo antes do inimigo usar outra escada
#define COOLDOWN_ESCADA 1.5f

void update_player(PlayerSettings *player, GameSettings settings, Sound som,  float time){
    if (player->alive)
    {
        player->velocity.x = 0.0f;

        if (!player->is_grounded)
        {
            player->velocity.y += settings.gravity * time;
        }

                if (IsKeyPressed(KEY_J))
                {
                    player->estado_player = ATTACKING;
                }

        if (player->position.x < 0)
            player->position.x = 0;

        if (player->position.x > (COLUNAS * TAM_TILE - RENDER_SIZE))
            player->position.x = (COLUNAS * TAM_TILE - RENDER_SIZE);

                if (IsKeyDown(KEY_SPACE) && player->is_grounded)
                {
                    player->velocity.y = player->jump_force;
                    player->is_grounded = false;
                    player->estado_player = JUMPING;
                    PlaySound(som);
                }

                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                {
                    if (player->is_grounded)
                        player->estado_player = RUNNING;

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                    {
                        player->velocity.x = -player->speed;
                        player->direcao = LEFT;
                    }

                    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                    {
                        player->velocity.x = player->speed;
                        player->direcao = RIGHT;
                    }
                }
                else
                {
                    if (player->is_grounded && player->estado_player == RUNNING)
                    {
                        player->estado_player = IDLE;
                    }
                }

                if ((player->estado_player != ATTACKING && player->is_grounded && player->velocity.x == 0.0f) || (player->estado_player == JUMPING && player->is_grounded && player->velocity.x == 0.0f))
                {
                    player->estado_player = IDLE;
                }
            }
            else
            {
                if (IsKeyPressed(KEY_R))
                    player->alive = true;
            }

    player->position.x += player->velocity.x * time;
    player->position.y += player->velocity.y * time;
}

void update_inimigo(Enemy *inimigo, GameSettings settings, float time, char mapa[LINHAS][COLUNAS]){
    if (inimigo->alive)
            {
                inimigo->velocity.x = inimigo->speed * inimigo->direcao;

        if (!inimigo->is_grounded)
        {
            inimigo->velocity.y += settings.gravity * time;
        }

        inimigo->position.x += inimigo->velocity.x * time;
        inimigo->position.y += inimigo->velocity.y * time;

                if (inimigo->position.x <= 0 || inimigo->position.x >= (COLUNAS * TAM_TILE - RENDER_SIZE))
                {
                    inimigo->direcao *= -1;
                }

                // Escadas do inimigo
                // Diminui o tempo de espera até chegar a zero
                if (inimigo->cooldown > 0.0f)
                    inimigo->cooldown -= time;

                // Descobre em qual bloquinho do mapa está o centro do inimigo
                int col_exam = (int)((inimigo->position.x + RENDER_SIZE / 2.0f) / TAM_TILE);
                int lin_exam = (int)((inimigo->position.y + RENDER_SIZE / 2.0f) / TAM_TILE);

                // Lê o caractere do mapa nesse ponto, com cuidado para não sair da matriz
                char tile_exam = ' ';
                if (col_exam >= 0 && col_exam < COLUNAS && lin_exam >= 0 && lin_exam < LINHAS)
                {
                    tile_exam = mapa[lin_exam][col_exam];
                }

                bool em_escada = (tile_exam == 'S' || tile_exam == 'D');

                // Só sorteia quando está numa escada, nao estava no frame anterior
                // (acabou de chegar), está no chão e a espera acabou
                if (em_escada && !inimigo->na_escada && inimigo->cooldown <= 0.0f)
                {
                    // GetRandomValue(1, 100) devolve um inteiro aleatório de 1 a 100
                    // Se for menor ou igual à chance ele usa a escada
                    if (GetRandomValue(1, 100) <= CHANCE_ESCADA)
                    {
                        if (tile_exam == 'S')
                        {
                            // SUBIR: mesmo impulso do jogador
                            inimigo->velocity.y = SUBIR_ESCADA;
                            inimigo->is_grounded = false;
                        }
                        else
                        {
                            // DESCER: mesmo deslocamento do jogador no tile 'D'
                            inimigo->position.y += DESCER_ESCADA;
                            inimigo->velocity.y = 0.0f;
                            inimigo->is_grounded = false;
                        }

                        inimigo->cooldown = COOLDOWN_ESCADA;
                    }
                }

                inimigo->na_escada = em_escada;
            }
}

void desenha_mapa(char mapa[LINHAS][COLUNAS], Texture2D chao, Texture2D escada, Texture2D ru)
{

    Vector2 origem = {0.0f, 0.0f};
    float rotacao = 0.0f;

    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            Rectangle posicao_telha = {j * TAM_TILE, i * TAM_TILE, RENDER_SIZE, RENDER_SIZE};

                    if (mapa[i][j] == 'Z')
                    { // DESENHA CHAO
                        Rectangle grama_frame = {0.0f, 0.0f, chao.width, chao.height};
                        DrawTexturePro(chao, grama_frame, posicao_telha, origem, rotacao, WHITE);
                    }
                    else if (mapa[i][j] == 'S' || mapa[i][j] == 'H' || mapa[i][j] == 'D')
                    { // DESENHA ESCADA
                        Rectangle ladder_frame = {0.0f, 0.0f, escada.width, escada.height};
                        DrawTexturePro(escada, ladder_frame, posicao_telha, origem, rotacao, WHITE);
                    }
                    else if (mapa[i][j] == 'F')
                    { // DESENHA O RU
                        Rectangle ru_frame = {0.0f, 0.0f, ru.width, ru.height};
                        DrawTexturePro(ru, ru_frame, posicao_telha, origem, rotacao, YELLOW);
                    }
                }
            }
}

void checa_colisoes(char mapa[LINHAS][COLUNAS], PlayerSettings *player, Enemy inimigos[], int qnt_inim){
            // 3. Colisão do Jogador e do inimigo com o mapa ('Z'), com o vento (' ') e com a escada ('S')
            for (int i = 0; i < LINHAS; i++)
            {
                for (int j = 0; j < COLUNAS; j++)
                {
                    Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};

                    if (mapa[i][j] == 'Z')
                    {
                        // COLISAO DO PLAYER COM O CHAO
                        if (CheckCollisionRecs(player->hitbox, tileRect))
                        {
                            if (player->velocity.y >= 0)
                            {
                                player->position.y = (i * TAM_TILE) - RENDER_SIZE;
                                player->velocity.y = 0.0f;
                                player->is_grounded = true;
                            }
                        }

                        // COLISAO DO INIMIGO COM O CHAO
                        for(int e = 0; e < qnt_inim; e++){
                            if (CheckCollisionRecs(inimigos[e].hitbox, tileRect))
                            {
                                if (inimigos[e].velocity.y >= 0)
                                {
                                    inimigos[e].position.y = (i * TAM_TILE) - RENDER_SIZE;
                                    inimigos[e].velocity.y = 0.0f;
                                    inimigos[e].is_grounded = true;
                                }
                            }
                            else
                                inimigos[e].is_grounded = false;
                        }
                    }
                    // bug aleatorio do player->corrigido
                    if (mapa[i][j] == ' ')
                    {
                        if (CheckCollisionRecs(player->hitbox, tileRect) && player->velocity.y == 0)
                        {
                            player->is_grounded = false;
                        }
                    }

                    // subindo escada com player->no s
                    if (mapa[i][j] == 'S')
                    {
                        if (CheckCollisionRecs(player->hitbox, tileRect))
                        {
                            if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player->velocity.x == 0)
                            {
                                player->velocity.y = SUBIR_ESCADA;
                                player->is_grounded = false;
                                player->estado_player = JUMPING;
                            }
                        }
                    }

                    // descendo escada com player->no d
                    if (mapa[i][j] == 'D')
                    {
                        if (CheckCollisionRecs(player->hitbox, tileRect))
                        {
                            if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && player->velocity.x == 0)
                            {
                                player->position.y += DESCER_ESCADA;
                                player->estado_player = JUMPING;
                            }
                        }
                    }
                }
            }
}