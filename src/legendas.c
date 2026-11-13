
#include "raylib.h"
#include "legendas.h"
#include <string.h>

static char textoLegenda[512] = "";
static char textoVisivel[512] = "";
static float tempoRestante = 0.0f;
static float tempoDigitacao = 0.0f;
static size_t bytesVisiveis = 0;
static bool legendaAtiva = false;
static Sound sonsDigitacao[3] = {0};
static int proximoSomDigitacao = 0;

void iniciarLegendas(void) {
    if (IsAudioDeviceReady()) {
        for (int indice = 0; indice < 3; indice++) {
            if (!IsSoundValid(sonsDigitacao[indice])) {
                sonsDigitacao[indice] = LoadSound(
                    TextFormat("assets/sons/digitacao_%d.ogg", indice + 1)
                );
            }

            if (IsSoundValid(sonsDigitacao[indice]))
                SetSoundVolume(sonsDigitacao[indice], 0.32f);
        }
    }

    textoLegenda[0] = '\0';
    textoVisivel[0] = '\0';
    tempoRestante = 0.0f;
    tempoDigitacao = 0.0f;
    bytesVisiveis = 0;
    legendaAtiva = false;
}

void tocarSomDigitacao(void) {
    for (int tentativa = 0; tentativa < 3; tentativa++) {
        Sound som = sonsDigitacao[proximoSomDigitacao];
        proximoSomDigitacao = (proximoSomDigitacao + 1) % 3;

        if (IsSoundValid(som)) {
            PlaySound(som);
            return;
        }
    }
}

void mostrarLegenda(const char *texto, float duracao) {
    if (texto == NULL) {
        return;
    }

    strncpy(textoLegenda, texto, sizeof(textoLegenda) - 1);
    textoLegenda[sizeof(textoLegenda) - 1] = '\0';
    textoVisivel[0] = '\0';

    tempoRestante = duracao;
    tempoDigitacao = 0.0f;
    bytesVisiveis = 0;
    legendaAtiva = true;
}

void atualizarLegendas(float delta) {
    if (!legendaAtiva) {
        return;
    }

    if (bytesVisiveis < strlen(textoLegenda)) {
        tempoDigitacao += delta;

        while (tempoDigitacao >= 0.035f &&
               bytesVisiveis < strlen(textoLegenda)) {
            unsigned char caractere =
                (unsigned char)textoLegenda[bytesVisiveis];
            size_t tamanhoCaractere = 1;

            if ((caractere & 0xE0) == 0xC0)
                tamanhoCaractere = 2;
            else if ((caractere & 0xE0) == 0xE0)
                tamanhoCaractere = 3;
            else if ((caractere & 0xF0) == 0xF0)
                tamanhoCaractere = 4;

            bytesVisiveis += tamanhoCaractere;
            tempoDigitacao -= 0.035f;
            tocarSomDigitacao();
        }

        memcpy(textoVisivel, textoLegenda, bytesVisiveis);
        textoVisivel[bytesVisiveis] = '\0';
    } else {
        tempoRestante -= delta;
    }

    if (tempoRestante <= 0.0f) {
        legendaAtiva = false;
        textoLegenda[0] = '\0';
        textoVisivel[0] = '\0';
    }
}

void desenharLegendas(void) {
    if (!legendaAtiva) {
        return;
    }

    int larguraTela = GetScreenWidth();
    int alturaTela = GetScreenHeight();

    int tamanhoFonte = 24;
    int espacamento = 8;

    int larguraCaixa = larguraTela - 100;
    int posX = 50;

    int quantidadeLinhas = 1;

    for (int i = 0; textoVisivel[i] != '\0'; i++) {
        if (textoVisivel[i] == '\n') {
            quantidadeLinhas++;
        }
    }

    int alturaTexto = quantidadeLinhas * (tamanhoFonte + espacamento);
    int alturaCaixa = alturaTexto + 30;

    if (alturaCaixa < 80) {
        alturaCaixa = 80;
    }

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

        int tamanhoLinha;

        if (fimLinha != NULL) {
            tamanhoLinha = (int)(fimLinha - inicioLinha);
        } else {
            tamanhoLinha = (int)strlen(inicioLinha);
        }

        char linha[512];

        if (tamanhoLinha >= (int)sizeof(linha)) {
            tamanhoLinha = sizeof(linha) - 1;
        }

        memcpy(linha, inicioLinha, tamanhoLinha);
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

        if (fimLinha == NULL) {
            break;
        }

        inicioLinha = fimLinha + 1;
    }
}

void liberarLegendas(void) {
    for (int indice = 0; indice < 3; indice++) {
        if (IsSoundValid(sonsDigitacao[indice]))
            UnloadSound(sonsDigitacao[indice]);
    }

    textoLegenda[0] = '\0';
    textoVisivel[0] = '\0';
    tempoRestante = 0.0f;
    tempoDigitacao = 0.0f;
    bytesVisiveis = 0;
    legendaAtiva = false;
}
