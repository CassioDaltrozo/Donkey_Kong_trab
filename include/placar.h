#ifndef PLACAR_H
#define PLACAR_H

#define MAX_PLACAR 10
// Ate 15 caracteres + 1 para o '\0' (fim da string)
#define MAX_NOME 20
#define ARQUIVO_PLACAR "placar.bin"
#define PONTUACAO_INICIAL 5000.0f

typedef struct tipo_placar
{
    char nome[20];
    int time;
    double tempo;
    double pontuacao;
} TIPO_PLACAR;

// Preenche o vetor de placar com valores iniciais vazios e zera a quantidade
void inicializa_placar(TIPO_PLACAR tipo_placar[], int *qtd);
double bonus_tempo(float tempo_fase);

#endif
