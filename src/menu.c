#include "menu.h"
#include <stddef.h>

typedef enum {
    OPCAO_INICIAR = 0,
    OPCAO_COMO_JOGAR,
    OPCAO_CREDITOS,
    OPCAO_SAIR,
    TOTAL_OPCOES
} OpcoesMenu;

static Texture2D texturaFundo;
static Music musicaMenu;
static int opcaoSelecionada = OPCAO_INICIAR;
static EstadoSubMenu estadoAtual = ESTADO_MENU_PRINCIPAL;

static float tempoEfeito = 0.0f;
static float brilhoOscilacao = 1.0f;

static float alphaFade = 0.0f;
static bool iniciandoFade = false;

bool carregarMenu(void) {
    if (IsAudioDeviceReady() && musicaMenu.stream.buffer == NULL) {
        musicaMenu = LoadMusicStream("assets/sons/musica_menu.mp3");
        if (musicaMenu.stream.buffer != NULL)
            musicaMenu.looping = true;
    }
    if (texturaFundo.id != 0) {
        UnloadTexture(texturaFundo);
        texturaFundo = (Texture2D){0};
    }

    const char *caminhosPossiveis[] = {
        "assets/extras/menu_fundo.png",
        "assets/extras/menu_fundo.PNG",
        "assets/extras/menu_fundo.jpg",
        "assets/extras/menu_fundo.JPG",
        "assets/extras/menu_fundo.jpeg",
        "assets/menu/menu_fundo.png",
        "assets/menu/menu_fundo.PNG",
        "assets/menu/menu_fundo.jpg",
        "assets/menu/menu_fundo.JPG",
        "assets/menu/menu_fundo.jpeg",
        "../assets/menu/menu_fundo.png",
        "../assets/menu/menu_fundo.jpg"
    };

    int totalCaminhos = sizeof(caminhosPossiveis) / sizeof(caminhosPossiveis[0]);

    for (int i = 0; i < totalCaminhos; i++) {
        if (FileExists(caminhosPossiveis[i])) {
            texturaFundo = LoadTexture(caminhosPossiveis[i]);
            if (texturaFundo.id != 0) {
                TraceLog(LOG_INFO, "Fundo do menu carregado com sucesso: %s", caminhosPossiveis[i]);
                break;
            }
        }
    }

    if (texturaFundo.id == 0) {
        TraceLog(LOG_ERROR, "ERRO CRITICO: Nao foi possivel carregar o fundo do menu em nenhum dos caminhos esperados!");
    } else {
        SetTextureFilter(texturaFundo, TEXTURE_FILTER_POINT);
    }
    
    opcaoSelecionada = OPCAO_INICIAR;
    estadoAtual = ESTADO_MENU_PRINCIPAL;
    alphaFade = 0.0f;
    iniciandoFade = false;
    
    return texturaFundo.id != 0;
}

void descarregarMenu(void) {
    if (musicaMenu.stream.buffer != NULL) {
        UnloadMusicStream(musicaMenu);
        musicaMenu = (Music){0};
    }
    if (texturaFundo.id != 0) {
        UnloadTexture(texturaFundo);
        texturaFundo = (Texture2D){0};
    }
}

EstadoSubMenu atualizarMenu(void) {
    if (musicaMenu.stream.buffer != NULL) {
        if (!IsMusicStreamPlaying(musicaMenu))
            PlayMusicStream(musicaMenu);
        UpdateMusicStream(musicaMenu);
    }
    float dt = GetFrameTime();
    tempoEfeito += dt;

    brilhoOscilacao = 0.90f + 0.10f * (float)GetRandomValue(8, 10) / 10.0f;

    if (iniciandoFade) {
        alphaFade += dt * 1.5f;
        if (alphaFade >= 1.0f) {
            alphaFade = 1.0f;
            if (musicaMenu.stream.buffer != NULL)
                StopMusicStream(musicaMenu);
            return ESTADO_MENU_INICIAR_JOGO;
        }
        return ESTADO_MENU_PRINCIPAL;
    }

    if (estadoAtual == ESTADO_MENU_COMO_JOGAR || estadoAtual == ESTADO_MENU_CREDITOS) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
            estadoAtual = ESTADO_MENU_PRINCIPAL;
        }
        return estadoAtual;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        opcaoSelecionada = (opcaoSelecionada - 1 + TOTAL_OPCOES) % TOTAL_OPCOES;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        opcaoSelecionada = (opcaoSelecionada + 1) % TOTAL_OPCOES;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (opcaoSelecionada) {
            case OPCAO_INICIAR:
                iniciandoFade = true;
                break;
            case OPCAO_COMO_JOGAR:
                estadoAtual = ESTADO_MENU_COMO_JOGAR;
                break;
            case OPCAO_CREDITOS:
                estadoAtual = ESTADO_MENU_CREDITOS;
                break;
            case OPCAO_SAIR:
                if (musicaMenu.stream.buffer != NULL)
                    StopMusicStream(musicaMenu);
                return ESTADO_MENU_SAIR;
        }
    }

    return estadoAtual;
}

void desenharMenu(void) {
    Color corFundo = ColorAlpha(WHITE, brilhoOscilacao);
    if (texturaFundo.id != 0) {
        DrawTexturePro(
            texturaFundo,
            (Rectangle){0, 0, (float)texturaFundo.width, (float)texturaFundo.height},
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0}, 0.0f, corFundo
        );
    } else {
        ClearBackground((Color){10, 10, 15, 255});
    }

    if (GetRandomValue(0, 100) > 85) {
        int yRuido = GetRandomValue(0, GetScreenHeight());
        DrawRectangle(0, yRuido, GetScreenWidth(), 2, ColorAlpha(RED, 0.15f));
    }

    if (estadoAtual == ESTADO_MENU_PRINCIPAL) {
        const char *opcoesTextos[TOTAL_OPCOES] = {
            "1. INICIAR SESSÃO",
            "2. COMO JOGAR",
            "3. CRÉDITOS",
            "4. SAIR"
        };

        int startY = GetScreenHeight() - 240;

        for (int i = 0; i < TOTAL_OPCOES; i++) {
            Color corTexto = RAYWHITE;
            int posX = 80;

            if (i == opcaoSelecionada) {
                corTexto = RED;
                posX += (GetRandomValue(0, 100) > 80) ? 3 : 0;
                DrawText(">", posX - 25, startY + (i * 45), 24, RED);
            }

            DrawText(opcoesTextos[i], posX, startY + (i * 45), 24, corTexto);
        }
    } 
    else if (estadoAtual == ESTADO_MENU_COMO_JOGAR) {
        DrawRectangle(60, 220, 680, 420, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx((Rectangle){60, 220, 680, 420}, 2, RED);

        DrawText("CONTROLES", 80, 240, 24, RED);

        const char *controles[][2] = {
            {"W, A, S, D / Setas", "Movimentar John"},
            {"E", "Interagir"},
            {"F", "Usar o flash"},
            {"Q", "Usar o kit médico"},
            {"Tab", "Abrir inventário"},
            {"Esc", "Pausar"}
        };

        for (int i = 0; i < 6; i++) {
            DrawText(controles[i][0], 80, 280 + (i * 30), 18, YELLOW);
            DrawText(controles[i][1], 300, 280 + (i * 30), 18, RAYWHITE);
        }

        DrawText("OBJETIVO:", 80, 480, 20, RED);
        DrawText("Recupere três fitas VHS, organize-as por", 80, 510, 18, RAYWHITE);
        DrawText("frequência e escape da locadora com vida.", 80, 535, 18, RAYWHITE);

        DrawText("Pressione ESC para voltar", 80, 590, 16, GRAY);
    } 
    else if (estadoAtual == ESTADO_MENU_CREDITOS) {
        DrawRectangle(60, 220, 680, 420, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx((Rectangle){60, 220, 680, 420}, 2, RED);

        DrawText("CRÉDITOS", 80, 240, 24, RED);
        DrawText("Projeto Acadêmico Autoral", 80, 275, 18, GRAY);

        DrawText("DESENVOLVEDORES:", 80, 320, 20, YELLOW);
        
        DrawText("1. Breno Pereira", 80, 355, 18, RAYWHITE);
        DrawText("2. Danilo Duleba", 80, 385, 18, RAYWHITE);
        DrawText("3. Felipe Espínola", 80, 415, 18, RAYWHITE);
        DrawText("4. Rafael Borba", 80, 445, 18, RAYWHITE);
        DrawText("5. Walter Maia", 80, 475, 18, RAYWHITE);

        DrawText("Pressione ESC para voltar", 80, 590, 16, GRAY);
    }

    if (iniciandoFade) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, alphaFade));
    }
}
