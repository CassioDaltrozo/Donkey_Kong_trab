#ifndef PLACAR_H
#define PLACAR_H

#define MAX_PLACAR 10
// Ate 19 caracteres + 1 para o '\0' (fim da string)
#define MAX_NOME 20
#define ARQUIVO_PLACAR "data/placar.bin"
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
int entra_placar(TIPO_PLACAR placar[], int qtd, double pontuacao);
void ordena_placar(TIPO_PLACAR placar[], int qtd);
void inserir_no_placar(TIPO_PLACAR placar[], int *qtd, const char nome[], double tempo, double pontuacao);
double bonus_tempo(float tempo_fase);
void salvar_placar(TIPO_PLACAR placar[]);
void carrega_placar(TIPO_PLACAR placar[], int *qtd);

#endif