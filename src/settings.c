#include "settings.h"
#include "raylib.h"
#include <stdio.h>

// FUNCAO CARREGA MAPA DEFINIDA
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

void posicoes_mapa(PlayerSettings *player, Enemy exams[], int *inim_qntd, char mapa[LINHAS][COLUNAS], Monkey *monkey, Ru *ru){
    // Posicionamento inicial do player e do inimigo com base na matriz do mapa
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            if (mapa[i][j] == 'P')
            {
                player->position = (Vector2){j * TAM_TILE, i * TAM_TILE};
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'E')
            {
                exams[*inim_qntd] = (Enemy){
                    .position = (Vector2){j * TAM_TILE, i * TAM_TILE},
                    .velocity = (Vector2){0.0f, 0.0f},
                    .speed = 90.0f,
                    .is_grounded = false,
                    .alive = true,
                    .direcao = LEFT,
                    .cooldown = 0.0f
                };

                (*inim_qntd)++;
                if (*inim_qntd > MAX_INIM)
                    *inim_qntd = MAX_INIM;
                
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'M')
            {
                monkey->position = (Vector2){j * TAM_TILE, i * TAM_TILE};
                monkey->hitbox = (Rectangle){monkey->position.x, monkey->position.y, RENDER_SIZE, RENDER_SIZE};
                mapa[i][j] = ' ';
            }
            else if (mapa[i][j] == 'F')
            {
                ru->hitbox = (Rectangle){j * TAM_TILE, i * TAM_TILE, RENDER_SIZE, RENDER_SIZE};
                ru->linha_ru = i;
            }
        }
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
