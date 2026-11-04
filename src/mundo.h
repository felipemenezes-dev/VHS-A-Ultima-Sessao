#ifndef VHS_MUNDO_H
#define VHS_MUNDO_H
#include "raylib.h"

#define LARGURA_MUNDO 1672
#define ALTURA_MUNDO 941
#define NUM_CENARIOS 5
#define NUM_DIRECOES 4
#define NUM_QUADROS 4
#define ALTURA_JOHN (ALTURA_MUNDO * 0.15f)
#define VELOCIDADE_JOHN (ALTURA_MUNDO * 0.35f)
#define LARGURA_PES (ALTURA_JOHN * 0.24f)
#define ALTURA_PES (ALTURA_JOHN * 0.12f)

enum Direcao { BAIXO = 0, CIMA, DIREITA, ESQUERDA };
enum Cenario { SALAO = 0, BANHEIRO, ESCRITORIO, ESTOQUE, SALA_PROJECAO };

typedef struct {
    const char *nome, *imagem, *mascara;
    Vector2 entrada;
} Sala;

typedef struct {
    int origem, destino;
    Rectangle area;
    Vector2 chegada;
    int direcaoOlhar;
    const char *rotulo;
    Vector2 limiteA, limiteB;
    Vector2 direcao;
} Porta;

typedef struct {
    const Vector2 *pontos;
    int quantidade;
} LimiteChao;
extern const LimiteChao limitesChao[NUM_CENARIOS];
bool pontoDentroChao(int sala, Vector2 ponto);

extern const Sala salas[NUM_CENARIOS];
extern const Porta portas[];
extern const int quantidadePortas;
Rectangle caixaPes(Vector2 posicao);
Rectangle areaExibicao(int largura, int altura);
Vector2 telaParaMundo(Vector2 ponto, Rectangle exibicao);
Vector2 mundoParaTela(Vector2 ponto, Rectangle exibicao);
int encontrarPorta(int sala, Vector2 posicao, Vector2 movimento);
bool trocarSala(int *sala, Vector2 *posicao, int *direcaoOlhar, Vector2 movimento,
                float *esperaTroca);
#endif
