#include "raylib.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>
#include "raymath.h"
#include <math.h>
#include "mecanicas.h"

// Tamanho de cada frame dentro do ficheiro da imagem original (ex: 256x256 por frame)
#define FRAME_WIDTH 256.0f
#define FRAME_HEIGHT 256.0f

// Tamanho visual e de colisão no ecrã (igual ao bloquinho (tile) do mapa: 32x32)
#define TAM_TILE 32.0f
#define RENDER_SIZE 32.0f

// Hitbox da banana
#define BANANA_LARGURA 10.0f
#define BANANA_ALTURA 6.0f

// subida na escada
#define SUBIR_ESCADA -450.0f

#define DESCER_ESCADA (2 * RENDER_SIZE)

#define MAX_INIM 6

int main(){
    // RENDERIZAÇÃO BÁSICA DO MAPA
    char mapa[LINHAS][COLUNAS];

    int inim_qntd = 0;

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
        .direcao = RIGHT,
        .hitbox = {0.0f}
    };

    Enemy exams[MAX_INIM];

    Monkey monkey = {
        .throw_interval = 3.0f,
        .throw_cooldown = 3.0f,
        .hitbox = {0}
    };

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
                exams[inim_qntd] = (Enemy){
                    .position = (Vector2){j * TAM_TILE, i * TAM_TILE},
                    .velocity = (Vector2){0.0f, 0.0f},
                    .speed = 90.0f,
                    .is_grounded = false,
                    .alive = true,
                    .direcao = LEFT,
                    .cooldown = 0.0f
                };

                inim_qntd++;
                if (inim_qntd > MAX_INIM)
                    inim_qntd = MAX_INIM;
                
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'M')
            {
                monkey.position = (Vector2){j * TAM_TILE, i * TAM_TILE};
                monkey.hitbox = (Rectangle){monkey.position.x, monkey.position.y, RENDER_SIZE, RENDER_SIZE};
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'F')
            {
                linha_ru = i;
            }
        }
    }

    // Inicialização das Animações
    Animation anim_idle = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_run = {.first_frame = 4, .last_frame = 7, .current_frame = 4, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_jump = {.first_frame = 8, .last_frame = 11, .current_frame = 11, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};
    Animation anim_attack = {.first_frame = 12, .last_frame = 15, .current_frame = 12, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};
    Animation anim_goblin = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1,.speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};

    // INICIALIZAÇÃO DOS ESTADOS DE ANIMAÇÃO E DAS DIREÇÕES
    PlayerState current_state = IDLE;
    PlayerState previous_state = IDLE;
    Animation *current_anim = &anim_idle;

    // INICIALIZAÇÃO DA JANELA, AUDIO, FPS E TEXTURAS
    InitWindow(settings.screen_width, settings.screen_height, "Donkey Kong do RU");
    InitAudioDevice();
    SetTargetFPS(settings.target_fps);
    
    Texture2D pause_text = LoadTexture("assets/menus/cara.png");
    Texture2D menu_text = LoadTexture("assets/menus/tema_de_fundo.png");
    Texture2D player_text = LoadTexture("assets/jogador/jogador.png");
    Texture2D ladder_text = LoadTexture("assets/mapa/escada2.png");
    Texture2D exam_text = LoadTexture("assets/inimigos/inimigoteste.png");
    Texture2D grama_text = LoadTexture("assets/mapa/grama2.png");
    Texture2D ru_text = LoadTexture("assets/mapa/ru.png");
    Sound jumpSound = LoadSound("assets/sons/jump.mp3");
    Sound somMorte = LoadSound("assets/sons/sommorte.mp3");
    Music musiquinhaTeste = LoadMusicStream("assets/musica/musicafundo.mp3");
    Music musiquinhaMenu = LoadMusicStream("assets/musica/bachsinfonia.mp3");

    PlayMusicStream(musiquinhaTeste);
    PlayMusicStream(musiquinhaMenu);

    // LOOP PRINCIPAL
    while (!WindowShouldClose())
    {

        // Hitboxes proporcionais ao tamanho de 32x32
        player.hitbox = (Rectangle){player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};

        switch(estado_jogo){
            case MENU:
                UpdateMusicStream(musiquinhaMenu);
                if (IsKeyPressed(KEY_ENTER)) estado_jogo = GAME;
                break;
            case GAME:
                if(IsKeyPressed(KEY_TAB)) estado_jogo = PAUSE; 

                UpdateMusicStream(musiquinhaTeste);

            float delta_time = GetFrameTime();
            previous_state = current_state;

            // 1. Lógica, Controles e posição do Jogador
            update_player(&player, &current_state, settings, jumpSound, delta_time);
            
            // 2. Lógica e Colisão do Inimigo com o mapa
            for(int i = 0; i < inim_qntd; i++){
                update_inimigo(&exams[i], settings, delta_time, mapa);
                exams[i].hitbox = (Rectangle){exams[i].position.x, exams[i].position.y, RENDER_SIZE, RENDER_SIZE};
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
                        frente = player.direcao * 3.0f * TAM_TILE;
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
                bananas[i].hitbox = (Rectangle){bananas[i].position.x, bananas[i].position.y, BANANA_LARGURA, BANANA_ALTURA};

                // Ao tocar no jogador a banana some. Se ele estiver atacando (J), ele
                // destrói a banana e sobrevive; se não, ele morre.
                if (player.alive && CheckCollisionRecs(player.hitbox, bananas[i].hitbox))
                {
                    bananas[i].active = false;
                    if (current_state != ATTACKING)
                    {
                        player.alive = false;
                        PlaySound(somMorte);
                    }
                }
            }

            checa_colisoes(mapa, &player, &current_state, exams, inim_qntd);


            // TESTA SE O JOGADOR ESTÁ VIVO OU NÃO
            for(int i = 0; i < inim_qntd; i++){
                if (player.alive && CheckCollisionRecs(player.hitbox, exams[i].hitbox))
                {
                    if (current_state == ATTACKING)
                    {
                        exams[i].alive = false;
                        inim_qntd--;
                        PlaySound(somMorte);
                    }
                    else
                    {
                        player.alive = false;
                        PlaySound(somMorte);
                    }
                }
            }

            if (player.alive && CheckCollisionRecs(player.hitbox, monkey.hitbox)){
                player.alive = false;
                PlaySound(somMorte);
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

            animation_update(&anim_goblin);

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

        //SWITCH 2: DESENHO
        switch(estado_jogo){
            case MENU:
                DrawTexture(menu_text, 0.0f, 0.0f, WHITE);
                DrawText("ENTER para iniciar", 40, menu_text.height / 3, 30, DARKBLUE);
                DrawText("R para ranking", 40, menu_text.height / 2.5, 30, DARKBLUE);
                DrawText("ESC para sair", 40, menu_text.height / 2, 30, DARKBLUE);
                break;

            case GAME:
            
                Vector2 origem = {0.0f, 0.0f};
                float rotacao = 0.0f;

                desenha_mapa(mapa, grama_text, ladder_text, ru_text);


                // Desenha o Jogador
                if (player.alive)
                {
                    Rectangle player_frame = animation_frame(current_anim, 4);
                    Rectangle desenho_player = {player.position.x, player.position.y, RENDER_SIZE, RENDER_SIZE};

                    // ATULIZA DIRECAO DO PLAYER NA SPRITE (inverte)
                    if (player.direcao == LEFT)
                    {
                        player_frame.width = -player_frame.width;
                    }

                    // DESENHA O PLAYER E HITBOX PARA TESTES
                    DrawTexturePro(player_text, player_frame, desenho_player, origem, rotacao, WHITE);
                    DrawRectangleLines((int)player.hitbox.x, (int)player.hitbox.y, (int)player.hitbox.width, (int)player.hitbox.height, GREEN);
                }
                else
                {
                    DrawText("VOCE MORREU! Pressione R para reiniciar", 150, 450, 20, RED);
                }

                // Desenha o Inimigo
                for(int i = 0; i < inim_qntd; i++){
                    if (exams[i].alive)
                    {
                        Rectangle exam_frame = animation_frame(&anim_goblin, 4);
                        Rectangle desenho_exam = {exams[i].position.x, exams[i].position.y, RENDER_SIZE, RENDER_SIZE};

                        // ATUALIZA DIRECAO DO INIMIGO NA SPRITE(inverte)
                        if (exams[i].direcao == LEFT)
                        {
                            exam_frame.width = -exam_frame.width;
                        }

                        // DESENHA O INIMIGO E HITBOX PARA TESTES
                        DrawTexturePro(exam_text, exam_frame, desenho_exam, origem, rotacao, WHITE);
                        DrawRectangleLines((int)exams[i].hitbox.x, (int)exams[i].hitbox.y, (int)exams[i].hitbox.width, (int)exams[i].hitbox.height, RED);
                    }
                }

                //desenha macaco
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
                ClearBackground(LIGHTGRAY);
                DrawTexture(pause_text, 500.0f, 500.0f, LIGHTGRAY);
                DrawText("JOGO PAUSADO", 100, 150, 100, DARKBROWN);
                DrawText("Aperte ENTER ou Q Para Sair", 100, 300,50, DARKBROWN);
                break;
        }

        EndDrawing();
    }

    // DESCARREGA MUSICA, TEXTURAS, FECHA AUDIO E JANELA
    UnloadMusicStream(musiquinhaTeste);
    UnloadMusicStream(musiquinhaMenu);
    UnloadSound(jumpSound);
    UnloadSound(somMorte);
    UnloadTexture(menu_text);
    UnloadTexture(pause_text);
    UnloadTexture(ladder_text);
    UnloadTexture(player_text);
    UnloadTexture(exam_text);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
