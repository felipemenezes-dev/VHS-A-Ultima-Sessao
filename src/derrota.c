#include "derrota.h"
#include "mundo.h"

static Texture2D fundo;
static const Rectangle botao = {566, 690, 540, 84};

static bool sobreBotao(void) {
    Vector2 cursor = telaParaMundo(GetMousePosition(),
        areaExibicao(GetScreenWidth(), GetScreenHeight()));
    return CheckCollisionPointRec(cursor, botao);
}

bool carregarDerrota(void) {
    fundo = LoadTexture("assets/ui/game_over.png");
    return fundo.id != 0;
}

void descarregarDerrota(void) {
    if (fundo.id)
        UnloadTexture(fundo);
    fundo = (Texture2D){0};
}

bool voltarAoMenu(void) {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) ||
        (sobreBotao() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

void desenharDerrota(void) {
    DrawTexturePro(fundo, (Rectangle){0, 0, fundo.width, fundo.height},
        (Rectangle){0, 0, LARGURA_MUNDO, ALTURA_MUNDO}, (Vector2){0}, 0, WHITE);
    DrawRectangle(0, 280, LARGURA_MUNDO, 250, Fade(BLACK, 0.65f));
    const char *titulo = "GAME OVER";
    const char *mensagem = "SINAL PERDIDO";
    DrawText(titulo, (LARGURA_MUNDO - MeasureText(titulo, 90)) / 2, 320, 90, RED);
    DrawText(mensagem, (LARGURA_MUNDO - MeasureText(mensagem, 30)) / 2, 440, 30, RAYWHITE);
    DrawRectangleRec(botao, sobreBotao() ? (Color){100, 12, 20, 245} : (Color){18, 8, 12, 245});
    DrawRectangleLinesEx(botao, 3, RED);
    const char *texto = "VOLTAR AO MENU";
    DrawText(texto, (LARGURA_MUNDO - MeasureText(texto, 32)) / 2, botao.y + 26, 32, RAYWHITE);
}
