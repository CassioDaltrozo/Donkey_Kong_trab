#include "placar.h"
#include <string.h>
#include <stdio.h>

void inicializa_placar(TIPO_PLACAR placar[], int *qtd)
{
    *qtd = 0;

    for (int i = 0; i < MAX_PLACAR; i++)
    {
        strcpy(placar[i].nome, "---");
        placar[i].time = 0;
        placar[i].tempo = 0.0;
        placar[i].pontuacao = 0.0;
    }
}

int entra_placar(TIPO_PLACAR placar[], int qtd, double pontuacao)
{
    if (qtd < MAX_PLACAR)
    {
        return 1;
    }
    else if (pontuacao > placar[MAX_PLACAR - 1].pontuacao)
    {
        return 1;
    }

    return 0;
}

void ordena_placar(TIPO_PLACAR placar[], int qtd)
{
    for (int i = 0; i < qtd - 1; i++)
    {
        for (int j = i + 1; j < qtd; j++)
        {
            if (placar[j].pontuacao > placar[i].pontuacao || (placar[i].pontuacao == placar[j].pontuacao && placar[j].tempo < placar[i].tempo))
            {
                TIPO_PLACAR aux = placar[i];
                placar[i] = placar[j];
                placar[j] = aux;
            }
        }
    }
}

void inserir_no_placar(TIPO_PLACAR placar[], int *qtd, const char nome[], double tempo, double pontuacao)
{
    int pos;

    if (!entra_placar(placar, *qtd, pontuacao))
    {
        return;
    }
    else if (*qtd < MAX_PLACAR)
    {
        pos = *qtd;
        (*qtd)++;
    }
    else if (*qtd == MAX_PLACAR)
    {
        pos = MAX_PLACAR - 1;
    }

    strncpy(placar[pos].nome, nome, MAX_NOME - 1);
    placar[pos].nome[MAX_NOME - 1] = '\0';
    placar[pos].time = 0;
    placar[pos].tempo = tempo;
    placar[pos].pontuacao = pontuacao;

    ordena_placar(placar, *qtd);
}

double bonus_tempo(float tempo_fase)
{
    if (tempo_fase < 30.0f)
        return 1000.0;
    else if (tempo_fase < 45.0f)
        return 500.0;
    else if (tempo_fase < 60.0f)
        return 250.0;
    else
        return 100.0;
}

void salvar_placar(TIPO_PLACAR placar[])
{
    FILE *arq = fopen(ARQUIVO_PLACAR, "wb");

    if (arq == NULL)
    {
        return;
    }
    else
    {
        fwrite(placar, sizeof(TIPO_PLACAR), MAX_PLACAR, arq);
        fclose(arq);
    }
}

void carrega_placar(TIPO_PLACAR placar[], int *qtd)
{
    unsigned int i;

    inicializa_placar(placar, qtd);

    FILE *arq = fopen(ARQUIVO_PLACAR, "rb");

    if (arq == NULL)
    {
        salvar_placar(placar);
        return;
    }
    else
    {
        unsigned int lidos = fread(placar, sizeof(TIPO_PLACAR), MAX_PLACAR, arq);
        fclose(arq);

        for (i = 0; i < lidos; i++)
        {
            if (placar[i].nome[0] != '\0' && strcmp(placar[i].nome, "---") != 0)
            {
                (*qtd)++;
            }
        }
        ordena_placar(placar, *qtd);
    }
}