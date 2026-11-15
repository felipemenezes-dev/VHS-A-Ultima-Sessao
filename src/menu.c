#include "menu.h"
#include "raylib.h"
#include "mundo.h"
#include <stddef.h>

typedef enum { OPCAO_INICIAR = 0, OPCAO_COMO_JOGAR, OPCAO_CREDITOS, OPCAO_SAIR, TOTAL_OPCOES } OpcoesMenu;

static Texture2D texturaFundo;
static Music musicaMenu;
static int opcaoSelecionada = OPCAO_INICIAR;
static EstadoSubMenu estadoAtual = ESTADO_MENU_PRINCIPAL;
static float tempoEfeito = 0.0f;
static float brilhoOscilacao = 1.0f;
static float alphaFade = 0.0f;
static bool iniciandoFade = false;

bool carregarMenu(void){
    if(IsAudioDeviceReady() && musicaMenu.stream.buffer == NULL){
        musicaMenu = LoadMusicStream("assets/sons/musica_menu.wav");
        if(musicaMenu.stream.buffer != NULL)
            musicaMenu.looping = true;
    }
    if(texturaFundo.id != 0){
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
    for(int i = 0; i < totalCaminhos; i++){
        if(FileExists(caminhosPossiveis[i])){
            texturaFundo = LoadTexture(caminhosPossiveis[i]);
            if(texturaFundo.id != 0){
                TraceLog(LOG_INFO, "Fundo do menu carregado com sucesso: %s", caminhosPossiveis[i]);
                break;
            }
        }
    }
    if(texturaFundo.id == 0){
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

void descarregarMenu(void){
    if(musicaMenu.stream.buffer != NULL){
        UnloadMusicStream(musicaMenu);
        musicaMenu = (Music){0};
    }
    if(texturaFundo.id != 0){
        UnloadTexture(texturaFundo);
        texturaFundo = (Texture2D){0};
    }
}

EstadoSubMenu atualizarMenu(void){
    if(musicaMenu.stream.buffer != NULL){
        if(!IsMusicStreamPlaying(musicaMenu))
            PlayMusicStream(musicaMenu);
        UpdateMusicStream(musicaMenu);
    }
    float dt = GetFrameTime();
    tempoEfeito += dt;
    brilhoOscilacao = 0.90f + 0.10f * (float)GetRandomValue(8, 10) / 10.0f;
    if(iniciandoFade){
        alphaFade += dt * 1.5f;
        if(alphaFade >= 1.0f){
            alphaFade = 1.0f;
            if(musicaMenu.stream.buffer != NULL)
                StopMusicStream(musicaMenu);
            return ESTADO_MENU_INICIAR_JOGO;
        }
        return ESTADO_MENU_PRINCIPAL;
    }
    if(estadoAtual == ESTADO_MENU_COMO_JOGAR || estadoAtual == ESTADO_MENU_CREDITOS){
        if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)){
            estadoAtual = ESTADO_MENU_PRINCIPAL;
        }
        return estadoAtual;
    }
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        opcaoSelecionada = (opcaoSelecionada - 1 + TOTAL_OPCOES) % TOTAL_OPCOES;
    }
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        opcaoSelecionada = (opcaoSelecionada + 1) % TOTAL_OPCOES;
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
        switch(opcaoSelecionada){
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
                if(musicaMenu.stream.buffer != NULL)
                    StopMusicStream(musicaMenu);
                return ESTADO_MENU_SAIR;
        }
    }
    return estadoAtual;
}

void desenharMenu(void){
    Color corFundo = ColorAlpha(WHITE, brilhoOscilacao);
    if(texturaFundo.id != 0){
        DrawTexturePro(texturaFundo, (Rectangle){0, 0, (float)texturaFundo.width, (float)texturaFundo.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0f, corFundo);
    } else {
        ClearBackground((Color){10, 10, 15, 255});
    }
    if(GetRandomValue(0, 100) > 85){
        int yRuido = GetRandomValue(0, GetScreenHeight());
        DrawRectangle(0, yRuido, GetScreenWidth(), 2, ColorAlpha(RED, 0.15f));
    }
    if(estadoAtual == ESTADO_MENU_PRINCIPAL){
        const char *opcoesTextos[TOTAL_OPCOES] = {"1. INICIAR SESSÃO", "2. COMO JOGAR", "3. CRÉDITOS", "4. SAIR"};
        int startY = GetScreenHeight() - 240;
        for(int i = 0; i < TOTAL_OPCOES; i++){
            Color corTexto = RAYWHITE;
            int posX = 80;
            if(i == opcaoSelecionada){
                corTexto = RED;
                posX += (GetRandomValue(0, 100) > 80) ? 3 : 0;
                DrawText(">", posX - 25, startY + (i * 45), 24, RED);
            }
            DrawText(opcoesTextos[i], posX, startY + (i * 45), 24, corTexto);
        }
    } else if(estadoAtual == ESTADO_MENU_COMO_JOGAR){
        DrawRectangle(60, 220, 680, 420, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx((Rectangle){60, 220, 680, 420}, 2, RED);
        DrawText("CONTROLES", 80, 240, 24, RED);
        const char *controles[][2] = {
            {"W, A, S, D / Setas", "Movimentar John"},
            {"E", "Interagir"},
            {"Z / X / Roda", "Selecionar item"},
            {"F", "Usar item selecionado"},
            {"Q", "Atalho para kit médico"},
            {"Esc", "Pausar"}
        };
        for(int i = 0; i < 6; i++){
            DrawText(controles[i][0], 80, 280 + (i * 30), 18, YELLOW);
            DrawText(controles[i][1], 300, 280 + (i * 30), 18, RAYWHITE);
        }
        DrawText("OBJETIVO:", 80, 480, 20, RED);
        DrawText("Recupere três fitas VHS, organize-as por", 80, 510, 18, RAYWHITE);
        DrawText("frequência e escape da locadora com vida.", 80, 535, 18, RAYWHITE);
        DrawText("Pressione ESC para voltar", 80, 590, 16, GRAY);
    } else if(estadoAtual == ESTADO_MENU_CREDITOS){
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
    if(iniciandoFade){
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, alphaFade));
    }
}

typedef enum { PAUSA_FECHADA = 0, PAUSA_MENU, PAUSA_CONTROLES, PAUSA_CONFIRMAR_MENU } EstadoMenuPausa;

static EstadoMenuPausa estadoMenuPausa = PAUSA_FECHADA;
static int opcaoPausa = 0;
static int opcaoConfirmacaoPausa = 1;

static const Rectangle botoesPausa[] = {
    {510, 350, 652, 72},
    {510, 436, 652, 72},
    {510, 522, 652, 72},
    {510, 608, 652, 72}
};

static const Rectangle botoesConfirmacaoPausa[] = {
    {520, 520, 300, 76},
    {850, 520, 300, 76}
};

static Vector2 mouseNoMundo(void){
    return telaParaMundo(GetMousePosition(), areaExibicao(GetScreenWidth(), GetScreenHeight()));
}

static bool pontoSobreRetangulo(Vector2 ponto, Rectangle area){
    return CheckCollisionPointRec(ponto, area);
}

void abrirMenuPausa(void){
    if(estadoMenuPausa != PAUSA_FECHADA)
        return;
    estadoMenuPausa = PAUSA_MENU;
    opcaoPausa = 0;
    opcaoConfirmacaoPausa = 1;
}

bool menuPausaEstaAberto(void){
    return estadoMenuPausa != PAUSA_FECHADA;
}

static AcaoPausa executarOpcaoPausa(int opcao){
    switch(opcao){
        case 0:
            estadoMenuPausa = PAUSA_FECHADA;
            return ACAO_PAUSA_CONTINUAR;
        case 1:
            estadoMenuPausa = PAUSA_CONTROLES;
            return ACAO_PAUSA_NENHUMA;
        case 2:
            estadoMenuPausa = PAUSA_CONFIRMAR_MENU;
            opcaoConfirmacaoPausa = 1;
            return ACAO_PAUSA_NENHUMA;
        case 3:
            estadoMenuPausa = PAUSA_FECHADA;
            return ACAO_PAUSA_SAIR;
        default:
            return ACAO_PAUSA_NENHUMA;
    }
}

AcaoPausa atualizarMenuPausa(void){
    if(estadoMenuPausa == PAUSA_FECHADA)
        return ACAO_PAUSA_NENHUMA;
    if(estadoMenuPausa == PAUSA_CONTROLES){
        if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
            estadoMenuPausa = PAUSA_MENU;
        }
        return ACAO_PAUSA_NENHUMA;
    }
    Vector2 movimentoMouse = GetMouseDelta();
    bool mouseMovido = movimentoMouse.x != 0.0f || movimentoMouse.y != 0.0f;
    if(estadoMenuPausa == PAUSA_CONFIRMAR_MENU){
        if(IsKeyPressed(KEY_ESCAPE)){
            estadoMenuPausa = PAUSA_MENU;
            return ACAO_PAUSA_NENHUMA;
        }
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            opcaoConfirmacaoPausa = 0;
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            opcaoConfirmacaoPausa = 1;
        Vector2 mouse = mouseNoMundo();
        for(int i = 0; i < 2; i++){
            if(pontoSobreRetangulo(mouse, botoesConfirmacaoPausa[i])){
                if(mouseMovido)
                    opcaoConfirmacaoPausa = i;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    opcaoConfirmacaoPausa = i;
                    if(i == 0){
                        estadoMenuPausa = PAUSA_FECHADA;
                        return ACAO_PAUSA_MENU_PRINCIPAL;
                    }
                    estadoMenuPausa = PAUSA_MENU;
                    return ACAO_PAUSA_NENHUMA;
                }
            }
        }
        if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
            if(opcaoConfirmacaoPausa == 0){
                estadoMenuPausa = PAUSA_FECHADA;
                return ACAO_PAUSA_MENU_PRINCIPAL;
            }
            estadoMenuPausa = PAUSA_MENU;
        }
        return ACAO_PAUSA_NENHUMA;
    }
    if(estadoMenuPausa == PAUSA_MENU){
        if(IsKeyPressed(KEY_ESCAPE)){
            estadoMenuPausa = PAUSA_FECHADA;
            return ACAO_PAUSA_CONTINUAR;
        }
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            opcaoPausa = (opcaoPausa - 1 + 4) % 4;
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            opcaoPausa = (opcaoPausa + 1) % 4;
        Vector2 mouse = mouseNoMundo();
        for(int i = 0; i < 4; i++){
            if(pontoSobreRetangulo(mouse, botoesPausa[i])){
                if(mouseMovido)
                    opcaoPausa = i;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    opcaoPausa = i;
                    return executarOpcaoPausa(i);
                }
            }
        }
        if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            return executarOpcaoPausa(opcaoPausa);
    }
    return ACAO_PAUSA_NENHUMA;
}

static void desenharMolduraPausa(void){
    DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, (Color){0, 0, 0, 205});
    DrawRectangle(390, 115, 892, 710, (Color){7, 9, 15, 248});
    DrawRectangleLinesEx((Rectangle){390, 115, 892, 710}, 4, (Color){145, 40, 52, 255});
}

void desenharMenuPausa(void){
    if(estadoMenuPausa == PAUSA_FECHADA)
        return;
    desenharMolduraPausa();
    if(estadoMenuPausa == PAUSA_CONTROLES){
        DrawText("CONTROLES", 690, 160, 42, RED);
        const char *controles[][2] = {
            {"W, A, S, D / Setas", "Movimentar John"},
            {"E", "Interagir com objetos"},
            {"Z / X / Roda", "Selecionar item"},
            {"F", "Usar o kit medico selecionado"},
            {"Q", "Atalho para kit medico"},
            {"Esc", "Voltar ao menu de pausa"}
        };
        for(int i = 0; i < 6; i++){
            DrawText(controles[i][0], 490, 245 + i * 62, 22, GOLD);
            DrawText(controles[i][1], 790, 245 + i * 62, 22, RAYWHITE);
        }
        DrawText("ENTER OU ESC - VOLTAR", 650, 740, 20, LIGHTGRAY);
        return;
    }
    if(estadoMenuPausa == PAUSA_CONFIRMAR_MENU){
        DrawText("VOLTAR AO MENU PRINCIPAL?", 525, 270, 34, RAYWHITE);
        DrawText("A sessao atual sera perdida.", 640, 350, 24, LIGHTGRAY);
        for(int i = 0; i < 2; i++){
            bool selecionado = i == opcaoConfirmacaoPausa;
            Rectangle area = botoesConfirmacaoPausa[i];
            DrawRectangleRec(area, selecionado ? (Color){90, 15, 27, 255} : (Color){25, 28, 36, 255});
            DrawRectangleLinesEx(area, selecionado ? 3.0f : 1.5f, selecionado ? GOLD : GRAY);
            const char *texto = i == 0 ? "SIM" : "NAO";
            DrawText(texto, (int)(area.x + (area.width - MeasureText(texto, 28)) / 2), (int)area.y + 23, 28, selecionado ? GOLD : RAYWHITE);
        }
        DrawText("SELECIONE COM A / D E CONFIRME COM ENTER", 535, 650, 20, LIGHTGRAY);
        return;
    }
    DrawText("JOGO PAUSADO", 680, 170, 42, GOLD);
    const char *opcoes[] = {"CONTINUAR", "CONTROLES", "VOLTAR AO MENU PRINCIPAL", "SAIR DO JOGO"};
    for(int i = 0; i < 4; i++){
        Rectangle area = botoesPausa[i];
        bool selecionado = i == opcaoPausa;
        DrawRectangleRec(area, selecionado ? (Color){67, 13, 25, 255} : (Color){20, 23, 31, 255});
        DrawRectangleLinesEx(area, selecionado ? 3.0f : 1.5f, selecionado ? GOLD : (Color){100, 104, 115, 255});
        DrawText(opcoes[i], (int)(area.x + (area.width - MeasureText(opcoes[i], 27)) / 2), (int)area.y + 22, 27, selecionado ? GOLD : RAYWHITE);
    }
    DrawText("SETAS: NAVEGAR     ENTER: SELECIONAR     ESC: CONTINUAR", 485, 735, 19, LIGHTGRAY);
}