
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define NUM_DIRECOES 4
#define NUM_FRAMES 4

enum Direcao {
    BAIXO = 0,
    CIMA,
    DIREITA,
    ESQUERDA
};

int main(void) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    int monitor = GetCurrentMonitor();

    InitWindow(
        GetMonitorWidth(monitor),
        GetMonitorHeight(monitor),
        "VHS - A Ultima Sessao"
    );

    SetTargetFPS(60);

    Texture2D fundo = LoadTexture(
        "assets/cenarios/salao_principal.png"
    );

    const char *pastas[NUM_DIRECOES] = {
        "baixo",
        "cima",
        "direita",
        "esquerda"
    };

    const char *nomes[NUM_DIRECOES] = {
        "baixo",
        "cima",
        "direita",
        "esquerda"
    };

    Texture2D animacoes[NUM_DIRECOES][NUM_FRAMES] = {0};
    Texture2D johnParado[NUM_DIRECOES] = {0};

    bool erro = fundo.id == 0;

    SetTextureFilter(fundo, TEXTURE_FILTER_POINT);

    for (int d = 0; d < NUM_DIRECOES; d++) {
        char caminho[256];

        snprintf(
            caminho,
            sizeof(caminho),
            "assets/personagens/john/%s/john_parado_%s.png",
            pastas[d],
            nomes[d]
        );

        johnParado[d] = LoadTexture(caminho);

        if (johnParado[d].id == 0) {
            TraceLog(LOG_ERROR, "Nao foi possivel carregar: %s", caminho);
            erro = true;
        } else {
            SetTextureFilter(johnParado[d], TEXTURE_FILTER_POINT);
        }

        for (int f = 0; f < NUM_FRAMES; f++) {
            snprintf(
                caminho,
                sizeof(caminho),
                "assets/personagens/john/%s/john_andando_%s%d.png",
                pastas[d],
                nomes[d],
                f + 1
            );

            animacoes[d][f] = LoadTexture(caminho);

            if (animacoes[d][f].id == 0) {
                TraceLog(LOG_ERROR, "Nao foi possivel carregar: %s", caminho);
                erro = true;
            } else {
                SetTextureFilter(
                    animacoes[d][f],
                    TEXTURE_FILTER_POINT
                );
            }
        }
    }

    if (erro) {
        for (int d = 0; d < NUM_DIRECOES; d++) {
            if (johnParado[d].id != 0) {
                UnloadTexture(johnParado[d]);
            }

            for (int f = 0; f < NUM_FRAMES; f++) {
                if (animacoes[d][f].id != 0) {
                    UnloadTexture(animacoes[d][f]);
                }
            }
        }

        if (fundo.id != 0) {
            UnloadTexture(fundo);
        }

        CloseWindow();
        return 1;
    }

    float larguraTela = (float)GetScreenWidth();
    float alturaTela = (float)GetScreenHeight();

    Vector2 john = {
        larguraTela * 0.50f,
        alturaTela * 0.82f
    };

    float velocidade = alturaTela * 0.35f;

    int direcao = BAIXO;
    int frameAtual = 0;

    float tempoAnimacao = 0.0f;
    float duracaoFrame = 0.11f;

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        larguraTela = (float)GetScreenWidth();
        alturaTela = (float)GetScreenHeight();

        float alturaJohn = alturaTela * 0.15f;

        float movimentoX = 0.0f;
        float movimentoY = 0.0f;

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            movimentoX -= 1.0f;
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            movimentoX += 1.0f;
        }

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            movimentoY -= 1.0f;
        }

        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            movimentoY += 1.0f;
        }

        bool andando =
            movimentoX != 0.0f || movimentoY != 0.0f;

        if (andando) {
            if (fabsf(movimentoY) > fabsf(movimentoX)) {
                direcao = movimentoY < 0.0f ? CIMA : BAIXO;
            } else {
                direcao = movimentoX > 0.0f ? DIREITA : ESQUERDA;
            }

            float comprimento = sqrtf(
                movimentoX * movimentoX +
                movimentoY * movimentoY
            );

            movimentoX /= comprimento;
            movimentoY /= comprimento;

            john.x += movimentoX * velocidade * delta;
            john.y += movimentoY * velocidade * delta;

            tempoAnimacao += delta;

            while (tempoAnimacao >= duracaoFrame) {
                tempoAnimacao -= duracaoFrame;
                frameAtual = (frameAtual + 1) % NUM_FRAMES;
            }
        } else {
            frameAtual = 0;
            tempoAnimacao = 0.0f;
        }

        Texture2D atual;

        if (andando) {
            atual = animacoes[direcao][frameAtual];
        } else {
            atual = johnParado[direcao];
        }

        float proporcao =
            (float)atual.width / (float)atual.height;

        float larguraJohn = alturaJohn * proporcao;

        if (john.x < larguraJohn / 2.0f) {
            john.x = larguraJohn / 2.0f;
        }

        if (john.x > larguraTela - larguraJohn / 2.0f) {
            john.x = larguraTela - larguraJohn / 2.0f;
        }

        if (john.y < alturaJohn) {
            john.y = alturaJohn;
        }

        if (john.y > alturaTela - 10.0f) {
            john.y = alturaTela - 10.0f;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexturePro(
            fundo,
            (Rectangle) {
                0.0f,
                0.0f,
                (float)fundo.width,
                (float)fundo.height
            },
            (Rectangle) {
                0.0f,
                0.0f,
                larguraTela,
                alturaTela
            },
            (Vector2) {0.0f, 0.0f},
            0.0f,
            WHITE
        );

        DrawTexturePro(
            atual,
            (Rectangle) {
                0.0f,
                0.0f,
                (float)atual.width,
                (float)atual.height
            },
            (Rectangle) {
                john.x - larguraJohn / 2.0f,
                john.y - alturaJohn,
                larguraJohn,
                alturaJohn
            },
            (Vector2) {0.0f, 0.0f},
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    for (int d = 0; d < NUM_DIRECOES; d++) {
        for (int f = 0; f < NUM_FRAMES; f++) {
            UnloadTexture(animacoes[d][f]);
        }

        UnloadTexture(johnParado[d]);
    }

    UnloadTexture(fundo);

    CloseWindow();

    return 0;
}
