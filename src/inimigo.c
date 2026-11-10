#include "inimigo.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define DURACAO_QUADRO_ATAQUE 0.11f
#define MOMENTO_IMPACTO_ATAQUE (DURACAO_QUADRO_ATAQUE * 2.0f)
#define DURACAO_ATAQUE (DURACAO_QUADRO_ATAQUE * QUADROS_ATAQUE)
#define ALCANCE_INICIO_ATAQUE 75.0f
#define ALCANCE_IMPACTO_ATAQUE 90.0f

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
    if (destino < 0 || inicio < 0)
        return;
    while (destino != inicio && inimigo->quantidade < CELULAS_INIMIGO - 1) {
        inimigo->caminho[inimigo->quantidade++] = destino;
        int proximoDestino = anteriores[destino];
        if (proximoDestino < 0)
            return;
        destino = proximoDestino;
    }
    inimigo->caminho[inimigo->quantidade++] = inicio;
    for (int indice = 0; indice < inimigo->quantidade / 2; indice++) {
        int outro = inimigo->quantidade - indice - 1;
        int celula = inimigo->caminho[indice];
        inimigo->caminho[indice] = inimigo->caminho[outro];
        inimigo->caminho[outro] = celula;
    }
}

static bool carregarAtaquesDirecao(Inimigo *inimigo, int direcao, const char *nomeDirecao) {
    for (int quadro = 0; quadro < QUADROS_ATAQUE; quadro++) {
        char caminho[320];
        snprintf(caminho, sizeof caminho,
                 "assets/personagens/rebobinador/Ataques/%s/reb_ataque_%s%d.png",
                 nomeDirecao, nomeDirecao, quadro + 1);

        inimigo->imagensAtaque[direcao][quadro] = LoadTexture(caminho);
        if (!inimigo->imagensAtaque[direcao][quadro].id) {
            TraceLog(LOG_ERROR, "Nao foi possivel carregar o sprite de ataque: %s", caminho);
            return false;
        }
        SetTextureFilter(inimigo->imagensAtaque[direcao][quadro], TEXTURE_FILTER_POINT);
    }
    return true;
}

bool iniciarInimigo(Inimigo *inimigo, const MapaColisao mapas[]) {
    if (!inimigo || !mapas)
        return false;
    memset(inimigo, 0, sizeof *inimigo);
    memset(ligacoes, 0, sizeof ligacoes);
    if (IsAudioDeviceReady()) {
        inimigo->somPasso = LoadSound("assets/sons/passo_inimigo.wav");
        if (!IsSoundValid(inimigo->somPasso)) {
            liberarInimigo(inimigo);
            return false;
        }
    }
    const char *direcoes[] = {"baixo", "cima", "direita", "esquerda"};
    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
        for (int quadro = 0; quadro < NUM_QUADROS; quadro++) {
            char caminho[256];
            snprintf(caminho, sizeof caminho, "assets/personagens/rebobinador/%s/reb_%s%d.png",
                     direcoes[direcao], direcoes[direcao], quadro + 1);
            Image imagem = LoadImage(caminho);
            if (!imagem.data) {
                TraceLog(LOG_ERROR, "Nao foi possivel carregar o sprite: %s", caminho);
                liberarInimigo(inimigo);
                return false;
            }
            ImageAlphaCrop(&imagem, 0.05f);
            inimigo->imagens[direcao][quadro] = LoadTextureFromImage(imagem);
            UnloadImage(imagem);
            if (!inimigo->imagens[direcao][quadro].id) {
                liberarInimigo(inimigo);
                return false;
            }
            SetTextureFilter(inimigo->imagens[direcao][quadro], TEXTURE_FILTER_POINT);
        }
        if (!carregarAtaquesDirecao(inimigo, direcao, direcoes[direcao])) {
            liberarInimigo(inimigo);
            return false;
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
    if (!inimigo || !mapas || sala < 0 || sala >= NUM_CENARIOS)
        return;

    if (inimigo->ativo) {
        inimigo->ativo = false;
        inimigo->atacando = false;
        inimigo->caminhando = false;
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
        inimigo->distanciaPasso = 0;
        inimigo->caminhando = false;
        inimigo->atacando = false;
        inimigo->danoAplicado = false;
        inimigo->ataqueAcertou = false;
        inimigo->quadroAtaque = 0;
        inimigo->tempoAtaque = 0.0f;
        inimigo->direcaoAtaque = BAIXO;
        inimigo->espera = 2;
        inimigo->esperaAtaque = 0;
        inimigo->acertos = 0;
        inimigo->atingidoFlash = false;
        inimigo->contatoAnterior = false;
        inimigo->atordoamento = 0;
        inimigo->porta = -1;
        return;
    }
    TraceLog(LOG_WARNING, "Sem ponto livre distante para ativar o Rebobinador");
}

Vector2 moverComPersonagem(const MapaColisao *mapa, Vector2 posicao, Vector2 deslocamento,
    bool bloquear, Vector2 outro) {
    if (!bloquear)
        return moverComColisao(mapa, posicao, deslocamento);

    int passos = (int)ceilf(fmaxf(fabsf(deslocamento.x), fabsf(deslocamento.y)));
    if (passos == 0)
        return posicao;
    deslocamento.x /= passos;
    deslocamento.y /= passos;
    for (int passo = 0; passo < passos; passo++) {
        Vector2 seguinte = moverComColisao(mapa, posicao, (Vector2){deslocamento.x, 0});
        if (!CheckCollisionRecs(caixaPes(seguinte), caixaPes(outro)) ||
            (CheckCollisionRecs(caixaPes(posicao), caixaPes(outro)) &&
             distancia(seguinte, outro) > distancia(posicao, outro)))
            posicao = seguinte;
        seguinte = moverComColisao(mapa, posicao, (Vector2){0, deslocamento.y});
        if (!CheckCollisionRecs(caixaPes(seguinte), caixaPes(outro)) ||
            (CheckCollisionRecs(caixaPes(posicao), caixaPes(outro)) &&
             distancia(seguinte, outro) > distancia(posicao, outro)))
            posicao = seguinte;
    }
    return posicao;
}

bool usarFlash(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john) {
    if (!inimigo || !mapas || salaJohn < 0 || salaJohn >= NUM_CENARIOS ||
        !inimigo->ativo || inimigo->sala != salaJohn || inimigo->atordoamento > 0 ||
        distancia(inimigo->posicao, john) > 140 ||
        !trechoLivre(&mapas[salaJohn], inimigo->posicao, john))
        return false;

    inimigo->atacando = false;
    inimigo->danoAplicado = false;
    inimigo->ataqueAcertou = false;
    inimigo->quadroAtaque = 0;
    inimigo->tempoAtaque = 0.0f;
    inimigo->atordoamento = 10.0f;
    inimigo->atingidoFlash = true;
    inimigo->quadro = 0;
    inimigo->tempoAnimacao = 0;
    return true;
}

void atualizarInimigo(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn, Vector2 john,
    int quantidadeFitas, float tempo) {
    if (!inimigo || !mapas)
        return;
    inimigo->caminhando = false;
    if (!inimigo->ativo || tempo <= 0)
        return;
    tempo = fminf(tempo, 0.25f);
    if (inimigo->atacando)
        return;
    if (inimigo->atordoamento > 0) {
        inimigo->atordoamento = fmaxf(0, inimigo->atordoamento - tempo);
        return;
    }
    inimigo->esperaTroca = fmaxf(0, inimigo->esperaTroca - tempo);
    if (inimigo->espera > 0) {
        inimigo->espera -= tempo;
        return;
    }
    if (inimigo->sala < 0 || inimigo->sala >= NUM_CENARIOS ||
        salaJohn < 0 || salaJohn >= NUM_CENARIOS)
        return;
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
        float velocidade = 110.0f;
        if (inimigo->atingidoFlash)
            velocidade *= 1.5f;
        else if (inimigo->acertos >= 2)
            velocidade *= 1.75f;
        else if (quantidadeFitas > 1)
            velocidade *= 1.65f;
        float passo = fminf(velocidade * tempo, restante);
        Vector2 movimento = {(destino.x - anterior.x) / restante, (destino.y - anterior.y) / restante};
        inimigo->direcao = fabsf(movimento.x) > fabsf(movimento.y)
            ? (movimento.x < 0 ? ESQUERDA : DIREITA) : (movimento.y < 0 ? CIMA : BAIXO);
        inimigo->posicao = moverComPersonagem(mapa, anterior,
            (Vector2){movimento.x * passo, movimento.y * passo}, inimigo->sala == salaJohn, john);
    }
    if (distancia(inimigo->posicao, destino) < 0.1f && inimigo->proximo < inimigo->quantidade)
        inimigo->proximo++;
    float deslocamento = distancia(anterior, inimigo->posicao);
    inimigo->caminhando = deslocamento > 0.01f;
    if (inimigo->caminhando) {
        inimigo->distanciaPasso += deslocamento;
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

static int direcaoParaJohn(Vector2 origem, Vector2 john) {
    float dx = john.x - origem.x;
    float dy = john.y - origem.y;
    if (fabsf(dx) > fabsf(dy))
        return dx < 0.0f ? ESQUERDA : DIREITA;
    return dy < 0.0f ? CIMA : BAIXO;
}

static void aplicarDano(Inimigo *inimigo, int quantidadeFitas, int *vida) {
    int dano = quantidadeFitas > 1 ? 40 : 20;
    *vida -= dano;
    if (*vida < 0)
        *vida = 0;
    inimigo->acertos++;
    inimigo->ataqueAcertou = true;
}

static void finalizarAtaque(Inimigo *inimigo, const MapaColisao mapas[],
                            Vector2 john, int salaJohn) {
    inimigo->atacando = false;
    inimigo->quadroAtaque = 0;
    inimigo->tempoAtaque = 0.0f;
    inimigo->esperaAtaque = 1.0f;
    inimigo->tempoAnimacao = 0.0f;
    inimigo->quadro = 0;

    if (inimigo->ataqueAcertou) {
        Vector2 recuo = {
            inimigo->posicao.x - john.x,
            inimigo->posicao.y - john.y
        };
        float comprimento = hypotf(recuo.x, recuo.y);

        if (comprimento < 0.01f) {
            recuo = (Vector2){
                -passosColuna[inimigo->direcaoAtaque],
                -passosLinha[inimigo->direcaoAtaque]
            };
            comprimento = 1.0f;
        }

        recuo.x *= 50.0f / comprimento;
        recuo.y *= 50.0f / comprimento;
        inimigo->posicao = moverComPersonagem(
            &mapas[salaJohn], inimigo->posicao, recuo, true, john);
        inimigo->espera = 2.0f;
    } else {
        inimigo->espera = 0.5f;
        inimigo->contatoAnterior = false;
    }

    inimigo->ataqueAcertou = false;
    inimigo->danoAplicado = false;
    inimigo->salaAlvo = -1;
}

void atacarJohn(Inimigo *inimigo, const MapaColisao mapas[], int salaJohn,
                Vector2 john, int quantidadeFitas, float tempo, int *vida) {
    if (!inimigo || !mapas || !vida || !inimigo->ativo || *vida <= 0 || tempo <= 0.0f)
        return;

    tempo = fminf(tempo, 0.25f);

    if (inimigo->atacando) {
        inimigo->tempoAtaque += tempo;

        int quadro = (int)(inimigo->tempoAtaque / DURACAO_QUADRO_ATAQUE);
        if (quadro >= QUADROS_ATAQUE)
            quadro = QUADROS_ATAQUE - 1;
        inimigo->quadroAtaque = quadro;

        if (!inimigo->danoAplicado &&
            inimigo->tempoAtaque >= MOMENTO_IMPACTO_ATAQUE) {
            inimigo->danoAplicado = true;

            bool aindaNoAlcance =
                inimigo->sala == salaJohn &&
                salaJohn >= 0 && salaJohn < NUM_CENARIOS &&
                distancia(inimigo->posicao, john) <= ALCANCE_IMPACTO_ATAQUE &&
                trechoLivre(&mapas[salaJohn], inimigo->posicao, john);

            if (aindaNoAlcance)
                aplicarDano(inimigo, quantidadeFitas, vida);
        }

        if (inimigo->tempoAtaque >= DURACAO_ATAQUE)
            finalizarAtaque(inimigo, mapas, john, salaJohn);

        return;
    }

    inimigo->esperaAtaque = fmaxf(0.0f, inimigo->esperaAtaque - tempo);

    if (salaJohn < 0 || salaJohn >= NUM_CENARIOS ||
        inimigo->sala != salaJohn) {
        inimigo->contatoAnterior = false;
        return;
    }

    bool contato = distancia(inimigo->posicao, john) <= ALCANCE_INICIO_ATAQUE;

    if (!contato) {
        inimigo->contatoAnterior = false;
        return;
    }

    if (inimigo->contatoAnterior || inimigo->espera > 0.0f ||
        inimigo->atordoamento > 0.0f || inimigo->esperaAtaque > 0.0f ||
        !trechoLivre(&mapas[salaJohn], inimigo->posicao, john))
        return;

    inimigo->contatoAnterior = true;
    inimigo->atacando = true;
    inimigo->caminhando = false;
    inimigo->direcaoAtaque = direcaoParaJohn(inimigo->posicao, john);
    inimigo->direcao = inimigo->direcaoAtaque;
    inimigo->quadroAtaque = 0;
    inimigo->tempoAtaque = 0.0f;
    inimigo->danoAplicado = false;
    inimigo->ataqueAcertou = false;
}

void atualizarSomInimigo(Inimigo *inimigo, int salaJohn, Vector2 john, bool pausado) {
    if (!IsSoundValid(inimigo->somPasso))
        return;
    if (pausado || !inimigo->ativo || !inimigo->caminhando || inimigo->atacando ||
        inimigo->sala != salaJohn || inimigo->espera > 0 || inimigo->atordoamento > 0) {
        StopSound(inimigo->somPasso);
        inimigo->distanciaPasso = 0;
        return;
    }

    float volume = fmaxf(0, 1.0f - distancia(inimigo->posicao, john) / 600.0f);
    volume = fminf(1.0f, volume * 1.8f);
    SetSoundVolume(inimigo->somPasso, volume);
    if (inimigo->distanciaPasso >= 60.0f) {
        inimigo->distanciaPasso = fmodf(inimigo->distanciaPasso, 60.0f);
        if (volume > 0)
            PlaySound(inimigo->somPasso);
    }
}

Texture2D imagemInimigo(const Inimigo *inimigo) {
    if (!inimigo)
        return (Texture2D){0};

    if (inimigo->atacando) {
        Texture2D ataque =
            inimigo->imagensAtaque[inimigo->direcaoAtaque][inimigo->quadroAtaque];
        if (ataque.id)
            return ataque;
    }

    return inimigo->imagens[inimigo->direcao][inimigo->quadro];
}

void liberarInimigo(Inimigo *inimigo) {
    if (!inimigo)
        return;

    if (IsSoundValid(inimigo->somPasso))
        UnloadSound(inimigo->somPasso);

    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
        for (int quadro = 0; quadro < NUM_QUADROS; quadro++) {
            if (inimigo->imagens[direcao][quadro].id)
                UnloadTexture(inimigo->imagens[direcao][quadro]);
        }
        for (int quadro = 0; quadro < QUADROS_ATAQUE; quadro++) {
            if (inimigo->imagensAtaque[direcao][quadro].id)
                UnloadTexture(inimigo->imagensAtaque[direcao][quadro]);
        }
    }

    memset(inimigo, 0, sizeof *inimigo);
}
