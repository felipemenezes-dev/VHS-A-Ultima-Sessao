
#include "legendas.h"
#include <string.h>

static char textoLegenda[256] = "";
static float tempoRestante = 0.0f;
static bool legendaAtiva = false;

void iniciarLegendas(void) {
    textoLegenda[0] = '\0';
    tempoRestante = 0.0f;
    legendaAtiva = false;
}

void mostrarLegenda(const char *texto, float duracao) {
    if (texto == NULL) {
        return;
    }

    strncpy(textoLegenda, texto, sizeof(textoLegenda) - 1);
    textoLegenda[sizeof(textoLegenda) - 1] = '\0';

    tempoRestante = duracao;
    legendaAtiva = true;
}

void atualizarLegendas(float delta) {
    if (!legendaAtiva) {
        return;
    }

    tempoRestante -= delta;

    if (tempoRestante <= 0.0f) {
        legendaAtiva = false;
        textoLegenda[0] = '\0';
    }
}

void desenharLegendas(void) {
    if (!legendaAtiva) {
        return;
    }

    int larguraTela = GetScreenWidth();
    int alturaTela = GetScreenHeight();

    int larguraCaixa = larguraTela - 100;
    int alturaCaixa = 80;

    int posX = 50;
    int posY = alturaTela - 120;

    DrawRectangle(
        posX,
        posY,
        larguraCaixa,
        alturaCaixa,
        Fade(BLACK, 0.80f)
    );

    DrawRectangleLines(
        posX,
        posY,
        larguraCaixa,
        alturaCaixa,
        Fade(WHITE, 0.50f)
    );

    int tamanhoFonte = 24;

    int larguraTexto = MeasureText(textoLegenda, tamanhoFonte);

    int textoX = (larguraTela - larguraTexto) / 2;
    int textoY = posY + (alturaCaixa - tamanhoFonte) / 2;

    DrawText(
        textoLegenda,
        textoX,
        textoY,
        tamanhoFonte,
        WHITE
    );
}

void liberarLegendas(void) {
    textoLegenda[0] = '\0';
    tempoRestante = 0.0f;
    legendaAtiva = false;
}
