
#include "raylib.h"
#include "mundo.h"
#include "colisoes.h"
#include "camadas.h"
#include "interface_inventario.h"
#include "objetos.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "legendas.h"

static Texture2D carregarImagemPersonagem(const char *caminho) {
    Image imagem = LoadImage(caminho);
    Texture2D textura = {0};

    if (imagem.data) {
        ImageAlphaCrop(&imagem, 0.05f);
        textura = LoadTextureFromImage(imagem);
        UnloadImage(imagem);

        if (textura.id)
            SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    }

    return textura;
}

static void localizarRecursos(void) {
    if (FileExists("assets/cenarios/salao_principal.png"))
        return;

    char caminho[4096];
    ssize_t quantidade = readlink(
        "/proc/self/exe",
        caminho,
        sizeof caminho - 1
    );

    if (quantidade <= 0)
        return;

    caminho[quantidade] = '\0';
    char *barra = strrchr(caminho, '/');

    if (!barra)
        return;

    *barra = '\0';

    if (ChangeDirectory(caminho) &&
        !FileExists("assets/cenarios/salao_principal.png")) {
        ChangeDirectory("..");
    }
}

static void desenharDepuracao(
    int sala,
    Vector2 personagem,
    Texture2D mascara
) {
    DrawTexture(
        mascara,
        0,
        0,
        (Color){125, 255, 150, 210}
    );

    DrawRectangleLinesEx(
        caixaPes(personagem),
        2,
        YELLOW
    );

    const LimiteChao *chao = &limitesChao[sala];

    for (int i = 0; i < chao->quantidade; i++) {
        DrawLineEx(
            chao->pontos[i],
            chao->pontos[(i + 1) % chao->quantidade],
            3,
            VIOLET
        );
    }

    for (int i = 0; i < quantidadePortas; i++) {
        const Porta *porta = &portas[i];

        if (porta->origem == sala) {
            DrawRectangleLinesEx(
                porta->area,
                3,
                SKYBLUE
            );

            DrawText(
                TextFormat("Porta %d: %s", i, porta->rotulo),
                (int)porta->area.x,
                (int)porta->area.y - 22,
                20,
                SKYBLUE
            );

            if (porta->limiteA.x != 0) {
                DrawLineEx(
                    porta->limiteA,
                    porta->limiteB,
                    3,
                    ORANGE
                );
            }
        }

        if (porta->destino == sala) {
            DrawCircleLines(
                porta->chegada.x,
                porta->chegada.y,
                18,
                YELLOW
            );

            DrawText(
                TextFormat("Entrada %d", i),
                (int)porta->chegada.x + 22,
                (int)porta->chegada.y - 10,
                19,
                YELLOW
            );
        }
    }

    DrawRectangle(
        20,
        90,
        440,
        116,
        (Color){6, 8, 14, 230}
    );

    DrawText(
        "F1 - MOSTRAR COLISOES",
        34,
        102,
        22,
        YELLOW
    );

    DrawText(
        TextFormat(
            "Pes: %.1f, %.1f | %.1f x %.1f",
            personagem.x,
            personagem.y,
            LARGURA_PES,
            ALTURA_PES
        ),
        34,
        135,
        19,
        RAYWHITE
    );

    DrawText(
        "Roxo: limite do piso",
        34,
        184,
        18,
        VIOLET
    );

    DrawText(
        "Azul: passagem | Amarelo: entrada",
        34,
        162,
        18,
        SKYBLUE
    );
}

static void mostrarLegendaObjeto(int idObjeto) {
    switch (idObjeto) {
        case 1:
            break;

        case 17:
            mostrarLegenda(
                "FITA ENCONTRADA - A CHAMADA\n"
                "Frequencia: 17\n"
                "[GRAVACAO] Quatro pulsos. "
                "Uma voz sussurra: nao olhe para tras.",
                5.0f
            );
            break;

        case 2:
            mostrarLegenda(
                "CHAVE DO ESTOQUE ENCONTRADA.",
                3.0f
            );
            break;

        case 42:
            mostrarLegenda(
                "SINAL DETECTADO.",
                3.0f
            );
            break;

        case 3:
            mostrarLegenda(
                "FUSIVEL ENCONTRADO.",
                3.0f
            );
            break;

        case 89:
            mostrarLegenda(
                "FITA ENCONTRADA - SALA VAZIA\n"
                "Frequencia: 89\n"
                "[GRAVACAO] A ultima sala esta vazia. "
                "A transmissao, nao.",
                5.0f
            );
            break;

        case 4:
            break;

        default:
            break;
    }
}

int main(void) {
    localizarRecursos();

    int monitor = GetCurrentMonitor();
    int larguraTela = GetMonitorWidth(monitor);
    int alturaTela = GetMonitorHeight(monitor);

    SetConfigFlags(
        FLAG_FULLSCREEN_MODE |
        FLAG_VSYNC_HINT
    );

    InitWindow(
        larguraTela,
        alturaTela,
        "VHS - A Ultima Sessao"
    );

    if (!IsWindowReady())
        return 1;

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Texture2D cenarios[NUM_CENARIOS] = {0};
    Texture2D texturasMascaras[NUM_CENARIOS] = {0};

    MapaColisao mascaras[NUM_CENARIOS] = {0};

    Texture2D animacoes[NUM_DIRECOES][NUM_QUADROS] = {0};
    Texture2D personagemParado[NUM_DIRECOES] = {0};

    const char *pastas[NUM_DIRECOES] = {
        "baixo",
        "cima",
        "direita",
        "esquerda"
    };

    bool erro = false;

    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        cenarios[sala] = LoadTexture(
            salas[sala].imagem
        );

        texturasMascaras[sala] = LoadTexture(
            salas[sala].mascara
        );

        if (
            !cenarios[sala].id ||
            cenarios[sala].width != LARGURA_MUNDO ||
            cenarios[sala].height != ALTURA_MUNDO ||
            !texturasMascaras[sala].id ||
            !carregarColisoesSala(&mascaras[sala], sala)
        ) {
            TraceLog(
                LOG_ERROR,
                "Cenario ou mascara invalida: %s",
                salas[sala].nome
            );

            erro = true;
        } else {
            SetTextureFilter(
                cenarios[sala],
                TEXTURE_FILTER_POINT
            );

            SetTextureFilter(
                texturasMascaras[sala],
                TEXTURE_FILTER_POINT
            );
        }
    }

    for (int direcao = 0; direcao < NUM_DIRECOES; direcao++) {
        char caminho[256];

        snprintf(
            caminho,
            sizeof caminho,
            "assets/personagens/john/%s/"
            "john_parado_%s.png",
            pastas[direcao],
            pastas[direcao]
        );

        personagemParado[direcao] =
            carregarImagemPersonagem(caminho);

        if (!personagemParado[direcao].id)
            erro = true;

        for (int quadro = 0; quadro < NUM_QUADROS; quadro++) {
            snprintf(
                caminho,
                sizeof caminho,
                "assets/personagens/john/%s/"
                "john_andando_%s%d.png",
                pastas[direcao],
                pastas[direcao],
                quadro + 1
            );

            animacoes[direcao][quadro] =
                carregarImagemPersonagem(caminho);

            if (!animacoes[direcao][quadro].id)
                erro = true;
        }
    }

    for (int sala = 0;
         sala < NUM_CENARIOS && !erro;
         sala++) {

        if (pesBloqueados(
            &mascaras[sala],
            salas[sala].entrada
        )) {
            TraceLog(
                LOG_ERROR,
                "Entrada bloqueada: %s",
                salas[sala].nome
            );

            erro = true;
        }
    }

    for (int i = 0;
         i < quantidadePortas && !erro;
         i++) {

        if (pesBloqueados(
            &mascaras[portas[i].destino],
            portas[i].chegada
        )) {
            TraceLog(
                LOG_ERROR,
                "Destino da porta %d bloqueado",
                i
            );

            erro = true;
        }
    }

    if (!erro && !iniciarCamadas()) {
        TraceLog(
            LOG_ERROR,
            "Nao foi possivel preparar as camadas visuais"
        );

        erro = true;
    }

    TelaInventario inventario = {0};

    if (!erro && !iniciarTelaInventario(&inventario)) {
        TraceLog(
            LOG_ERROR,
            "Nao foi possivel iniciar o inventario"
        );

        erro = true;
    }

    if (!erro && !carregarObjetosMapa()) {
        TraceLog(
            LOG_ERROR,
            "Nao foi possivel carregar os objetos do cenario"
        );

        erro = true;
    }

    if (!erro) {
        iniciarLegendas();

        mostrarLegenda(
            "A ULTIMA SESSAO AINDA NAO TERMINOU.",
            5.0f
        );
    }

    RenderTexture2D quadro = {0};

    if (!erro) {
        quadro = LoadRenderTexture(
            LARGURA_MUNDO,
            ALTURA_MUNDO
        );

        if (!quadro.id) {
            erro = true;
        } else {
            SetTextureFilter(
                quadro.texture,
                TEXTURE_FILTER_BILINEAR
            );
        }
    }

    Vector2 personagem = salas[SALAO].entrada;

    int salaAtual = SALAO;
    int direcao = BAIXO;
    int quadroAnimacao = 0;

    float tempoAnimacao = 0;
    float duracaoQuadro = 0.11f;

    float esperaTransicao = 0;
    float escurecimento = 0;

    bool mostrarColisoes = false;
    bool andando = false;

    while (!erro && !WindowShouldClose()) {
        float deltaLegendas = fminf(
            GetFrameTime(),
            0.25f
        );

        atualizarLegendas(deltaLegendas);

        bool teclaEsc = IsKeyPressed(KEY_ESCAPE);

        if (teclaEsc && !inventario.aberto)
            break;

        bool teclaTab = IsKeyPressed(KEY_TAB);

        bool inventarioAberto = atualizarTelaInventario(
            &inventario,
            teclaTab,
            teclaEsc
        );

        if (inventario.aberto && !teclaTab) {
            int passoX =
                (IsKeyPressed(KEY_RIGHT) ||
                 IsKeyPressed(KEY_D)) -
                (IsKeyPressed(KEY_LEFT) ||
                 IsKeyPressed(KEY_A));

            int passoY =
                (IsKeyPressed(KEY_DOWN) ||
                 IsKeyPressed(KEY_S)) -
                (IsKeyPressed(KEY_UP) ||
                 IsKeyPressed(KEY_W));

            Vector2 cursor = telaParaMundo(
                GetMousePosition(),
                areaExibicao(
                    GetScreenWidth(),
                    GetScreenHeight()
                )
            );

            selecionarEspacoInventario(
                &inventario,
                passoX,
                passoY,
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
                cursor,
                LARGURA_MUNDO,
                ALTURA_MUNDO
            );
        }

        bool mudouSala = false;

        if (!inventarioAberto) {
            float tempoQuadro = fminf(
                GetFrameTime(),
                0.25f
            );

            esperaTransicao = fmaxf(
                0,
                esperaTransicao - tempoQuadro
            );

            escurecimento = fmaxf(
                0,
                escurecimento - tempoQuadro
            );

            Vector2 movimento = {0};

            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                movimento.x -= 1;

            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                movimento.x += 1;

            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                movimento.y -= 1;

            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                movimento.y += 1;

            andando = false;

            if (movimento.x != 0 || movimento.y != 0) {
                if (fabsf(movimento.y) > fabsf(movimento.x)) {
                    direcao = movimento.y < 0
                        ? CIMA
                        : BAIXO;
                } else {
                    direcao = movimento.x < 0
                        ? ESQUERDA
                        : DIREITA;
                }

                float comprimento = hypotf(
                    movimento.x,
                    movimento.y
                );

                movimento.x /= comprimento;
                movimento.y /= comprimento;

                Vector2 posicaoAnterior = personagem;

                personagem = moverComColisao(
                    &mascaras[salaAtual],
                    personagem,
                    (Vector2){
                        movimento.x *
                        VELOCIDADE_JOHN *
                        tempoQuadro,

                        movimento.y *
                        VELOCIDADE_JOHN *
                        tempoQuadro
                    }
                );

                andando = hypotf(
                    personagem.x - posicaoAnterior.x,
                    personagem.y - posicaoAnterior.y
                ) > 0.01f;

                mudouSala = trocarSala(
                    &salaAtual,
                    &personagem,
                    &direcao,
                    movimento,
                    &esperaTransicao
                );

                if (mudouSala) {
                    escurecimento = 0.14f;
                    andando = false;

                    TraceLog(
                        LOG_INFO,
                        "Entrada na sala: %s | posicao %.0f, %.0f",
                        salas[salaAtual].nome,
                        personagem.x,
                        personagem.y
                    );
                }
            }

            if (IsKeyPressed(KEY_E)) {
                int idColetado = 0;

                if (coletarObjetoProximo(
                    salaAtual,
                    personagem,
                    &inventario.dados,
                    &idColetado
                )) {
                    mostrarLegendaObjeto(idColetado);
                }
            }

            if (andando) {
                tempoAnimacao += tempoQuadro;

                while (tempoAnimacao >= duracaoQuadro) {
                    tempoAnimacao -= duracaoQuadro;

                    quadroAnimacao =
                        (quadroAnimacao + 1) %
                        NUM_QUADROS;
                }
            } else {
                quadroAnimacao = 0;
                tempoAnimacao = 0;
            }

            if (IsKeyPressed(KEY_F1))
                mostrarColisoes = !mostrarColisoes;
        }

        if (
            !inventarioAberto &&
            mostrarColisoes &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        ) {
            Vector2 ponto = telaParaMundo(
                GetMousePosition(),
                areaExibicao(
                    GetScreenWidth(),
                    GetScreenHeight()
                )
            );

            printf(
                "Ponto: X = %.2f%%, Y = %.2f%% | "
                "Imagem: %.0f, %.0f\n",
                100 * ponto.x / LARGURA_MUNDO,
                100 * ponto.y / ALTURA_MUNDO,
                ponto.x,
                ponto.y
            );

            fflush(stdout);
        }

        Texture2D personagemAtual =
            andando && !mudouSala
            ? animacoes[direcao][quadroAnimacao]
            : personagemParado[direcao];

        BeginTextureMode(quadro);

        ClearBackground(BLACK);

        DrawTexture(
            cenarios[salaAtual],
            0,
            0,
            WHITE
        );

        desenharJohnComCamadas(
            cenarios[salaAtual],
            personagemAtual,
            salaAtual,
            personagem
        );

        desenharObjetosMapa(salaAtual);

        if (escurecimento > 0) {
            DrawRectangle(
                0,
                0,
                LARGURA_MUNDO,
                ALTURA_MUNDO,
                Fade(BLACK, escurecimento * 2)
            );
        }

        if (mostrarColisoes) {
            desenharDepuracao(
                salaAtual,
                personagem,
                texturasMascaras[salaAtual]
            );
        }

        DrawRectangle(
            18,
            16,
            320,
            64,
            (Color){6, 8, 14, 195}
        );

        DrawText(
            salas[salaAtual].nome,
            30,
            24,
            20,
            RAYWHITE
        );

        DrawText(
            TextFormat(
                "FITAS: %d / 3",
                inventarioContarFitas(&inventario.dados)
            ),
            30,
            52,
            18,
            GOLD
        );

        if (!inventarioAberto) {
            desenharIndicacaoColeta(
                salaAtual,
                personagem
            );
        }

        desenharInventario(
            &inventario,
            LARGURA_MUNDO,
            ALTURA_MUNDO
        );

        EndTextureMode();

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexturePro(
            quadro.texture,
            (Rectangle){
                0,
                0,
                LARGURA_MUNDO,
                -ALTURA_MUNDO
            },
            areaExibicao(
                GetScreenWidth(),
                GetScreenHeight()
            ),
            (Vector2){0, 0},
            0,
            WHITE
        );

        desenharLegendas();

        EndDrawing();
    }

    if (quadro.id)
        UnloadRenderTexture(quadro);

    for (int sala = 0; sala < NUM_CENARIOS; sala++) {
        if (cenarios[sala].id)
            UnloadTexture(cenarios[sala]);

        if (texturasMascaras[sala].id)
            UnloadTexture(texturasMascaras[sala]);

        liberarColisoes(&mascaras[sala]);
    }

    for (
        int direcaoSprite = 0;
        direcaoSprite < NUM_DIRECOES;
        direcaoSprite++
    ) {
        if (personagemParado[direcaoSprite].id) {
            UnloadTexture(
                personagemParado[direcaoSprite]
            );
        }

        for (
            int quadroSprite = 0;
            quadroSprite < NUM_QUADROS;
            quadroSprite++
        ) {
            if (animacoes[direcaoSprite][quadroSprite].id) {
                UnloadTexture(
                    animacoes[direcaoSprite][quadroSprite]
                );
            }
        }
    }

    liberarObjetosMapa();

    liberarTelaInventario(
        &inventario
    );

    liberarCamadas();

    liberarLegendas();

    CloseWindow();

    return erro ? 1 : 0;
}
