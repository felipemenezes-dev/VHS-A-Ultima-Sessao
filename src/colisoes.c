#include "colisoes.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

static bool prepararMapaColisao(MapaColisao *mapa, const char *caminho, int sala) {
    memset(mapa, 0, sizeof *mapa);
    mapa->sala = sala;
    Image imagemCarregada = LoadImage(caminho);
    if (!imagemCarregada.data)
        return false;
    if (imagemCarregada.width != LARGURA_MUNDO || imagemCarregada.height != ALTURA_MUNDO) {
        UnloadImage(imagemCarregada);
        return false;
    }
    Color *p = LoadImageColors(imagemCarregada);
    if (!p) {
        UnloadImage(imagemCarregada);
        return false;
    }
    mapa->largura = imagemCarregada.width;
    mapa->altura = imagemCarregada.height;
    mapa->somas = calloc((size_t)(mapa->largura + 1) * (mapa->altura + 1), sizeof *mapa->somas);
    if (!mapa->somas) {
        UnloadImageColors(p);
        UnloadImage(imagemCarregada);
        return false;
    }

    for (int y = 0; y < mapa->altura; y++) {
        uint32_t linha = 0;
        for (int x = 0; x < mapa->largura; x++) {

            bool exterior = sala >= 0 && (!pontoDentroChao(sala, (Vector2){x + .001f, y + .001f}) ||
                                          !pontoDentroChao(sala, (Vector2){x + .999f, y + .001f}) ||
                                          !pontoDentroChao(sala, (Vector2){x + .001f, y + .999f}) ||
                                          !pontoDentroChao(sala, (Vector2){x + .999f, y + .999f}));
            linha += p[y * mapa->largura + x].a != 0 || exterior;
            mapa->somas[(y + 1) * (mapa->largura + 1) + x + 1] =
                linha + mapa->somas[y * (mapa->largura + 1) + x + 1];
        }
    }
    UnloadImageColors(p);
    UnloadImage(imagemCarregada);
    mapa->pronto = true;
    return true;
}

bool carregarMapaColisao(MapaColisao *mapa, const char *caminho) {
    return prepararMapaColisao(mapa, caminho, -1);
}

bool carregarColisoesSala(MapaColisao *mapa, int sala) {
    if (sala < 0 || sala >= NUM_CENARIOS)
        return false;
    return prepararMapaColisao(mapa, salas[sala].mascara, sala);
}

static bool cruzaLimitePorta(const MapaColisao *mapa, Rectangle r) {
    Vector2 cantos[4] = {
        {r.x, r.y}, {r.x + r.width, r.y}, {r.x + r.width, r.y + r.height}, {r.x, r.y + r.height}};
    for (int i = 0; i < quantidadePortas; i++) {
        const Porta *d = &portas[i];
        if (d->origem != mapa->sala || (d->limiteA.x == 0 && d->limiteB.x == 0))
            continue;
        if (CheckCollisionPointRec(d->limiteA, r) || CheckCollisionPointRec(d->limiteB, r))
            return true;
        for (int j = 0; j < 4; j++)
            if (CheckCollisionLines(d->limiteA, d->limiteB, cantos[j], cantos[(j + 1) % 4], NULL))
                return true;
    }
    return false;
}

void liberarColisoes(MapaColisao *mapa) {
    free(mapa->somas);
    memset(mapa, 0, sizeof *mapa);
}

bool retanguloBloqueado(const MapaColisao *mapa, Rectangle r) {
    if (!isfinite(r.x) || !isfinite(r.y) || !isfinite(r.width) || !isfinite(r.height) ||
        r.width <= 0 || r.height <= 0)
        return true;
    if (r.x < 0 || r.y < 0 || r.x + r.width > LARGURA_MUNDO || r.y + r.height > ALTURA_MUNDO)
        return true;
    int x0 = (int)floorf(r.x), y0 = (int)floorf(r.y);
    int x1 = (int)ceilf(r.x + r.width), y1 = (int)ceilf(r.y + r.height);
    if (x0 < 0 || y0 < 0 || x1 > LARGURA_MUNDO || y1 > ALTURA_MUNDO)
        return true;
    if (!mapa->pronto)
        return true;

    if (cruzaLimitePorta(mapa, r))
        return true;
    int s = mapa->largura + 1;
    return mapa->somas[y1 * s + x1] - mapa->somas[y0 * s + x1] - mapa->somas[y1 * s + x0] +
               mapa->somas[y0 * s + x0] >
           0;
}

bool pesBloqueados(const MapaColisao *mapa, Vector2 p) {
    return retanguloBloqueado(mapa, caixaPes(p));
}

Vector2 moverComColisao(const MapaColisao *mapa, Vector2 p, Vector2 d) {
    if (!isfinite(d.x) || !isfinite(d.y) || pesBloqueados(mapa, p))
        return p;

    float distancia = fmaxf(fabsf(d.x), fabsf(d.y));
    if (distancia > 100000)
        return p;
    int passos = (int)ceilf(distancia);
    if (passos < 1)
        return p;
    d.x /= passos;
    d.y /= passos;
    for (int i = 0; i < passos; i++) {
        Vector2 seguinte = {p.x + d.x, p.y};
        if (!pesBloqueados(mapa, seguinte))
            p = seguinte;
        seguinte = (Vector2){p.x, p.y + d.y};
        if (!pesBloqueados(mapa, seguinte))
            p = seguinte;
    }
    return p;
}
