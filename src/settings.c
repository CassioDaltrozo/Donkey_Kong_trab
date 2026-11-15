#include "settings.h"
#include "raylib.h"
#include <stdio.h>

// UPTADE DE ANIMAÇAO, USANDO PONTEIROS
void animation_update(Animation *self)
{
    float delta_time = GetFrameTime();
    self->duration_left -= delta_time;

    if (self->duration_left <= 0.0)
    {
        self->duration_left = self->speed;

        if (self->current_frame == self->last_frame && self->type == ONESHOT)
        {
            return;
        }

        self->current_frame += self->step;

        if (self->current_frame > self->last_frame)
        {
            switch (self->type)
            {
            case REPEATING:
                self->current_frame = self->first_frame;
                break;

            case ONESHOT:
                self->current_frame = self->last_frame;
                break;
            }
        }
        else if (self->current_frame < self->first_frame)
        {
            switch (self->type)
            {
            case REPEATING:
                self->current_frame = self->last_frame;
                break;

            case ONESHOT:
                self->current_frame = self->first_frame;
                break;
            }
        }
    }
}

// RETORNO DO FRAME DA PNG PARA DESENHO
Rectangle animation_frame(Animation *self, int num_frames_per_row)
{
    float x = (float)(self->current_frame % num_frames_per_row) * 256.0f;
    float y = (float)(self->current_frame / num_frames_per_row) * 256.0f;

    return (Rectangle){.x = x, .y = y, .width = 256.0f, .height = 256.0f};
}

// RESET PARA PERMANENCIA
void reset_animation(Animation *self)
{
    self->current_frame = self->first_frame;
    self->duration_left = self->speed;
}

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
