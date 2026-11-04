#include "mundo.h"
#include <math.h>


#define SALA(n, f, x, y)                                                                           \
    {                                                                                              \
        n, "assets/cenarios/" f ".png", "assets/cenarios/colisoes_" f ".png", {                    \
            x, y                                                                                   \
        }                                                                                          \
    }
const Sala salas[NUM_CENARIOS] = {{"Salão principal",
                                   "assets/cenarios/salao_principal.png",
                                   "assets/cenarios/colisoes_salao.png",
                                   {836, 583}},
                                  SALA("Banheiro", "banheiro", 1320, 620),
                                  SALA("Escritório", "escritorio", 835, 704),
                                  SALA("Estoque", "estoque", 845, 727),
                                  SALA("Sala de projeção", "sala_projecao", 825, 704)};


const Porta portas[] = {{SALAO,
                         BANHEIRO,
                         {74, 463, 90, 98},
                         {1320, 620},
                         ESQUERDA,
                         "Banheiro",
                         {110, 460},
                         {61, 566},
                         {-1, 0}},
                        {BANHEIRO,
                         SALAO,
                         {1360, 590, 70, 110},
                         {185, 520},
                         DIREITA,
                         "Salão principal",
                         {1411, 579},
                         {1457, 751},
                         {1, 0}},
                        {SALAO,
                         ESCRITORIO,
                         {450, 202, 102, 60},
                         {835, 704},
                         CIMA,
                         "Escritório",
                         {447, 197},
                         {568, 197},
                         {0, -1}},
                        {ESCRITORIO,
                         SALAO,
                         {775, 756, 126, 140},
                         {500, 290},
                         BAIXO,
                         "Salão principal",
                         {0, 0},
                         {0, 0},
                         {0, 1}},
                        {SALAO,
                         ESTOQUE,
                         {1125, 202, 105, 60},
                         {845, 727},
                         CIMA,
                         "Estoque",
                         {1117, 198},
                         {1229, 198},
                         {0, -1}},
                        {ESTOQUE,
                         SALAO,
                         {765, 784, 165, 130},
                         {1174, 293},
                         BAIXO,
                         "Salão principal",
                         {0, 0},
                         {0, 0},
                         {0, 1}},
                        {ESTOQUE,
                         SALA_PROJECAO,
                         {778, 202, 132, 67},
                         {825, 704},
                         CIMA,
                         "Sala de projeção",
                         {0, 0},
                         {0, 0},
                         {0, -1}},
                        {SALA_PROJECAO,
                         ESTOQUE,
                         {773, 765, 100, 113},
                         {845, 305},
                         BAIXO,
                         "Estoque",
                         {0, 0},
                         {0, 0},
                         {0, 1}}};
const int quantidadePortas = sizeof portas / sizeof *portas;


static const Vector2 chaoSalao[] = {{235, 196}, {1390, 196}, {1550, 350}, {1630, 735}, {1530, 835},
                                    {978, 835}, {978, 728},  {700, 728},  {700, 835},  {132, 835},
                                    {50, 740},  {50, 570},   {110, 460},  {170, 300}};
static const Vector2 chaoBanheiro[] = {
    {230, 330},  {1345, 330}, {1360, 550}, {1410, 580}, {1450, 750}, {1450, 800}, {1165, 800},
    {1165, 845}, {1010, 845}, {1010, 800}, {150, 800},  {185, 560},  {230, 560}};
static const Vector2 chaoEscritorio[] = {{208, 245}, {1458, 245}, {1475, 759}, {938, 759},
                                         {938, 740}, {905, 740},  {905, 886},  {772, 886},
                                         {772, 740}, {738, 740},  {738, 759},  {208, 759}};
static const Vector2 chaoEstoque[] = {
    {150, 155},  {240, 155},  {240, 225},  {774, 225},  {774, 200},  {912, 200},
    {912, 225},  {1435, 225}, {1435, 155}, {1520, 155}, {1540, 707}, {1460, 707},
    {1460, 780}, {973, 780},  {973, 765},  {935, 765},  {935, 896},  {760, 896},
    {760, 765},  {724, 765},  {724, 780},  {205, 780},  {205, 708},  {118, 708}};
static const Vector2 chaoProjecao[] = {{335, 240}, {1330, 240}, {1380, 770}, {925, 770},
                                       {925, 746}, {904, 746},  {904, 868},  {754, 868},
                                       {754, 746}, {730, 746},  {730, 770},  {265, 770}};
#define CHAO(pontos) {pontos, (int)(sizeof pontos / sizeof *pontos)}
const LimiteChao limitesChao[NUM_CENARIOS] = {CHAO(chaoSalao), CHAO(chaoBanheiro),
                                              CHAO(chaoEscritorio), CHAO(chaoEstoque),
                                              CHAO(chaoProjecao)};
#undef CHAO


bool pontoDentroChao(int sala, Vector2 p) {
    if (sala < 0 || sala >= NUM_CENARIOS || !isfinite(p.x) || !isfinite(p.y))
        return false;
    const LimiteChao *chao = &limitesChao[sala];
    bool pontoDentroPoligono = false;
    for (int i = 0, j = chao->quantidade - 1; i < chao->quantidade; j = i++) {
        Vector2 a = chao->pontos[i], b = chao->pontos[j];
        if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)
            pontoDentroPoligono = !pontoDentroPoligono;
    }
    return pontoDentroPoligono;
}


Rectangle caixaPes(Vector2 p) {
    return (Rectangle){p.x - LARGURA_PES / 2, p.y - ALTURA_PES, LARGURA_PES, ALTURA_PES};
}


Rectangle areaExibicao(int largura, int altura) {
    float escala = fminf((float)largura / LARGURA_MUNDO, (float)altura / ALTURA_MUNDO);
    return (Rectangle){(largura - LARGURA_MUNDO * escala) / 2, (altura - ALTURA_MUNDO * escala) / 2,
                       LARGURA_MUNDO * escala, ALTURA_MUNDO * escala};
}


Vector2 telaParaMundo(Vector2 p, Rectangle v) {
    return (Vector2){(p.x - v.x) * LARGURA_MUNDO / v.width, (p.y - v.y) * ALTURA_MUNDO / v.height};
}


Vector2 mundoParaTela(Vector2 p, Rectangle v) {
    return (Vector2){v.x + p.x * v.width / LARGURA_MUNDO, v.y + p.y * v.height / ALTURA_MUNDO};
}


int encontrarPorta(int sala, Vector2 p, Vector2 movimento) {
    for (int i = 0; i < quantidadePortas; i++) {
        const Porta *d = &portas[i];
        if (d->origem == sala && CheckCollisionPointRec(p, d->area) &&
            movimento.x * d->direcao.x + movimento.y * d->direcao.y > 0)
            return i;
    }
    return -1;
}


bool trocarSala(int *sala, Vector2 *p, int *direcaoOlhar, Vector2 movimento, float *esperaTroca) {
    if (*esperaTroca > 0)
        return false;
    int i = encontrarPorta(*sala, *p, movimento);
    if (i < 0)
        return false;
    *sala = portas[i].destino;
    *p = portas[i].chegada;
    *direcaoOlhar = portas[i].direcaoOlhar;
    *esperaTroca = .25f;
    return true;
}
