#ifndef VHS_INIMIGO_H
#define VHS_INIMIGO_H
#include "colisoes.h"

#define PASSO_INIMIGO 12
#define COLUNAS_INIMIGO (LARGURA_MUNDO / PASSO_INIMIGO)
#define LINHAS_INIMIGO (ALTURA_MUNDO / PASSO_INIMIGO)
#define CELULAS_INIMIGO (COLUNAS_INIMIGO * LINHAS_INIMIGO)

typedef struct {
    bool ativo;
    int sala, direcao, quadro;
    Vector2 posicao;
    int caminho[CELULAS_INIMIGO];
    int quantidade, proximo, salaAlvo, celulaAlvo, porta;
    float espera, tempoAnimacao, tempoParado, esperaTroca;
    Texture2D imagens[NUM_DIRECOES][NUM_QUADROS];
} Inimigo;

bool iniciarInimigo(Inimigo *inimigo, const MapaColisao mapas[]);
void alternarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int sala, Vector2 john);
void atualizarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john, float tempo);
Texture2D imagemInimigo(const Inimigo *inimigo);
void liberarInimigo(Inimigo *inimigo);
#endif
