#ifndef RANKING_H
#define RANKING_H

#define MAX_RANKING 10
// Ate 15 caracteres + 1 para o '\0' (fim da string)
#define MAX_NICK 16 
#define ARQUIVO_RANKING "data/ranking.bin"

typedef struct RegistroRanking
{
    char nickname[MAX_NICK];
    int pontuacao;
} RegistroRanking;

// Preenche o vetor de ranking com valores iniciais vazios e zera a quantidade
void inicializa_ranking(RegistroRanking ranking[], int *qtd);

#endif
