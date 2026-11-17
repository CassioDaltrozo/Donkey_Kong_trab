#include "placar.h"
#include <string.h>

void inicializa_placar(TIPO_PLACAR tipo_placar[], int *qtd)
{
    *qtd = 0;

    for (int i = 0; i < MAX_PLACAR; i++)
    {
        strcpy(tipo_placar[i].nome, "---");
        tipo_placar[i].time = 0;
        tipo_placar[i].tempo = 0.0;
        tipo_placar[i].pontuacao = 0.0;
    }
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