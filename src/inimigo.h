#ifndef VHS_INIMIGO_H
#define VHS_INIMIGO_H
#include "colisoes.h"

#define PASSO_INIMIGO 12
#define COLUNAS_INIMIGO (LARGURA_MUNDO / PASSO_INIMIGO)
#define LINHAS_INIMIGO (ALTURA_MUNDO / PASSO_INIMIGO)
#define CELULAS_INIMIGO (COLUNAS_INIMIGO * LINHAS_INIMIGO)
#define QUADROS_ATAQUE 4

typedef struct {
    bool ativo;
    bool caminhando;
    bool atacando;
    bool danoAplicado;
    bool ataqueAcertou;
    Sound somPasso;
    Sound somAtaque;
    float distanciaPasso;
    bool contatoAnterior, atingidoFlash;
    int acertos;
    float atordoamento;
    int sala, direcao, quadro;
    Vector2 posicao;
    int caminho[CELULAS_INIMIGO];
    int quantidade, proximo, salaAlvo, celulaAlvo, porta;
    float espera, tempoAnimacao, tempoParado, esperaTroca, esperaAtaque;
    int direcaoAtaque, quadroAtaque;
    float tempoAtaque;
    Texture2D imagens[NUM_DIRECOES][NUM_QUADROS];
    Texture2D imagensAtaque[NUM_DIRECOES][QUADROS_ATAQUE];
} Inimigo;

bool iniciarInimigo(Inimigo *inimigo, const MapaColisao mapas[]);
void alternarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int sala, Vector2 john);
void atualizarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john,
    int quantidadeFitas, float tempo);
Vector2 moverComPersonagem(const MapaColisao *mapa, Vector2 posicao, Vector2 deslocamento,
    bool bloquear, Vector2 outro);
bool usarFlash(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john);
void atacarJohn(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john,
    int quantidadeFitas, float tempo, int *vida);
void atualizarSomInimigo(Inimigo *inimigo, int salaJohn, Vector2 john, bool pausado);
Texture2D imagemInimigo(const Inimigo *inimigo);
void liberarInimigo(Inimigo *inimigo);
#endif
