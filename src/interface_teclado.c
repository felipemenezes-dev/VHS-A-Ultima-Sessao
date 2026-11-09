#include "interface_teclado.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>


static void definirMensagem(TecladoSaida *teclado, const char *texto, float duracao) {
    if (!teclado || !texto)
        return;


    snprintf(teclado->mensagem, sizeof teclado->mensagem, "%s", texto);
    teclado->tempoMensagem = duracao;
}


void iniciarTecladoSaida(TecladoSaida *teclado) {
    if (!teclado)
        return;


    *teclado = (TecladoSaida){0};
}


void abrirTecladoSaida(TecladoSaida *teclado, const char *codigo) {
    if (!teclado)
        return;


    teclado->aberto = true;
    teclado->entrada[0] = '\0';
    teclado->mensagem[0] = '\0';


    teclado->transmissaoCompleta = codigo && strlen(codigo) == 3;
    if (teclado->transmissaoCompleta)
        snprintf(teclado->codigoEsperado, sizeof teclado->codigoEsperado, "%s", codigo);
    else
        teclado->codigoEsperado[0] = '\0';
}


void atualizarTecladoSaida(TecladoSaida *teclado, float delta) {
    if (!teclado || !teclado->aberto)
        return;


    if (delta < 0.0f)
        delta = 0.0f;
    if (delta > 0.25f)
        delta = 0.25f;


    teclado->tempoMensagem -= delta;
    if (teclado->tempoMensagem <= 0.0f) {
        teclado->tempoMensagem = 0.0f;
        teclado->mensagem[0] = '\0';
    }


    if (IsKeyPressed(KEY_ESCAPE)) {
        teclado->aberto = false;
        return;
    }


    if (teclado->saidaLiberada) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E))
            teclado->aberto = false;
        return;
    }


    int caractere;
    while ((caractere = GetCharPressed()) > 0) {
        if (caractere >= '0' && caractere <= '9') {
            size_t tamanho = strlen(teclado->entrada);


            if (tamanho < 3) {
                teclado->entrada[tamanho] = (char)caractere;
                teclado->entrada[tamanho + 1] = '\0';
            }
        }
    }


    if (IsKeyPressed(KEY_BACKSPACE)) {
        size_t tamanho = strlen(teclado->entrada);
        if (tamanho > 0)
            teclado->entrada[tamanho - 1] = '\0';
    }


    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (!teclado->transmissaoCompleta) {
            teclado->entrada[0] = '\0';
            definirMensagem(
                teclado,
                "TRANSMISSAO INCOMPLETA - RETORNE AO APARELHO VHS.",
                3.5f
            );
            return;
        }


        if (strlen(teclado->entrada) != 3) {
            definirMensagem(teclado, "DIGITE OS TRES DIGITOS.", 2.5f);
            return;
        }


        if (strcmp(teclado->entrada, teclado->codigoEsperado) == 0) {
            teclado->saidaLiberada = true;
            definirMensagem(teclado, "SAIDA LIBERADA. A FECHADURA FOI DESTRAVADA.", 4.0f);
        } else {
            teclado->entrada[0] = '\0';
            definirMensagem(teclado, "SEQUENCIA INCORRETA. TENTE NOVAMENTE.", 3.0f);
        }
    }
}


void desenharTecladoSaida(const TecladoSaida *teclado) {
    if (!teclado || !teclado->aberto)
        return;


    DrawRectangle(0, 0, 1672, 941, (Color){0, 0, 0, 210});
    DrawRectangle(490, 170, 692, 610, (Color){8, 10, 16, 250});
    DrawRectangleLinesEx(
        (Rectangle){490, 170, 692, 610},
        3.0f,
        teclado->saidaLiberada ? GREEN : (Color){160, 43, 53, 255}
    );


    DrawText("TECLADO DA SAIDA", 570, 220, 34, RAYWHITE);
    DrawText(
        teclado->saidaLiberada ? "FECHADURA DESTRAVADA" : "DIGITE O CODIGO DE DESBLOQUEIO",
        570,
        280,
        20,
        teclado->saidaLiberada ? GREEN : LIGHTGRAY
    );


    DrawRectangle(610, 355, 450, 112, (Color){2, 4, 8, 255});
    DrawRectangleLinesEx(
        (Rectangle){610, 355, 450, 112},
        3.0f,
        teclado->saidaLiberada ? GREEN : GOLD
    );


    for (int i = 0; i < 3; i++) {
        int x = 705 + i * 110;
        DrawRectangle(x, 375, 78, 72, (Color){20, 24, 31, 255});
        DrawRectangleLinesEx(
            (Rectangle){x, 375, 78, 72},
            2.0f,
            teclado->saidaLiberada ? GREEN : (Color){117, 80, 70, 255}
        );


        if (teclado->entrada[i] != '\0')
            DrawText(TextFormat("%c", teclado->entrada[i]), x + 24, 385, 48, RAYWHITE);
        else
            DrawText("_", x + 24, 385, 48, GRAY);
    }


    if (teclado->mensagem[0])
        DrawText(teclado->mensagem, 570, 525, 20, teclado->saidaLiberada ? GREEN : GOLD);
    else if (teclado->saidaLiberada)
        DrawText("PRESSIONE ENTER PARA FECHAR.", 570, 525, 20, GREEN);
    else if (!teclado->transmissaoCompleta)
        DrawText("A TRANSMISSAO AINDA NAO FOI RECONSTRUIDA.", 570, 525, 18, GOLD);
    else
        DrawText("NUMEROS: DIGITAR  |  BACKSPACE: APAGAR  |  ENTER: CONFIRMAR", 570, 525, 17, LIGHTGRAY);


    DrawText("ESC: CANCELAR", 570, 700, 18, RAYWHITE);
}


bool tecladoSaidaEstaAberto(const TecladoSaida *teclado) {
    return teclado && teclado->aberto;
}


bool tecladoSaidaEstaLiberado(const TecladoSaida *teclado) {
    return teclado && teclado->saidaLiberada;
}
