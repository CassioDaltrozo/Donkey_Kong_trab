#include "raylib.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>
#include "raymath.h"
#include <math.h>

// Tamanho de cada frame dentro do ficheiro da imagem original (ex: 256x256 por frame)
#define FRAME_WIDTH 256.0f
#define FRAME_HEIGHT 256.0f

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define TAM_TILE 32.0f
#define RENDER_SIZE 32.0f

// Hitbox da banana
#define BANANA_LARGURA 10.0f
#define BANANA_ALTURA 6.0f

// Chance de 0 a 100 do inimigo usar a escada
#define CHANCE_ESCADA 60
// Tempo antes do inimigo usar outra escada
#define COOLDOWN_ESCADA 2.0f

// subida na escada
#define SUBIR_ESCADA -450.0f

#define DESCER_ESCADA (2 * RENDER_SIZE)
int main()
{
    // RENDERIZAÇÃO BÁSICA DO MAPA
    char mapa[LINHAS][COLUNAS];

    if (carregaMapa("maps/mapa1.txt", mapa) == 0)
    {
        printf("Erro ao carregar o mapa!\n");
        return 0;
    }

    // DEFINIÇÃO DE DADOS DO JOGO, DO JOGADOR E DO INIMIGO 1
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
        .jump_force = -380.0f,
    };

    Enemy exam = {
        .velocity = {0.0f, 0.0f},
        .speed = 90.0f,
        .is_grounded = false,
        .alive = true};

    Monkey monkey = {
        .throw_interval = 3.0f,
        .throw_cooldown = 3.0f};

    Banana bananas[MAX_BANANAS] = {0};

    int linha_ru = -1;

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
            else if (mapa[i][j] == 'M')
            {
                monkey.position = (Vector2){
                    j * TAM_TILE, i * TAM_TILE};
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'F')
            {
                linha_ru = i;
            }
        }
    }

    // Inicialização das Animações
    Animation anim_idle = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 0.15f, .duration_left = 0.15f, .type = REPEATING};
    Animation anim_run = {.first_frame = 4, .last_frame = 7, .current_frame = 4, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_jump = {.first_frame = 8, .last_frame = 11, .current_frame = 11, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};
    Animation anim_attack = {.first_frame = 12, .last_frame = 15, .current_frame = 12, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};

    // INICIALIZAÇÃO DOS ESTADOS DE ANIMAÇÃO E DAS DIREÇÕES
    PlayerState current_state = IDLE;
    PlayerState previous_state = IDLE;
    Direction player_direction = RIGHT;
    Direction exam_direction = LEFT;
    bool exam_na_escada = false;
    float exam_cooldown_escada = 0.0f;
    Animation *current_anim = &anim_idle;

    // INICIALIZAÇÃO DA JANELA, AUDIO, FPS E TEXTURAS
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

    // LOOP PRINCIPAL
    while (!WindowShouldClose())
    {

        // Hitboxes proporcionais ao tamanho de 32x32
        Rectangle player_hitbox = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};
        Rectangle exam_hitbox = {0};

        switch (estado_jogo)
        {
        case MENU:
            if (IsKeyPressed(KEY_ENTER))
                estado_jogo = GAME;
            break;
        case GAME:
            if (IsKeyPressed(KEY_TAB))
                estado_jogo = PAUSE;

            UpdateMusicStream(musiquinhaTeste);

            float delta_time = GetFrameTime();
            previous_state = current_state;

            // 1. Lógica, Controles e posição do Jogador
            if (player.alive)
            {
                player.velocity.x = 0.0f;

                if (!player.is_grounded)
                {
                    player.velocity.y += settings.gravity * delta_time;
                }

                if (IsKeyPressed(KEY_J))
                {
                    current_state = ATTACKING;
                }

                if (player.position.x < 0)
                    player.position.x = 0;

                if (player.position.x > (COLUNAS * TAM_TILE - RENDER_SIZE))
                    player.position.x = (COLUNAS * TAM_TILE - RENDER_SIZE);

                if (IsKeyDown(KEY_SPACE) && player.is_grounded)
                {
                    player.velocity.y = player.jump_force;
                    player.is_grounded = false;
                    current_state = JUMPING;
                    PlaySound(jumpSound);
                }

                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                {
                    if (player.is_grounded)
                        current_state = RUNNING;

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                    {
                        player.velocity.x = -player.speed;
                        player_direction = LEFT;
                    }

                    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                    {
                        player.velocity.x = player.speed;
                        player_direction = RIGHT;
                    }
                }
                else
                {
                    if (player.is_grounded && current_state == RUNNING)
                    {
                        current_state = IDLE;
                    }
                }

                if ((current_state != ATTACKING || current_anim->current_frame == current_anim->last_frame) && player.is_grounded && player.velocity.x == 0.0f)
                {
                    current_state = IDLE;
                }
            }
            else
            {
                if (IsKeyPressed(KEY_R))
                    player.alive = true;
            }

            player.position.x += player.velocity.x * delta_time;
            player.position.y += player.velocity.y * delta_time;

            // 2. Lógica e Colisão do Inimigo com o mapa
            if (exam.alive)
            {
                exam.velocity.x = exam.speed * exam_direction;

                if (!exam.is_grounded)
                {
                    exam.velocity.y += settings.gravity * delta_time;
                }

                exam.position.x += exam.velocity.x * delta_time;
                exam.position.y += exam.velocity.y * delta_time;

                exam_hitbox = (Rectangle){exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE};

                if (exam.position.x <= 0 || exam.position.x >= (COLUNAS * TAM_TILE - RENDER_SIZE))
                {
                    exam_direction *= -1;
                }
            }

            // Macaco e bananas
            if (player.alive)
            {
                // Só conta o tempo com o jogador vivo
                monkey.throw_cooldown -= delta_time;

                if (monkey.throw_cooldown <= 0.0f)
                {
                    monkey.throw_cooldown = monkey.throw_interval;

                    // Preve onde o jogador vai estar
                    // Andando: velocidade x tempo = distância que ele percorre em 1,2 s
                    // Parado: o macaco aposta que ele vai andar para o lado que olha
                    float frente;
                    if (player.velocity.x != 0.0f)
                    {
                        frente = player.velocity.x * 1.2f;
                    }
                    else
                    {
                        frente = player_direction * 3.0f * TAM_TILE;
                    }
                    float alvo_x = player.position.x + frente;

                    // Não deixa o alvo sair da tela.
                    if (alvo_x < 0.0f)
                        alvo_x = 0.0f;
                    if (alvo_x > COLUNAS * TAM_TILE - RENDER_SIZE)
                        alvo_x = COLUNAS * TAM_TILE - RENDER_SIZE;

                    // Converter posição em coluna e linha do mapa
                    // Dividir por TAM_TILE diz em qual bloquinho estamos
                    // Usamos o centro do jogador (+ RENDER_SIZE / 2) para evitar erro de borda
                    int coluna = (int)((alvo_x + RENDER_SIZE / 2.0f) / TAM_TILE);
                    int linha_jogador = (int)((player.position.y + RENDER_SIZE / 2.0f) / TAM_TILE);

                    // PASSO 3: achar o chão nessa coluna, descendo no máximo 2 linhas.
                    int linha_chao = achaChao(mapa, coluna, linha_jogador, 2);

                    // Posição X da banana: centralizada dentro do bloquinho
                    float banana_x = coluna * TAM_TILE + (TAM_TILE - BANANA_LARGURA) / 2.0f;

                    // Regras que impedem o arremesso
                    bool pode_jogar = true;

                    // 1. não achou chão
                    if (linha_chao == -1)
                        pode_jogar = false;

                    // 2. o chão achado é o da plataforma do RU
                    if (pode_jogar && linha_chao <= linha_ru + 1)
                        pode_jogar = false;

                    // 3. a banana cairia em cima do jogador
                    if (pode_jogar && fabsf(banana_x - player.position.x) < TAM_TILE)
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
            }

            // Atualiza cada banana do estoque
            // continue pula para a próxima banana sem executar o resto do corpo
            for (int i = 0; i < MAX_BANANAS; i++)
            {
                if (!bananas[i].active)
                    continue;

                // Diminui o tempo de vida da banana
                bananas[i].lifetime -= delta_time;
                if (bananas[i].lifetime <= 0.0f)
                {
                    bananas[i].active = false;
                    continue;
                }

                // A banana está parada, então só precisa checar a colisão com o jogador
                Rectangle banana_hitbox = {bananas[i].position.x, bananas[i].position.y, BANANA_LARGURA, BANANA_ALTURA};

                // Ao tocar no jogador a banana some. Se ele estiver atacando (J), ele
                // destrói a banana e sobrevive; se não, ele morre.
                if (player.alive && CheckCollisionRecs(player_hitbox, banana_hitbox))
                {
                    bananas[i].active = false;
                    if (current_state != ATTACKING)
                    {
                        player.alive = false;
                        PlaySound(somMorte);
                    }
                }
            }

            // 3. Colisão do Jogador e do inimigo com o mapa ('Z'), com o vento (' ') e com a escada ('S')
            for (int i = 0; i < LINHAS; i++)
            {
                for (int j = 0; j < COLUNAS; j++)
                {
                    Rectangle tileRect = {j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE};

                    if (mapa[i][j] == 'Z')
                    {
                        // COLISAO DO PLAYER COM O CHAO
                        if (CheckCollisionRecs(player_hitbox, tileRect))
                        {
                            if (player.velocity.y >= 0)
                            {
                                player.position.y = (i * TAM_TILE) - RENDER_SIZE;
                                player.velocity.y = 0.0f;
                                player.is_grounded = true;
                            }
                        }

                        // COLISAO DO INIMIGO COM O CHAO
                        if (CheckCollisionRecs(exam_hitbox, tileRect))
                        {
                            if (exam.velocity.y >= 0)
                            {
                                exam.position.y = (i * TAM_TILE) - RENDER_SIZE;
                                exam.velocity.y = 0.0f;
                                exam.is_grounded = true;
                            }
                        }
                        else
                            exam.is_grounded = false;
                    }
                    // bug aleatorio do player corrigido
                    if (mapa[i][j] == ' ')
                    {
                        if (CheckCollisionRecs(player_hitbox, tileRect) && player.velocity.y == 0)
                        {
                            player.is_grounded = false;
                        }
                    }

                    // subindo escada com player no s
                    if (mapa[i][j] == 'S')
                    {
                        if (CheckCollisionRecs(player_hitbox, tileRect))
                        {
                            if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.velocity.x == 0)
                            {
                                player.velocity.y = SUBIR_ESCADA;
                                player.is_grounded = false;
                                current_state = JUMPING;
                            }
                        }
                    }

                    // descendo escada com player no d
                    if (mapa[i][j] == 'D')
                    {
                        if (CheckCollisionRecs(player_hitbox, tileRect))
                        {
                            if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && player.velocity.x == 0)
                            {
                                player.position.y += DESCER_ESCADA;
                                current_state = JUMPING;
                            }
                        }
                    }
                }
            }

            // Escadas do inimigo
            if (exam.alive)
            {
                // Diminui o tempo de espera até chegar a zero
                if (exam_cooldown_escada > 0.0f)
                    exam_cooldown_escada -= delta_time;

                // Descobre em qual bloquinho do mapa está o centro do inimigo
                int col_exam = (int)((exam.position.x + RENDER_SIZE / 2.0f) / TAM_TILE);
                int lin_exam = (int)((exam.position.y + RENDER_SIZE / 2.0f) / TAM_TILE);

                // Lê o caractere do mapa nesse ponto, com cuidado para não sair da matriz
                char tile_exam = ' ';
                if (col_exam >= 0 && col_exam < COLUNAS && lin_exam >= 0 && lin_exam < LINHAS)
                {
                    tile_exam = mapa[lin_exam][col_exam];
                }

                // Quando o inimigo está apoiado no chão, a colisão deixa velocity.y em zero
                // Se estiver caindo (> 0) ou subindo (< 0), não deve usar escada
                bool no_chao = (exam.velocity.y == 0.0f);
                bool em_escada = (tile_exam == 'S' || tile_exam == 'D');

                // Só sorteia quando está numa escada, nao estava no frame anterior
                // (acabou de chegar), está no chão e a espera acabou
                if (em_escada && !exam_na_escada && no_chao && exam_cooldown_escada <= 0.0f)
                {
                    // GetRandomValue(1, 100) devolve um inteiro aleatório de 1 a 100
                    // Se for menor ou igual à chance ele usa a escada
                    if (GetRandomValue(1, 100) <= CHANCE_ESCADA)
                    {
                        if (tile_exam == 'S')
                        {
                            // SUBIR: mesmo impulso do jogador
                            exam.velocity.y = SUBIR_ESCADA;
                            exam.is_grounded = false;
                        }
                        else
                        {
                            // DESCER: mesmo deslocamento do jogador no tile 'D'
                            exam.position.y += DESCER_ESCADA;
                            exam.velocity.y = 0.0f;
                            exam.is_grounded = false;
                        }

                        exam_cooldown_escada = COOLDOWN_ESCADA;
                    }
                }

                // Guarda para o próximo frame se ele estava em uma escada
                exam_na_escada = em_escada;
            }

            // TESTA SE O JOGADOR ESTÁ VIVO OU NÃO
            if (player.alive && CheckCollisionRecs(player_hitbox, exam_hitbox))
            {
                if (current_state == ATTACKING)
                {
                    exam.alive = false;
                    PlaySound(somMorte);
                }
                else
                {
                    player.alive = false;
                    PlaySound(somMorte);
                }
            }

            // TROCA ESTADO DE ANIMAÇÃO DO JOGADOR
            switch (current_state)
            {
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

            if (player.alive)
            {
                if (current_state != previous_state)
                {
                    reset_animation(current_anim);
                }
                animation_update(current_anim);
            }

            break;

        case PAUSE:
            if (IsKeyPressed(KEY_ENTER))
                estado_jogo = GAME;
            else if (IsKeyPressed(KEY_Q))
                estado_jogo = MENU;

            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (estado_jogo)
        {
        case MENU:
            DrawText("Aperte ENTER ou ESC", COLUNAS * TAM_TILE / 2, LINHAS * TAM_TILE / 2, 20, RED);
            break;
        case GAME:
            // Desenha os elementos do mapa
            Vector2 origem = {0.0f, 0.0f};
            float rotacao = 0.0f;

            for (int i = 0; i < LINHAS; i++)
            {
                for (int j = 0; j < COLUNAS; j++)
                {
                    Rectangle posicao_telha = {j * TAM_TILE, i * TAM_TILE, RENDER_SIZE, RENDER_SIZE};

                    if (mapa[i][j] == 'Z')
                    { // DESENHA CHAO
                        Rectangle grama_frame = {0.0f, 0.0f, grama_text.width, grama_text.height};
                        DrawTexturePro(grama_text, grama_frame, posicao_telha, origem, rotacao, WHITE);
                    }
                    else if (mapa[i][j] == 'S' || mapa[i][j] == 'H' || mapa[i][j] == 'D')
                    { // DESENHA ESCADA
                        Rectangle ladder_frame = {0.0f, 0.0f, ladder_text.width, ladder_text.height};
                        DrawTexturePro(ladder_text, ladder_frame, posicao_telha, origem, rotacao, WHITE);
                    }
                    else if (mapa[i][j] == 'F')
                    { // DESENHA O RU
                        Rectangle ru_frame = {0.0f, 0.0f, ru_text.width, ru_text.height};
                        DrawTexturePro(ru_text, ru_frame, posicao_telha, origem, rotacao, YELLOW);
                    }
                }
            }

            // Desenha o Jogador
            if (player.alive)
            {
                Rectangle player_frame = animation_frame(current_anim, 4);
                Rectangle desenho_player = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};

                // ATULIZA DIRECAO DO PLAYER NA SPRITE (inverte)
                if (player_direction == LEFT)
                {
                    player_frame.width = -player_frame.width;
                }

                // DESENHA O PLAYER E HITBOX PARA TESTES
                DrawTexturePro(player_text, player_frame, desenho_player, origem, rotacao, WHITE);
                DrawRectangleLines((int)player_hitbox.x, (int)player_hitbox.y, (int)player_hitbox.width, (int)player_hitbox.height, GREEN);
            }
            else
            {
                DrawText("VOCE MORREU! Pressione R para reiniciar", 150, 450, 20, RED);
            }

            // Desenha o Inimigo
            if (exam.alive)
            {
                Rectangle exam_frame = {0.0f, 0.0f, (float)exam_text.width, (float)exam_text.height};
                Rectangle desenho_exam = {exam.position.x, exam.position.y, RENDER_SIZE, RENDER_SIZE};

                // ATUALIZA DIRECAO DO INIMIGO NA SPRITE(inverte)
                if (exam_direction == LEFT)
                {
                    exam_frame.width = -exam_frame.width;
                }

                // DESENHA O INIMIGO E HITBOX PARA TESTES
                DrawTexturePro(exam_text, exam_frame, desenho_exam, origem, rotacao, WHITE);
                DrawRectangleLines((int)exam_hitbox.x, (int)exam_hitbox.y, (int)exam_hitbox.width, (int)exam_hitbox.height, RED);
            }

            DrawRectangle((int)monkey.position.x, (int)monkey.position.y, (int)RENDER_SIZE, (int)RENDER_SIZE, BROWN);
            for (int i = 0; i < MAX_BANANAS; i++)
            {
                if (bananas[i].active)
                {
                    DrawEllipse((int)(bananas[i].position.x + BANANA_LARGURA / 2.0f), (int)(bananas[i].position.y + BANANA_ALTURA / 2.0f), 8, 4, YELLOW);
                    DrawRectangleLines((int)bananas[i].position.x, (int)bananas[i].position.y, (int)BANANA_LARGURA, (int)BANANA_ALTURA, RED);
                }
            }

            break;
        case PAUSE:
            DrawText("Aperte ENTER ou Q", COLUNAS * TAM_TILE / 2, LINHAS * TAM_TILE / 2, 20, RED);
            break;
        }
        EndDrawing();
    }

    // DESCARREGA MUSICA, TEXTURAS, FECHA AUDIO E JANELA
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
