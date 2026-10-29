#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define NUM_DIRECOES 4
#define NUM_FRAMES 4
#define NUM_CENARIOS 5

enum Direcao {
    BAIXO = 0,
    CIMA,
    DIREITA,
    ESQUERDA
};

enum Cenario {
    SALAO = 0,
    BANHEIRO,
    ESCRITORIO,
    ESTOQUE,
    SALA_PROJECAO
};

int main(int argc, char **argv) {
    bool janela = argc > 1 && TextIsEqual(argv[1], "--windowed");
    SetConfigFlags(janela ? FLAG_WINDOW_RESIZABLE : FLAG_FULLSCREEN_MODE);

    // Zero selects the monitor resolution after the graphics system initializes.
    InitWindow(janela ? 1280 : 0, janela ? 720 : 0, "VHS - A Ultima Sessao");
    if (!IsWindowReady()) return 1;

    SetTargetFPS(60);

    const char *caminhosCenarios[NUM_CENARIOS] = {
        "assets/cenarios/salao_principal.png",
        "assets/cenarios/banheiro.png",
        "assets/cenarios/escritorio.png",
        "assets/cenarios/estoque.png",
        "assets/cenarios/sala_projecao.png"
    };

    Texture2D cenarios[NUM_CENARIOS] = {0};

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

    bool erro = false;

    for (int i = 0; i < NUM_CENARIOS; i++) {
        cenarios[i] = LoadTexture(caminhosCenarios[i]);

        if (cenarios[i].id == 0) {
            TraceLog(
                LOG_ERROR,
                "Nao foi possivel carregar: %s",
                caminhosCenarios[i]
            );
            erro = true;
        } else {
            SetTextureFilter(cenarios[i], TEXTURE_FILTER_POINT);
        }
    }

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
                SetTextureFilter(animacoes[d][f], TEXTURE_FILTER_POINT);
            }
        }
    }

    if (erro) {
        for (int i = 0; i < NUM_CENARIOS; i++) {
            if (cenarios[i].id != 0) {
                UnloadTexture(cenarios[i]);
            }
        }

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

        CloseWindow();
        return 1;
    }

    float larguraTela = (float)GetScreenWidth();
    float alturaTela = (float)GetScreenHeight();

    Vector2 john = {
        larguraTela * 0.50f,
        alturaTela * 0.62f
    };

    float velocidade = alturaTela * 0.35f;

    int cenarioAtual = SALAO;
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

        bool andando = movimentoX != 0.0f || movimentoY != 0.0f;

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

        bool mudouCenario = false;

        if (cenarioAtual == SALAO) {
            Rectangle portaBanheiro = {
                0.0f,
                alturaTela * 0.35f,
                larguraTela * 0.055f,
                alturaTela * 0.29f
            };

            Rectangle portaEscritorio = {
                larguraTela * 0.255f,
                alturaTela * 0.045f,
                larguraTela * 0.095f,
                alturaTela * 0.16f
            };

            Rectangle portaEstoque = {
                larguraTela * 0.655f,
                alturaTela * 0.045f,
                larguraTela * 0.095f,
                alturaTela * 0.16f
            };

            if (movimentoX < 0.0f && CheckCollisionPointRec(john, portaBanheiro)) {
                cenarioAtual = BANHEIRO;

                john.x = larguraTela * 0.855f;
                john.y = alturaTela * 0.68f;

                direcao = ESQUERDA;
                frameAtual = 0;
                tempoAnimacao = 0.0f;

                mudouCenario = true;
            } else if (CheckCollisionPointRec(john, portaEscritorio)) {
                cenarioAtual = ESCRITORIO;

                john.x = larguraTela * 0.50f;
                john.y = alturaTela * 0.73f;

                direcao = CIMA;
                frameAtual = 0;
                tempoAnimacao = 0.0f;

                mudouCenario = true;
            } else if (CheckCollisionPointRec(john, portaEstoque)) {
                cenarioAtual = ESTOQUE;

                john.x = larguraTela * 0.50f;
                john.y = alturaTela * 0.73f;

                direcao = CIMA;
                frameAtual = 0;
                tempoAnimacao = 0.0f;

                mudouCenario = true;
            }
        } else if (cenarioAtual == BANHEIRO) {
            Rectangle saidaBanheiro = {
                larguraTela * 0.89f,
                alturaTela * 0.50f,
                larguraTela * 0.11f,
                alturaTela * 0.25f
            };

            if (movimentoX > 0.0f && CheckCollisionPointRec(john, saidaBanheiro)) {
                cenarioAtual = SALAO;

                john.x = larguraTela * 0.06f;
                john.y = alturaTela * 0.53f;

                direcao = DIREITA;
                frameAtual = 0;
                tempoAnimacao = 0.0f;

                mudouCenario = true;
            }
        } else {
            if (cenarioAtual == ESTOQUE) {
                Rectangle portaProjecao = {
                    larguraTela * 0.44f,
                    alturaTela * 0.03f,
                    larguraTela * 0.12f,
                    alturaTela * 0.21f
                };

                if (movimentoY < 0.0f && CheckCollisionPointRec(john, portaProjecao)) {
                    cenarioAtual = SALA_PROJECAO;

                    john.x = larguraTela * 0.50f;
                    john.y = alturaTela * 0.73f;

                    direcao = CIMA;
                    frameAtual = 0;
                    tempoAnimacao = 0.0f;

                    mudouCenario = true;
                }
            }

            Rectangle saidaComodo = {
                larguraTela * 0.38f,
                alturaTela * 0.86f,
                larguraTela * 0.24f,
                alturaTela * 0.13f
            };

            if (CheckCollisionPointRec(john, saidaComodo)) {
                int comodoAnterior = cenarioAtual;

                if (comodoAnterior == SALA_PROJECAO) {
                    cenarioAtual = ESTOQUE;

                    john.x = larguraTela * 0.50f;
                    john.y = alturaTela * 0.23f;
                } else {
                    cenarioAtual = SALAO;

                    if (comodoAnterior == ESCRITORIO) {
                        john.x = larguraTela * 0.30f;
                        john.y = alturaTela * 0.23f;
                    } else if (comodoAnterior == ESTOQUE) {
                        john.x = larguraTela * 0.70f;
                        john.y = alturaTela * 0.23f;
                    }
                }

                direcao = BAIXO;
                frameAtual = 0;
                tempoAnimacao = 0.0f;

                mudouCenario = true;
            }
        }

        Texture2D atual;

        if (andando && !mudouCenario) {
            atual = animacoes[direcao][frameAtual];
        } else {
            atual = johnParado[direcao];
        }

        float proporcao = (float)atual.width / (float)atual.height;
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
            cenarios[cenarioAtual],
            (Rectangle) {
                0.0f,
                0.0f,
                (float)cenarios[cenarioAtual].width,
                (float)cenarios[cenarioAtual].height
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

    for (int i = 0; i < NUM_CENARIOS; i++) {
        UnloadTexture(cenarios[i]);
    }

    for (int d = 0; d < NUM_DIRECOES; d++) {
        for (int f = 0; f < NUM_FRAMES; f++) {
            UnloadTexture(animacoes[d][f]);
        }

        UnloadTexture(johnParado[d]);
    }

    CloseWindow();
    return 0;
}