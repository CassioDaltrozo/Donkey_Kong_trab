#include "mecanicas.h"
#include "raylib.h"
#include "settings.h"
#include "raymath.h"
#include <math.h>

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

int achaChao(char mapa[LINHAS][COLUNAS], int col, int linha_inicial, int max_descida)
{
    if (col < 0 || col >= COLUNAS)
    {
        return -1;
    }

    for (int i = linha_inicial; i <= linha_inicial + max_descida && i < LINHAS; i++)
    {
        if (i >= 0 && mapa[i][col] == 'Z')
        {
            return i;
        }
    }

    return -1;
}

void macaco(Monkey *monkey, PlayerSettings *player, Banana bananas[], char mapa[LINHAS][COLUNAS], float time, Sound somMorte){
    // Só conta o tempo com o jogador vivo
                monkey->throw_cooldown -= time;

                if (monkey->throw_cooldown <= 0.0f)
                {
                    monkey->throw_cooldown = monkey->throw_interval;

                    // Preve onde o jogador vai estar
                    // Andando: velocidade x tempo = distância que ele percorre em 1,2 s
                    // Parado: o macaco aposta que ele vai andar para o lado que olha
                    float frente;
                    if (player->velocity.x != 0.0f)
                    {
                        frente = player->velocity.x * 1.2f;
                    }
                    else
                    {
                        frente = player->direcao * 3.0f * TAM_TILE;
                    }
                    float alvo_x = player->position.x + frente;

                    // Não deixa o alvo sair da tela.
                    if (alvo_x < 0.0f)
                        alvo_x = 0.0f;
                    if (alvo_x > COLUNAS * TAM_TILE - RENDER_SIZE)
                        alvo_x = COLUNAS * TAM_TILE - RENDER_SIZE;

                    // Converter posição em coluna e linha do mapa
                    // Dividir por TAM_TILE diz em qual bloquinho estamos
                    // Usamos o centro do jogador (+ RENDER_SIZE / 2) para evitar erro de borda
                    int coluna = (int)((alvo_x + RENDER_SIZE / 2.0f) / TAM_TILE);
                    int linha_jogador = (int)((player->position.y + RENDER_SIZE / 2.0f) / TAM_TILE);

                    // PASSO 3: achar o chão nessa coluna, descendo no máximo 2 linhas.
                    int linha_chao = achaChao(mapa, coluna, linha_jogador, 2);

                    // Posição X da banana: centralizada dentro do bloquinho
                    float banana_x = coluna * TAM_TILE + (TAM_TILE - BANANA_LARGURA) / 2.0f;

                    // Regras que impedem o arremesso
                    bool pode_jogar = true;

                    // 1. não achou chão
                    if (linha_chao == -1)
                        pode_jogar = false;

                    // 3. a banana cairia em cima do jogador
                    if (pode_jogar && fabsf(banana_x - player->position.x) < TAM_TILE)
                        pode_jogar = false;

                    // Se tudo certo, usa uma banana livre do estoque
                    if (pode_jogar)
                    {
                        for (int i = 0; i < MAX_BANANAS; i++)
                        {
                            if (!bananas[i].active)
                            {
                                bananas[i].active = true;
                                bananas[i].lifetime = 8.0f;
                                // O Y é o topo do chão menos a altura da banana, para ela
                                // ficar "sentada" em cima da plataforma.
                                bananas[i].position = (Vector2){banana_x, linha_chao * TAM_TILE - BANANA_ALTURA};
                                break;
                            }
                        }
                    }
                }

            // Atualiza cada banana do estoque
            // continue pula para a próxima banana sem executar o resto do corpo
            for (int i = 0; i < MAX_BANANAS; i++)
            {
                if (!bananas[i].active)
                    continue;

                // Diminui o tempo de vida da banana
                bananas[i].lifetime -= time;
                if (bananas[i].lifetime <= 0.0f)
                {
                    bananas[i].active = false;
                    continue;
                }

                // A banana está parada, então só precisa checar a colisão com o jogador
                bananas[i].hitbox = (Rectangle){bananas[i].position.x, bananas[i].position.y, BANANA_LARGURA, BANANA_ALTURA};

                // Ao tocar no jogador a banana some. Se ele estiver atacando (J), ele
                // destrói a banana e sobrevive; se não, ele morre.
                if (player->alive && CheckCollisionRecs(player->hitbox, bananas[i].hitbox))
                {
                    bananas[i].active = false;
                    if (player->estado_player != ATTACKING)
                    {
                        player->alive = false;
                        PlaySound(somMorte);
                    }
                }
            }
}