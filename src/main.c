#include <stdio.h>
#include "raylib.h"

#define TAM_TILE 20
#define COLUNAS 30
#define LINHAS 30

int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS]);

int main()
{
    int i, j;
    char mapa[LINHAS][COLUNAS];

    // Chama a funcao de carregamento do mapa, e caso ela seja falsa, encerra o programa
    if (carregaMapa("maps/mapa1.txt", mapa) == 0)
    {
        printf("Mapa carregou errado!\n");
        return 0;
    }

    // Inicializa a janela, com as config abaixo
    InitWindow(COLUNAS * TAM_TILE, LINHAS * TAM_TILE, "Donkey Kong do RU");
    SetTargetFPS(60);

    // Laco enquanto a janela estiver aberta
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        // 2 for para percorrer a matriz e desenhar as coisas nas respectivas posicoes
        for (i = 0; i < LINHAS; i++)
        {
            for (j = 0; j < COLUNAS; j++)
            {
                // Desenha as plataformas
                if (mapa[i][j] == 'Z')
                {
                    DrawRectangle(j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, GRAY);
                }

                // Desenha o personagem
                else if (mapa[i][j] == 'P')
                {
                    DrawRectangle(j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, BLUE);
                }

                // Desenha os inimigos
                else if (mapa[i][j] == 'E')
                {
                    DrawRectangle(j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, RED);
                }

                // Desenha as escadas
                else if (mapa[i][j] == 'S' || mapa[i][j] == 'H' || mapa[i][j] == 'D')
                {
                    DrawRectangle(j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, GREEN);
                }

                // Desenha a saida do nivel
                else if (mapa[i][j] == 'F')
                {
                    DrawRectangle(j * TAM_TILE, i * TAM_TILE, TAM_TILE, TAM_TILE, YELLOW);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

int carregaMapa(char nomeArquivo[], char mapa[LINHAS][COLUNAS])
{
    FILE *arq; // Variavel que representa o arquivo(mapa) aberto, apontando pra ele com o ponteiro
    char linha[COLUNAS + 2];
    int i, j;

    // Abre o arquivo e faz a leitura(r) dele, caso seja nulo, termina a funcao retornando 0
    arq = fopen(nomeArquivo, "r");
    if (arq == NULL)
    {
        return 0;
    }

    // Percorre as linhas do mapa
    for (i = 0; i < LINHAS; i++)
    {
        // Preenche a linha da matriz com espacos
        for (j = 0; j < COLUNAS; j++)
        {
            mapa[i][j] = ' ';
        }

        // Faz a leitura da linha seguinte e guarda em linha[]
        if (fgets(linha, COLUNAS + 2, arq) == NULL)
        {
            // Coloca \0 na primeira posicao pra esvaziar a linha e evitar copiar a antiga
            linha[0] = '\0';
        }

        /* Copia de linha para matriz, e quando chega ao fim do texto(\0) ou na quebra de linha(\n), para.
        O que nao foi copiado continua sendo espaco */
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

    // Fecha e avisa que funcionou
    fclose(arq);
    return 1;
}