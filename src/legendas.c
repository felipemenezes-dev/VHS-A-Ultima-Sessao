#include "raylib.h"
#include "legendas.h"
#include <string.h>

static char textoLegenda[512] = "";
static char textoVisivel[512] = "";
static float tempoRestante = 0.0f;
static bool legendaAtiva = false;

void iniciarLegendas(void) {
    textoLegenda[0] = '\0';
    textoVisivel[0] = '\0';
    tempoRestante = 0.0f;
    legendaAtiva = false;
}

void mostrarLegenda(const char *texto, float duracao) {
    if (!texto)
        return;

    strncpy(textoLegenda, texto, sizeof(textoLegenda) - 1);
    textoLegenda[sizeof(textoLegenda) - 1] = '\0';
    strncpy(textoVisivel, textoLegenda, sizeof(textoVisivel) - 1);
    textoVisivel[sizeof(textoVisivel) - 1] = '\0';

    tempoRestante = duracao;
    legendaAtiva = true;
}

void atualizarLegendas(float delta) {
    if (!legendaAtiva || delta <= 0.0f)
        return;

    tempoRestante -= delta;

    if (tempoRestante <= 0.0f) {
        legendaAtiva = false;
        textoLegenda[0] = '\0';
        textoVisivel[0] = '\0';
    }
}

void desenharLegendas(void) {
    if (!legendaAtiva)
        return;

    int larguraTela = GetScreenWidth();
    int alturaTela = GetScreenHeight();
    int tamanhoFonte = 24;
    int espacamento = 8;
    int larguraCaixa = larguraTela - 100;
    int posX = 50;
    int quantidadeLinhas = 1;

    for (int i = 0; textoVisivel[i] != '\0'; i++) {
        if (textoVisivel[i] == '\n')
            quantidadeLinhas++;
    }

    int alturaTexto = quantidadeLinhas * (tamanhoFonte + espacamento);
    int alturaCaixa = alturaTexto + 30;

    if (alturaCaixa < 80)
        alturaCaixa = 80;

    int posY = alturaTela - alturaCaixa - 40;

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

    int textoY = posY + 15;
    const char *inicioLinha = textoVisivel;

    while (*inicioLinha != '\0') {
        const char *fimLinha = strchr(inicioLinha, '\n');
        int tamanhoLinha = fimLinha != NULL
            ? (int)(fimLinha - inicioLinha)
            : (int)strlen(inicioLinha);

        char linha[512];

        if (tamanhoLinha >= (int)sizeof(linha))
            tamanhoLinha = (int)sizeof(linha) - 1;

        memcpy(linha, inicioLinha, (size_t)tamanhoLinha);
        linha[tamanhoLinha] = '\0';

        int larguraTexto = MeasureText(linha, tamanhoFonte);
        int textoX = (larguraTela - larguraTexto) / 2;

        DrawText(
            linha,
            textoX,
            textoY,
            tamanhoFonte,
            WHITE
        );

        textoY += tamanhoFonte + espacamento;

        if (fimLinha == NULL)
            break;

        inicioLinha = fimLinha + 1;
    }
}

void liberarLegendas(void) {
    textoLegenda[0] = '\0';
    textoVisivel[0] = '\0';
    tempoRestante = 0.0f;
    legendaAtiva = false;
}
