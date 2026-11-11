#include "mecanicas.h"
#include "raylib.h"
#include "settings.h"

void update_player(PlayerSettings *player, PlayerState *estado_player, GameSettings settings, Direction *direc, Sound som,  float time){
    if (player->alive)
            {
                player->velocity.x = 0.0f;

                if (!player->is_grounded)
                {
                    player->velocity.y += settings.gravity * time;
                }

                if (IsKeyPressed(KEY_J))
                {
                    *estado_player = ATTACKING;
                }

                if (player->position.x < 0)
                    player->position.x = 0;

                if (player->position.x > (COLUNAS * TAM_TILE - RENDER_SIZE))
                    player->position.x = (COLUNAS * TAM_TILE - RENDER_SIZE);

                if (IsKeyDown(KEY_SPACE) && player->is_grounded)
                {
                    player->velocity.y = player->jump_force;
                    player->is_grounded = false;
                    *estado_player = JUMPING;
                    PlaySound(som);
                }

                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                {
                    if (player->is_grounded)
                        *estado_player = RUNNING;

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                    {
                        player->velocity.x = -player->speed;
                        *direc = LEFT;
                    }

                    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                    {
                        player->velocity.x = player->speed;
                        *direc = RIGHT;
                    }
                }
                else
                {
                    if (player->is_grounded && *estado_player == RUNNING)
                    {
                        *estado_player = IDLE;
                    }
                }

                if ((*estado_player != ATTACKING && player->is_grounded && player->velocity.x == 0.0f) || (*estado_player == JUMPING && player->is_grounded && player->velocity.x == 0.0f))
                {
                    *estado_player = IDLE;
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

void update_inimigo(Enemy *inimigo, GameSettings settings, Direction *direc, float time){
    if (inimigo->alive)
            {
                inimigo->velocity.x = inimigo->speed * (*direc);

                if (!inimigo->is_grounded)
                {
                    inimigo->velocity.y += settings.gravity * time;
                }

                inimigo->position.x += inimigo->velocity.x * time;
                inimigo->position.y += inimigo->velocity.y * time;


                if (inimigo->position.x <= 0 || inimigo->position.x >= (COLUNAS * TAM_TILE - RENDER_SIZE))
                {
                    *direc *= -1;
                }
            }
}

void desenha_mapa(char mapa[LINHAS][COLUNAS], Texture2D chao, Texture2D escada, Texture2D ru){

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