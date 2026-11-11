#include "derrota.h"
#include "mundo.h"

static Texture2D fundo;
static int opcaoSelecionada;
static const Rectangle botoes[3] = {
    {520, 590, 632, 62},
    {520, 665, 632, 62},
    {520, 740, 632, 62}
};

static bool sobreBotao(int indice) {
    if (indice < 0 || indice >= 3)
        return false;

    Vector2 cursor = telaParaMundo(
        GetMousePosition(),
        areaExibicao(GetScreenWidth(), GetScreenHeight())
    );

    return CheckCollisionPointRec(cursor, botoes[indice]);
}

bool carregarDerrota(void) {
    fundo = LoadTexture("assets/ui/game_over.png");
    opcaoSelecionada = 0;
    return fundo.id != 0;
}

void descarregarDerrota(void) {
    if (fundo.id)
        UnloadTexture(fundo);

    fundo = (Texture2D){0};
}

void reiniciarMenuDerrota(void) {
    opcaoSelecionada = 0;
}

AcaoDerrota atualizarMenuDerrota(void) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        opcaoSelecionada = (opcaoSelecionada + 2) % 3;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        opcaoSelecionada = (opcaoSelecionada + 1) % 3;

    if (IsKeyPressed(KEY_ESCAPE))
        return ACAO_DERROTA_MENU_PRINCIPAL;

    for (int i = 0; i < 3; i++) {
        if (sobreBotao(i) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            return (AcaoDerrota)i;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        return (AcaoDerrota)opcaoSelecionada;

    return ACAO_DERROTA_NENHUMA;
}

void desenharDerrota(void) {
    if (fundo.id) {
        DrawTexturePro(
            fundo,
            (Rectangle){0, 0, fundo.width, fundo.height},
            (Rectangle){0, 0, LARGURA_MUNDO, ALTURA_MUNDO},
            (Vector2){0},
            0,
            WHITE
        );
    } else {
        ClearBackground(BLACK);
    }

    DrawRectangle(0, 0, LARGURA_MUNDO, ALTURA_MUNDO, (Color){0, 0, 0, 170});
    DrawRectangle(330, 170, 1012, 650, (Color){5, 6, 12, 235});
    DrawRectangleLinesEx((Rectangle){330, 170, 1012, 650}, 4, RED);

    const char *titulo = "SINAL PERDIDO";
    const char *mensagem = "JOHN FOI ABSORVIDO PELA FITA.";
    const char *descricao = "A sessao terminou antes que a transmissao pudesse ser interrompida.";

    DrawText(
        titulo,
        (LARGURA_MUNDO - MeasureText(titulo, 62)) / 2,
        235,
        62,
        RED
    );

    DrawText(
        mensagem,
        (LARGURA_MUNDO - MeasureText(mensagem, 28)) / 2,
        350,
        28,
        RAYWHITE
    );

    DrawText(
        descricao,
        (LARGURA_MUNDO - MeasureText(descricao, 20)) / 2,
        420,
        20,
        LIGHTGRAY
    );

    const char *opcoes[3] = {
        "TENTAR NOVAMENTE",
        "MENU PRINCIPAL",
        "SAIR"
    };

    for (int i = 0; i < 3; i++) {
        bool selecionado = i == opcaoSelecionada || sobreBotao(i);
        Color fundoBotao = selecionado
            ? (Color){105, 12, 24, 245}
            : (Color){16, 12, 18, 240};
        Color texto = selecionado ? WHITE : LIGHTGRAY;

        DrawRectangleRec(botoes[i], fundoBotao);
        DrawRectangleLinesEx(
            botoes[i],
            2,
            selecionado ? RED : (Color){95, 42, 50, 255}
        );

        if (selecionado)
            DrawText(">", (int)botoes[i].x + 28, (int)botoes[i].y + 15, 30, RED);

        int largura = MeasureText(opcoes[i], 25);
        DrawText(
            opcoes[i],
            (int)(botoes[i].x + (botoes[i].width - largura) / 2),
            (int)botoes[i].y + 18,
            25,
            texto
        );
    }
}
