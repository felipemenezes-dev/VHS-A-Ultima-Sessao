#include "raylib.h"
#include "mundo.h"
#include "colisoes.h"
#include "camadas.h"
#include "interface_inventario.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Texture2D carregarImagemPersonagem(const char *caminho) {
    Image imagem = LoadImage(caminho);
    Texture2D textura = {0};
    if (imagem.data) {
        ImageAlphaCrop(&imagem, .05f);
        textura = LoadTextureFromImage(imagem);
        UnloadImage(imagem);
        if (textura.id)
            SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    }
    return textura;
}

static void localizarRecursos(const char *executavel) {
    if (FileExists("assets/cenarios/salao_principal.png"))
        return;
    char caminho[4096];
    ssize_t quantidade = readlink("/proc/self/exe", caminho, sizeof caminho - 1);
    if (quantidade > 0)
        caminho[quantidade] = '\0';
    else
        snprintf(caminho, sizeof caminho, "%s", executavel);
    char *barra = strrchr(caminho, '/');
    if (!barra)
        return;
    *barra = '\0';
    if (ChangeDirectory(caminho) && !FileExists("assets/cenarios/salao_principal.png"))
        ChangeDirectory("..");
}

static void capturarTela(const char *caminho) {
    Image imagem = LoadImageFromScreen();
    if (imagem.data) {
        ExportImage(imagem, caminho);
        UnloadImage(imagem);
    }
}

static void desenharDepuracao(int sala, Vector2 john, Texture2D mascara) {
    DrawTexture(mascara, 0, 0, (Color){125, 255, 150, 210});
    DrawRectangleLinesEx(caixaPes(john), 2, YELLOW);
    const LimiteChao *chao = &limitesChao[sala];
    for (int i = 0; i < chao->quantidade; i++)
        DrawLineEx(chao->pontos[i], chao->pontos[(i + 1) % chao->quantidade], 3, VIOLET);
    for (int i = 0; i < quantidadePortas; i++) {
        const Porta *d = &portas[i];
        if (d->origem == sala) {
            DrawRectangleLinesEx(d->area, 3, SKYBLUE);
            DrawText(TextFormat("P%d -> %s", i, d->rotulo), d->area.x, d->area.y - 22, 20, SKYBLUE);
            if (d->limiteA.x)
                DrawLineEx(d->limiteA, d->limiteB, 3, ORANGE);
        }
        if (d->destino == sala) {
            DrawCircleLines(d->chegada.x, d->chegada.y, 18, YELLOW);
            DrawText(TextFormat("Entrada %d", i), d->chegada.x + 22, d->chegada.y - 10, 19, YELLOW);
        }
    }
    DrawRectangle(20, 90, 440, 116, (Color){6, 8, 14, 230});
    DrawText("F1 - DEPURACAO", 34, 102, 22, YELLOW);
    DrawText(TextFormat("Pes: %.1f, %.1f | %.1f x %.1f", john.x, john.y, LARGURA_PES, ALTURA_PES),
             34, 135, 19, RAYWHITE);
    DrawText("Roxo: limite do piso", 34, 184, 18, VIOLET);
    DrawText("Azul: transicao | Amarelo: entrada", 34, 162, 18, SKYBLUE);
}

int main(int quantidadeArgumentos, char **argumentos) {
    int larguraInicial = 1280, alturaInicial = 720, limiteQuadros = 0;
    bool telaCheia = false;
    const char *captura = NULL, *galeria = NULL;
    for (int i = 1; i < quantidadeArgumentos; i++) {
        if (!strcmp(argumentos[i], "--fullscreen"))
            telaCheia = true;
        else if (!strcmp(argumentos[i], "--size") && i + 2 < quantidadeArgumentos) {
            larguraInicial = atoi(argumentos[++i]);
            alturaInicial = atoi(argumentos[++i]);
        } else if (!strcmp(argumentos[i], "--frames") && i + 1 < quantidadeArgumentos)
            limiteQuadros = atoi(argumentos[++i]);
        else if (!strcmp(argumentos[i], "--capture") && i + 1 < quantidadeArgumentos)
            captura = argumentos[++i];
        else if (!strcmp(argumentos[i], "--gallery") && i + 1 < quantidadeArgumentos)
            galeria = argumentos[++i];
        else {
            fprintf(stderr,
                    "Uso: %s [--fullscreen] [--size L A] [--frames N --capture arquivo.png] "
                    "[--gallery pasta]\n",
                    argumentos[0]);
            return 2;
        }
    }
    if (larguraInicial < 640 || alturaInicial < 360 || larguraInicial > 7680 ||
        alturaInicial > 4320) {
        fprintf(stderr, "Resolucao invalida (minimo 640 x 360).\n");
        return 2;
    }
    localizarRecursos(argumentos[0]);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(larguraInicial, alturaInicial, "VHS - A Ultima Sessao");
    if (!IsWindowReady())
        return 1;
    SetExitKey(KEY_NULL);
    SetWindowMinSize(640, 360);
    if (telaCheia)
        ToggleBorderlessWindowed();
    SetTargetFPS(60);

    Texture2D cenarios[NUM_CENARIOS] = {0}, texturasMascaras[NUM_CENARIOS] = {0};
    MapaColisao mascaras[NUM_CENARIOS] = {0};
    Texture2D animacoes[NUM_DIRECOES][NUM_QUADROS] = {0};
    Texture2D johnParado[NUM_DIRECOES] = {0};
    const char *pastas[NUM_DIRECOES] = {"baixo", "cima", "direita", "esquerda"};
    bool erro = false;
    for (int i = 0; i < NUM_CENARIOS; i++) {
        cenarios[i] = LoadTexture(salas[i].imagem);
        texturasMascaras[i] = LoadTexture(salas[i].mascara);
        if (!cenarios[i].id || cenarios[i].width != LARGURA_MUNDO ||
            cenarios[i].height != ALTURA_MUNDO || !texturasMascaras[i].id ||
            !carregarColisoesSala(&mascaras[i], i)) {
            TraceLog(LOG_ERROR, "Cenario/mascara ausente ou invalido: %s", salas[i].nome);
            erro = true;
        } else {
            SetTextureFilter(cenarios[i], TEXTURE_FILTER_POINT);
            SetTextureFilter(texturasMascaras[i], TEXTURE_FILTER_POINT);
        }
    }
    for (int d = 0; d < NUM_DIRECOES; d++) {
        char caminho[256];
        snprintf(caminho, sizeof caminho, "assets/personagens/john/%s/john_parado_%s.png",
                 pastas[d], pastas[d]);
        johnParado[d] = carregarImagemPersonagem(caminho);
        if (!johnParado[d].id)
            erro = true;
        for (int f = 0; f < NUM_QUADROS; f++) {
            snprintf(caminho, sizeof caminho, "assets/personagens/john/%s/john_andando_%s%d.png",
                     pastas[d], pastas[d], f + 1);
            animacoes[d][f] = carregarImagemPersonagem(caminho);
            if (!animacoes[d][f].id)
                erro = true;
        }
    }
    for (int i = 0; i < NUM_CENARIOS && !erro; i++) {
        if (pesBloqueados(&mascaras[i], salas[i].entrada)) {
            TraceLog(LOG_ERROR, "Entrada bloqueada: %s", salas[i].nome);
            erro = true;
        }
    }
    for (int i = 0; i < quantidadePortas && !erro; i++) {
        if (pesBloqueados(&mascaras[portas[i].destino], portas[i].chegada)) {
            TraceLog(LOG_ERROR, "Destino da porta %d bloqueado", i);
            erro = true;
        }
    }
    if (!erro && !iniciarCamadas()) {
        TraceLog(LOG_ERROR, "Falha ao preparar camadas visuais");
        erro = true;
    }
    TelaInventario inventario = {0};
    if (!erro && !iniciarTelaInventario(&inventario)) {
        TraceLog(LOG_ERROR, "Fundo do inventario ausente");
        erro = true;
    }
    RenderTexture2D quadro = {0};
    if (!erro) {
        quadro = LoadRenderTexture(LARGURA_MUNDO, ALTURA_MUNDO);
        if (!quadro.id)
            erro = true;
        else
            SetTextureFilter(quadro.texture, TEXTURE_FILTER_BILINEAR);
    }

    Vector2 john = salas[SALAO].entrada;
    int cenarioAtual = SALAO, direcao = BAIXO, quadroAnimacaoAtual = 0;
    float tempoAnimacao = 0, duracaoQuadroAnimacao = .11f, esperaTransicao = 0, escurecimento = 0;
    bool mostrarColisoes = false, andando = false;
    int quadroAtual = 0, indiceGaleria = 0;
    while (!erro && !WindowShouldClose()) {
        bool escPressionado = IsKeyPressed(KEY_ESCAPE);
        if (escPressionado && !inventario.aberto)
            break;
        bool tabPressionado = IsKeyPressed(KEY_TAB);
        bool pausarQuadro = atualizarTelaInventario(&inventario, tabPressionado, escPressionado);
        if (inventario.aberto && !tabPressionado) {
            int passoX = (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) -
                         (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A));
            int passoY = (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) -
                         (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W));
            Vector2 cursor = telaParaMundo(GetMousePosition(),
                                           areaExibicao(GetScreenWidth(), GetScreenHeight()));
            selecionarEspacoInventario(&inventario, passoX, passoY,
                                       IsMouseButtonPressed(MOUSE_BUTTON_LEFT), cursor,
                                       LARGURA_MUNDO, ALTURA_MUNDO);
        }
        bool mudouCenario = false;
        if (!pausarQuadro) {
            float tempoQuadro = fminf(GetFrameTime(), .25f);
            esperaTransicao = fmaxf(0, esperaTransicao - tempoQuadro);
            escurecimento = fmaxf(0, escurecimento - tempoQuadro);
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
            if (movimento.x || movimento.y) {
                if (fabsf(movimento.y) > fabsf(movimento.x))
                    direcao = movimento.y < 0 ? CIMA : BAIXO;
                else
                    direcao = movimento.x < 0 ? ESQUERDA : DIREITA;
                float comprimento = hypotf(movimento.x, movimento.y);
                movimento.x /= comprimento;
                movimento.y /= comprimento;
                Vector2 anterior = john;
                john = moverComColisao(&mascaras[cenarioAtual], john,
                                       (Vector2){movimento.x * VELOCIDADE_JOHN * tempoQuadro,
                                                 movimento.y * VELOCIDADE_JOHN * tempoQuadro});
                andando = hypotf(john.x - anterior.x, john.y - anterior.y) > .01f;
                mudouCenario =
                    trocarSala(&cenarioAtual, &john, &direcao, movimento, &esperaTransicao);
                if (mudouCenario) {
                    escurecimento = .14f;
                    andando = false;
                    TraceLog(LOG_INFO, "Passagem: %s | entrada %.0f, %.0f",
                             salas[cenarioAtual].nome, john.x, john.y);
                }
            }
            if (andando) {
                tempoAnimacao += tempoQuadro;
                while (tempoAnimacao >= duracaoQuadroAnimacao) {
                    tempoAnimacao -= duracaoQuadroAnimacao;
                    quadroAnimacaoAtual = (quadroAnimacaoAtual + 1) % NUM_QUADROS;
                }
            } else {
                quadroAnimacaoAtual = 0;
                tempoAnimacao = 0;
            }
            if (IsKeyPressed(KEY_F1))
                mostrarColisoes = !mostrarColisoes;
        }
        if (IsKeyPressed(KEY_F11))
            ToggleBorderlessWindowed();
        if (!pausarQuadro && mostrarColisoes && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 p = telaParaMundo(GetMousePosition(),
                                      areaExibicao(GetScreenWidth(), GetScreenHeight()));
            printf("Ponto: X = %.2f%%, Y = %.2f%% | Imagem: %.0f, %.0f\n",
                   100 * p.x / LARGURA_MUNDO, 100 * p.y / ALTURA_MUNDO, p.x, p.y);
            fflush(stdout);
        }
        if (galeria) {
            cenarioAtual = indiceGaleria / 2;
            john = salas[cenarioAtual].entrada;
            mostrarColisoes = indiceGaleria % 2;
            direcao = BAIXO;
            andando = false;
            escurecimento = 0;
        }
        Texture2D atual = andando && !mudouCenario ? animacoes[direcao][quadroAnimacaoAtual]
                                                   : johnParado[direcao];
        BeginTextureMode(quadro);
        ClearBackground(BLACK);
        DrawTexture(cenarios[cenarioAtual], 0, 0, WHITE);
        desenharJohnComCamadas(cenarios[cenarioAtual], atual, cenarioAtual, john);
        if (escurecimento > 0)
            DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, Fade(BLACK, escurecimento * 2));
        if (mostrarColisoes)
            desenharDepuracao(cenarioAtual, john, texturasMascaras[cenarioAtual]);
        DrawRectangle(18, 16, 270, 34, (Color){6, 8, 14, 195});
        DrawText(salas[cenarioAtual].nome, 30, 24, 20, RAYWHITE);
        DrawRectangle(0, ALTURA_MUNDO - 29, LARGURA_MUNDO, 29, (Color){6, 8, 14, 195});
        DrawText("WASD / SETAS: mover    TAB: inventario    F1: colisoes    F11: tela cheia    "
                 "ESC: sair",
                 24, ALTURA_MUNDO - 22, 17, LIGHTGRAY);
        desenharInventario(&inventario, LARGURA_MUNDO, ALTURA_MUNDO);
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(quadro.texture, (Rectangle){0, 0, LARGURA_MUNDO, -ALTURA_MUNDO},
                       areaExibicao(GetScreenWidth(), GetScreenHeight()), (Vector2){0, 0}, 0,
                       WHITE);
        EndDrawing();
        if (galeria && quadroAtual % 3 == 2) {
            char caminho[4096];
            snprintf(caminho, sizeof caminho, "%s/%d-%s.png", galeria, indiceGaleria / 2,
                     mostrarColisoes ? "debug" : "normal");
            capturarTela(caminho);
            if (++indiceGaleria == 10)
                break;
        }
        quadroAtual++;
        if (limiteQuadros > 0 && quadroAtual >= limiteQuadros) {
            if (captura)
                capturarTela(captura);
            break;
        }
    }
    if (quadro.id)
        UnloadRenderTexture(quadro);
    for (int i = 0; i < NUM_CENARIOS; i++) {
        if (cenarios[i].id)
            UnloadTexture(cenarios[i]);
        if (texturasMascaras[i].id)
            UnloadTexture(texturasMascaras[i]);
        liberarColisoes(&mascaras[i]);
    }
    for (int d = 0; d < NUM_DIRECOES; d++) {
        if (johnParado[d].id)
            UnloadTexture(johnParado[d]);
        for (int f = 0; f < NUM_QUADROS; f++)
            if (animacoes[d][f].id)
                UnloadTexture(animacoes[d][f]);
    }
    liberarTelaInventario(&inventario);
    liberarCamadas();
    CloseWindow();
    return erro ? 1 : 0;
}
