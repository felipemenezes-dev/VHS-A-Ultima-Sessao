#include "inimigo.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned char ligacoes[NUM_CENARIOS][CELULAS_INIMIGO];
static bool livres[NUM_CENARIOS][CELULAS_INIMIGO];
static int anteriores[CELULAS_INIMIGO];
static int fila[CELULAS_INIMIGO];
static const int passosColuna[] = {0, 0, 1, -1};
static const int passosLinha[] = {1, -1, 0, 0};

static Vector2 pontoCelula(int celula) {
    return (Vector2){(celula % COLUNAS_INIMIGO + 0.5f) * PASSO_INIMIGO,
                     (celula / COLUNAS_INIMIGO + 0.5f) * PASSO_INIMIGO};
}

static float distancia(Vector2 inicio, Vector2 fim) {
    return hypotf(fim.x - inicio.x, fim.y - inicio.y);
}

static bool trechoLivre(const MapaColisao *mapa, Vector2 inicio, Vector2 fim) {
    int passos = (int)ceilf(distancia(inicio, fim));
    for (int passo = 0; passo <= passos; passo++) {
        float parte = passos ? (float)passo / passos : 0;
        Vector2 ponto = {inicio.x + (fim.x - inicio.x) * parte,
                         inicio.y + (fim.y - inicio.y) * parte};
        if (pesBloqueados(mapa, ponto))
            return false;
    }
    return true;
}

static int celulaProxima(const MapaColisao *mapa, Vector2 ponto) {
    int melhor = -1;
    float menor = INFINITY;
    for (int celula = 0; celula < CELULAS_INIMIGO; celula++) {
        float atual = distancia(ponto, pontoCelula(celula));
        if (livres[mapa->sala][celula] && atual < menor &&
            trechoLivre(mapa, ponto, pontoCelula(celula))) {
            melhor = celula;
            menor = atual;
        }
    }
    return melhor;
}

static int buscarCaminhos(int sala, int inicio) {
    for (int celula = 0; celula < CELULAS_INIMIGO; celula++)
        anteriores[celula] = -1;
    if (inicio < 0)
        return 0;
    int quantidade = 1;
    fila[0] = inicio;
    anteriores[inicio] = inicio;
    for (int indice = 0; indice < quantidade; indice++) {
        int atual = fila[indice];
        for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
            if (!(ligacoes[sala][atual] & (1 << direcao)))
                continue;
            int vizinho = atual + passosColuna[direcao] + passosLinha[direcao] * COLUNAS_INIMIGO;
            if (anteriores[vizinho] < 0) {
                anteriores[vizinho] = atual;
                fila[quantidade++] = vizinho;
            }
        }
    }
    return quantidade;
}

static int proximaPorta(int origem, int destino) {
    int distanciaSala[NUM_CENARIOS];
    int salasFila[NUM_CENARIOS];
    for (int sala = 0; sala < NUM_CENARIOS; sala++)
        distanciaSala[sala] = -1;
    distanciaSala[destino] = 0;
    salasFila[0] = destino;
    int quantidade = 1;
    for (int indice = 0; indice < quantidade; indice++) {
        for (int porta = 0; porta < quantidadePortas; porta++) {
            int anterior = portas[porta].origem;
            if (portas[porta].destino == salasFila[indice] && distanciaSala[anterior] < 0) {
                distanciaSala[anterior] = distanciaSala[salasFila[indice]] + 1;
                salasFila[quantidade++] = anterior;
            }
        }
    }
    for (int porta = 0; porta < quantidadePortas; porta++) {
        if (portas[porta].origem == origem && distanciaSala[origem] > 0 &&
            distanciaSala[portas[porta].destino] == distanciaSala[origem] - 1)
            return porta;
    }
    return -1;
}

static void planejarCaminho(Inimigo *inimigo, const MapaColisao *mapa, Vector2 alvo) {
    int inicio = celulaProxima(mapa, inimigo->posicao);
    int quantidade = buscarCaminhos(inimigo->sala, inicio);
    int destino = -1;
    float menor = INFINITY;
    inimigo->quantidade = 0;
    inimigo->proximo = 0;
    for (int indice = 0; indice < quantidade; indice++) {
        int celula = fila[indice];
        Vector2 ponto = pontoCelula(celula);
        if (inimigo->porta >= 0 && !CheckCollisionPointRec(ponto, portas[inimigo->porta].area))
            continue;
        float atual = distancia(ponto, alvo);
        if (atual < menor) {
            menor = atual;
            destino = celula;
        }
    }
    if (destino < 0)
        return;
    while (destino != inicio) {
        inimigo->caminho[inimigo->quantidade++] = destino;
        destino = anteriores[destino];
    }
    inimigo->caminho[inimigo->quantidade++] = inicio;
    for (int indice = 0; indice < inimigo->quantidade / 2; indice++) {
        int outro = inimigo->quantidade - indice - 1;
        int celula = inimigo->caminho[indice];
        inimigo->caminho[indice] = inimigo->caminho[outro];
        inimigo->caminho[outro] = celula;
    }
}

bool iniciarInimigo(Inimigo *inimigo, const MapaColisao mapas[]) {
    memset(inimigo, 0, sizeof *inimigo);
    memset(ligacoes, 0, sizeof ligacoes);
    const char *direcoes[] = {"baixo", "cima", "direita", "esquerda"};
    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
        for (int quadro = 0; quadro < NUM_QUADROS; quadro++) {
            char caminho[256];
            snprintf(caminho, sizeof caminho, "assets/personagens/rebobinador/%s/reb_%s%d.png",
                     direcoes[direcao], direcoes[direcao], quadro + 1);
            Image imagem = LoadImage(caminho);
            if (!imagem.data)
                return false;
            ImageAlphaCrop(&imagem, 0.05f);
            inimigo->imagens[direcao][quadro] = LoadTextureFromImage(imagem);
            UnloadImage(imagem);
            if (!inimigo->imagens[direcao][quadro].id)
                return false;
            SetTextureFilter(inimigo->imagens[direcao][quadro], TEXTURE_FILTER_POINT);
        }
    }
    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        for (int celula = 0; celula < CELULAS_INIMIGO; celula++)
            livres[sala][celula] = !pesBloqueados(&mapas[sala], pontoCelula(celula));
        for (int celula = 0; celula < CELULAS_INIMIGO; celula++) {
            if (!livres[sala][celula])
                continue;
            for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
                int coluna = celula % COLUNAS_INIMIGO + passosColuna[direcao];
                int linha = celula / COLUNAS_INIMIGO + passosLinha[direcao];
                if (coluna < 0 || coluna >= COLUNAS_INIMIGO || linha < 0 || linha >= LINHAS_INIMIGO)
                    continue;
                int vizinho = linha * COLUNAS_INIMIGO + coluna;
                if (livres[sala][vizinho] && trechoLivre(&mapas[sala], pontoCelula(celula), pontoCelula(vizinho)))
                    ligacoes[sala][celula] |= 1 << direcao;
            }
        }
    }
    return true;
}

void alternarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int sala, Vector2 john) {
    if (inimigo->ativo) {
        inimigo->ativo = false;
        return;
    }
    int inicio = celulaProxima(&mapas[sala], john);
    int quantidade = buscarCaminhos(sala, inicio);
    for (int indice = 0; indice < quantidade; indice++) {
        Vector2 ponto = pontoCelula(fila[indice]);
        if (distancia(ponto, john) < 300)
            continue;
        inimigo->ativo = true;
        inimigo->sala = sala;
        inimigo->posicao = ponto;
        inimigo->direcao = BAIXO;
        inimigo->quadro = 0;
        inimigo->salaAlvo = -1;
        inimigo->quantidade = 0;
        inimigo->proximo = 0;
        inimigo->tempoAnimacao = 0;
        inimigo->tempoParado = 0;
        inimigo->esperaTroca = 0;
        inimigo->espera = 2;
        return;
    }
    TraceLog(LOG_WARNING, "Sem ponto livre distante para ativar o Rebobinador");
}

void atualizarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john, float tempo) {
    if (!inimigo->ativo || tempo <= 0)
        return;
    tempo = fminf(tempo, 0.25f);
    inimigo->esperaTroca = fmaxf(0, inimigo->esperaTroca - tempo);
    if (inimigo->espera > 0) {
        inimigo->espera -= tempo;
        return;
    }
    const MapaColisao *mapa = &mapas[inimigo->sala];
    inimigo->porta = proximaPorta(inimigo->sala, salaJohn);
    Vector2 alvo = john;
    if (inimigo->porta >= 0) {
        Rectangle area = portas[inimigo->porta].area;
        alvo = (Vector2){area.x + area.width / 2, area.y + area.height / 2};
    }
    int celulaAlvo = (int)(alvo.y / PASSO_INIMIGO) * COLUNAS_INIMIGO + (int)(alvo.x / PASSO_INIMIGO);
    if (inimigo->salaAlvo != salaJohn || inimigo->celulaAlvo != celulaAlvo || inimigo->tempoParado > 0.4f) {
        planejarCaminho(inimigo, mapa, alvo);
        inimigo->salaAlvo = salaJohn;
        inimigo->celulaAlvo = celulaAlvo;
        inimigo->tempoParado = 0;
    }
    Vector2 destino = inimigo->posicao;
    if (inimigo->porta < 0 && trechoLivre(mapa, inimigo->posicao, john)) {
        destino = john;
    } else if (inimigo->proximo < inimigo->quantidade) {
        while (inimigo->proximo + 1 < inimigo->quantidade &&
               trechoLivre(mapa, inimigo->posicao, pontoCelula(inimigo->caminho[inimigo->proximo + 1])))
            inimigo->proximo++;
        destino = pontoCelula(inimigo->caminho[inimigo->proximo]);
    }
    Vector2 anterior = inimigo->posicao;
    float restante = distancia(anterior, destino);
    if (restante > 0.1f) {
        float passo = fminf(110 * tempo, restante);
        Vector2 movimento = {(destino.x - anterior.x) / restante, (destino.y - anterior.y) / restante};
        inimigo->direcao = fabsf(movimento.x) > fabsf(movimento.y)
            ? (movimento.x < 0 ? ESQUERDA : DIREITA) : (movimento.y < 0 ? CIMA : BAIXO);
        inimigo->posicao = moverComColisao(mapa, anterior, (Vector2){movimento.x * passo, movimento.y * passo});
    }
    if (distancia(inimigo->posicao, destino) < 0.1f && inimigo->proximo < inimigo->quantidade)
        inimigo->proximo++;
    if (distancia(anterior, inimigo->posicao) > 0.01f) {
        inimigo->tempoParado = 0;
        inimigo->tempoAnimacao += tempo;
        inimigo->quadro = (int)(inimigo->tempoAnimacao / 0.14f) % NUM_QUADROS;
    } else {
        inimigo->tempoParado += tempo;
        inimigo->tempoAnimacao = 0;
        inimigo->quadro = 0;
    }
    if (inimigo->porta >= 0 && trocarSala(&inimigo->sala, &inimigo->posicao,
            &inimigo->direcao, portas[inimigo->porta].direcao, &inimigo->esperaTroca)) {
        inimigo->salaAlvo = -1;
        inimigo->quantidade = 0;
    }
}

Texture2D imagemInimigo(const Inimigo *inimigo) {
    return inimigo->imagens[inimigo->direcao][inimigo->quadro];
}

void liberarInimigo(Inimigo *inimigo) {
    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++)
        for (int quadro = 0; quadro < NUM_QUADROS; quadro++)
            if (inimigo->imagens[direcao][quadro].id)
                UnloadTexture(inimigo->imagens[direcao][quadro]);
    memset(inimigo, 0, sizeof *inimigo);
}
