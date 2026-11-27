#include "raylib.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>
#include "mecanicas.h"
#include "placar.h"

int main()
{
    // RENDERIZAÇÃO BÁSICA DO MAPA
    char mapa[LINHAS][COLUNAS], caminho[50];
    int inim_qntd = 0, fase = 1;
    float pontuacao = PONTUACAO_INICIAL, tempo_fase = 0.0f;
    double tempo_total = 0.0;

    // inicializacao do placar
    int qtd_placar = 0, tam_nome = 0;
    char nome[MAX_NOME] = "";
    bool entrou_placar = false;
    TIPO_PLACAR placar[MAX_PLACAR];
    carrega_placar(placar, &qtd_placar);

    sprintf(caminho, "maps/mapa%d.txt", fase);

    if (carregaMapa(caminho, mapa) == 0)
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
        .hitbox = {0.0f},
        .estado_player = IDLE,
        .vidas = MAX_VIDAS
    };

    Enemy exams[MAX_INIM];

    Monkey monkey = {
        .throw_interval = 3.0f,
        .throw_cooldown = 3.0f,
        .hitbox = {0}};

    Ru ru;

    Banana bananas[MAX_BANANAS] = {0};

    posicoes_mapa(&player, exams, &inim_qntd, mapa, &monkey, &ru);

    // Inicialização das Animações
    Animation anim_idle = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_run = {.first_frame = 4, .last_frame = 7, .current_frame = 4, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_jump = {.first_frame = 8, .last_frame = 11, .current_frame = 11, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};
    Animation anim_attack = {.first_frame = 12, .last_frame = 15, .current_frame = 12, .step = 1, .speed = 0.06f, .duration_left = 0.06f, .type = ONESHOT};
    Animation anim_goblin = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 0.08f, .duration_left = 0.08f, .type = REPEATING};
    Animation anim_macaco = {.first_frame = 0, .last_frame = 3, .current_frame = 0, .step = 1, .speed = 1.0f, .duration_left = 0.08f, .type = REPEATING};

    // INICIALIZAÇÃO DOS ESTADOS DE ANIMAÇÃO E DAS DIREÇÕES
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
    Texture2D macaco_text = LoadTexture("assets/inimigos/macaco.png");
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

        switch (estado_jogo)
        {
        case MENU:
            UpdateMusicStream(musiquinhaMenu);
            if (IsKeyPressed(KEY_ENTER))
            {
                estado_jogo = GAME;
                pontuacao = PONTUACAO_INICIAL;
                tempo_fase = 0.0f;
                tempo_total = 0.0;
                inim_qntd = 0;
                player.alive = true;
                player.vidas = MAX_VIDAS;
                player.velocity = (Vector2){0.0f, 0.0f};
                player.estado_player = IDLE;
                player.is_grounded = true;
                fase = 1;

                for (int i = 0; i < MAX_BANANAS; i++)
                {
                    bananas[i].active = false;
                }
                sprintf(caminho, "maps/mapa%d.txt", fase);

                carregaMapa(caminho, mapa);

                posicoes_mapa(&player, exams, &inim_qntd, mapa, &monkey, &ru);
            }
            else if (IsKeyPressed(KEY_R))
            {
                estado_jogo = PLACAR;
            }

            break;
        case GAME:
            if (IsKeyPressed(KEY_TAB))
                estado_jogo = PAUSE;

            if(!IsMusicStreamPlaying(musiquinhaTeste))
                PlayMusicStream(musiquinhaTeste);

            UpdateMusicStream(musiquinhaTeste);

            float delta_time = GetFrameTime();
            previous_state = player.estado_player;

            // Logica da pontuacao
            if (player.alive)
            {
                tempo_fase += delta_time;
                tempo_total += delta_time;
                pontuacao -= (delta_time * 50.0f);

                banana_macaco(&monkey, delta_time, &player, bananas, mapa, somMorte);

                // TESTA SE O JOGADOR ESTÁ VIVO OU NÃO
                for (int i = 0; i < inim_qntd; i++)
                {
                if (CheckCollisionRecs(player.hitbox, exams[i].hitbox))
                {
                    if (player.estado_player == ATTACKING)
                    {
                        exams[i].alive = false;
                        inim_qntd--;
                        PlaySound(somMorte);
                    }
                    else
                    {
                        player.vidas--;
                        player.position.x = 0;
                        PlaySound(somMorte);
                    }
                }
                }

                if (CheckCollisionRecs(player.hitbox, monkey.hitbox))
                {
                player.vidas--;
                player.position.x = 0;

                PlaySound(somMorte);
                }

                if (CheckCollisionRecs(player.hitbox, ru.hitbox))
            {
                pontuacao += (float)bonus_tempo(tempo_fase);
                tempo_fase = 0.0f;
                player.velocity = (Vector2){0.0f, 0.0f};
                player.estado_player = IDLE;
                player.is_grounded = true;

                for (int i = 0; i < MAX_BANANAS; i++)
                {
                    bananas[i].active = false;
                }

                fase++;
                sprintf(caminho, "maps/mapa%d.txt", fase);

                if (carregaMapa(caminho, mapa) == 0)
                {
                    entrou_placar = entra_placar(placar, qtd_placar, (double)pontuacao);
                    nome[0] = '\0';
                    tam_nome = 0;
                    estado_jogo = VITORIA;
                }
                else
                {
                    inim_qntd = 0;
                    posicoes_mapa(&player, exams, &inim_qntd, mapa, &monkey, &ru);
                }
            }

            }

            if (pontuacao < 0.0f)
                pontuacao = 0.0f;

            // 1. Lógica, Controles e posição do Jogador
            update_player(&player, settings, jumpSound, delta_time);

            // 2. Lógica e Colisão do Inimigo com o mapa
            for (int i = 0; i < inim_qntd; i++)
            {
                update_inimigo(&exams[i], settings, delta_time, mapa);
                exams[i].hitbox = (Rectangle){exams[i].position.x, exams[i].position.y, RENDER_SIZE, RENDER_SIZE};
            }

            checa_colisoes(mapa, &player, exams, inim_qntd);

            if (player.vidas == 0){
                player.alive = false;
                if(IsKeyPressed(KEY_Q))
                    estado_jogo = MENU;
            }

            // TROCA ESTADO DE ANIMAÇÃO DO JOGADOR
            switch (player.estado_player)
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
                if (player.estado_player != previous_state)
                {
                    reset_animation(current_anim);
                }
                animation_update(current_anim);
            }

            animation_update(&anim_goblin);
            animation_update(&anim_macaco);

            break;

        case PAUSE:
            if (IsKeyPressed(KEY_ENTER))
                estado_jogo = GAME;
            else if (IsKeyPressed(KEY_Q))
                estado_jogo = MENU;

            break;

        case PLACAR:
            UpdateMusicStream(musiquinhaMenu);

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                estado_jogo = MENU;
            }
            break;

        case VITORIA:
            UpdateMusicStream(musiquinhaMenu);

            int tecla = GetCharPressed();
            if (entrou_placar == true)
            {
                while (tecla > 0)
                {
                    if (((tecla >= 'a' && tecla <= 'z') || (tecla >= 'A' && tecla <= 'Z') || (tecla >= '0' && tecla <= '9') || tecla == '_' || tecla == '-') && tam_nome < MAX_NOME - 1)
                    {
                        nome[tam_nome] = (char)tecla;
                        tam_nome++;
                        nome[tam_nome] = '\0';
                    }
                    tecla = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && tam_nome > 0)
                {
                    tam_nome--;
                    nome[tam_nome] = '\0';
                }
                if (IsKeyPressed(KEY_ENTER) && tam_nome > 0)
                {
                    inserir_no_placar(placar, &qtd_placar, nome, tempo_total, (double)pontuacao);
                    salvar_placar(placar);
                    estado_jogo = PLACAR;
                }
            }
            else if (!entrou_placar)
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    estado_jogo = MENU;
                }
            }

            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // SWITCH 2: DESENHO
        switch (estado_jogo)
        {
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
                //DrawRectangleLines((int)player.hitbox.x, (int)player.hitbox.y, (int)player.hitbox.width, (int)player.hitbox.height, GREEN);
            }
            else
            {
                ClearBackground(GRAY);
                StopMusicStream(musiquinhaTeste);
                DrawText("GAME OVER! Pressione Q para sair", 300, 450, 20, RED);
            }

            DrawText(TextFormat("VIDAS RESTANTES: %d", player.vidas), 700, 10, 20, RED);

            // Desenha o Inimigo
            for (int i = 0; i < inim_qntd; i++)
            {
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
                    //DrawRectangleLines((int)exams[i].hitbox.x, (int)exams[i].hitbox.y, (int)exams[i].hitbox.width, (int)exams[i].hitbox.height, RED);
                }
            }

            // desenha macaco
            Rectangle macaco_frame = animation_frame(&anim_macaco, 4);
            Rectangle desenho_macaco = {monkey.position.x, monkey.position.y, RENDER_SIZE, RENDER_SIZE};
            DrawTexturePro(macaco_text, macaco_frame, desenho_macaco, origem, rotacao, WHITE);

            for (int i = 0; i < MAX_BANANAS; i++)
            {
                if (bananas[i].active)
                {
                    DrawEllipse((int)(bananas[i].position.x + BANANA_LARGURA / 2.0f), (int)(bananas[i].position.y + BANANA_ALTURA / 2.0f), 8, 4, YELLOW);
                    //DrawRectangleLines((int)bananas[i].position.x, (int)bananas[i].position.y, (int)BANANA_LARGURA, (int)BANANA_ALTURA, RED);
                }
            }

            DrawText(TextFormat("FASE: %d", fase), 20, 70, 24, BLACK);
            DrawText(TextFormat("PONTOS: %d", (int)pontuacao), 20, 10, 20, BLACK);
            DrawText(TextFormat("TEMPO: %.1fs", tempo_fase), 20, 40, 20, BLACK);

            break;

        case PAUSE:
            ClearBackground(LIGHTGRAY);
            DrawTexture(pause_text, 500.0f, 500.0f, LIGHTGRAY);
            DrawText("JOGO PAUSADO", 100, 150, 100, DARKBROWN);
            DrawText("Aperte ENTER ou Q Para Sair", 100, 300, 50, DARKBROWN);
            break;

        case PLACAR:
            DrawTexture(menu_text, 0.0f, 0.0f, WHITE);
            DrawRectangle(80, 50, 800, 550, Fade(RAYWHITE, 0.88f));
            DrawText("TOP 10 PLACAR", 120, 70, 32, BLACK);
            DrawText("PRESSIONE BACKSPACE PARA VOLTAR", 120, 560, 32, BLACK);

            int i, y;

            for (i = 0; i < MAX_PLACAR; i++)
            {
                y = 130 + i * 42;
                DrawText(TextFormat("%2d. %-19s %6.0f pts  (%.1fs)", i + 1, placar[i].nome, placar[i].pontuacao, placar[i].tempo), 120, y, 24, BLACK);
            }
            break;

        case VITORIA:
            DrawTexture(menu_text, 0.0f, 0.0f, WHITE);
            DrawRectangle(60, 40, 840, 560, Fade(RAYWHITE, 0.88f));
            DrawText("PARABENS! VOCE VENCEU!", 200, 80, 24, BLACK);
            DrawText(TextFormat("PONTUACAO FINAL: %.0f", (double)pontuacao), 200, 140, 24, BLACK);
            DrawText(TextFormat("TEMPO FINAL: %.1fs", (tempo_total)), 200, 190, 24, BLACK);

            if (entrou_placar)
            {
                DrawText("NOVO RECORDE! Digite seu nome (max 19 caracteres, s/ espaco)", 120, 260, 24, BLACK);
                DrawRectangleLines(80, 305, 500, 50, BLACK);
                DrawText(TextFormat("%s_", nome), 135, 315, 24, BLACK);
                DrawText("ENTER para salvar", 80, 380, 24, BLACK);
            }
            else
            {
                DrawText("Nao entrou no placar. Pressione ENTER para voltar ao menu.", 120, 280, 32, BLACK);
            }
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
