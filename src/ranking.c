#include "ranking.h"
#include <string.h>

void inicializa_ranking(RegistroRanking ranking[], int *qtd)
{
    *qtd = 0;

    for (int i = 0; i < MAX_RANKING; i++)
    {
        strcpy(ranking[i].nickname, "---");
        ranking[i].pontuacao = 0;
    }
}
