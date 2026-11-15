#include "interface_inventario.h"
#include "objetos.h"
#include <math.h>
#include <stddef.h>

#define BASE_W 1280.0f
#define BASE_H 720.0f
#define ITEM_W 72.0f
#define ITEM_H 68.0f
#define ITEM_STEP 78.0f
#define MAX_ITENS_VISIVEIS 5
#define PAINEL_X 18.0f
#define PAINEL_Y 560.0f
#define PAINEL_H 145.0f
#define PAINEL_LARGURA_MINIMA 190.0f

static int totalItens(const TelaInventario *inventario) {
    if (!inventario) return 0;
    int total = 0;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) if (inventario->dados.objetos[i].estado != OBJETO_VAZIO) total++;
    return total;
}

static int indiceSelecionado(const TelaInventario *inventario, int total) {
    if (!inventario || total <= 0)
        return 0;
    int selecionado = inventario->selecionado;
    if (selecionado < 0) selecionado = 0;
    if (selecionado >= total) selecionado = total - 1;
    return selecionado;
}

static const ObjetoInventario *itemNaPosicao(const TelaInventario *inventario, int posicao) {
    if (!inventario || posicao < 0)
        return NULL;
    int indice = 0;
    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
        const ObjetoInventario *objeto = &inventario->dados.objetos[i];
        if (objeto->estado == OBJETO_VAZIO) continue;
        if (indice == posicao)
            return objeto;
        indice++;
    }
    return NULL;
}

static const char *nomeCurtoItem(const ObjetoInventario *objeto) {
    if (!objeto)
        return "ITEM";
    switch (objeto->id) {
        case ID_KIT_MEDICO:
            return "KIT MED.";
        case ID_CHAVE_ESTOQUE:
            return "CHAVE";
        case ID_FUSIVEL:
            return "FUSIVEL";
        default: return objeto->nome[0] != "\0"[0] ? objeto->nome : "ITEM";
    }
}

bool iniciarTelaInventario(TelaInventario *inventario) {
    if (!inventario) return false;
    *inventario = (TelaInventario){0};
    return true;
}

void liberarTelaInventario(TelaInventario *inventario) {
    if (!inventario) return;
    inventarioLimpar(&inventario->dados);
    *inventario = (TelaInventario){0};
}

void selecionarItemInventario(TelaInventario *inventario, int passo) {
    if (!inventario || passo == 0)
        return;
    int total = totalItens(inventario);
    if (total <= 0) { inventario->selecionado = 0;
            return; }
    int atual = indiceSelecionado(inventario, total);
    inventario->selecionado = (atual + passo) % total;
    if (inventario->selecionado < 0) inventario->selecionado += total;
}

const ObjetoInventario *inventarioObjetoSelecionado(const TelaInventario *inventario) {
    if (!inventario)
        return NULL;
    int total = totalItens(inventario);
    if (total <= 0) return NULL;
    int selecionado = indiceSelecionado(inventario, total);
    return itemNaPosicao(inventario, selecionado);
}

static int calcularInicioVisivel(const TelaInventario *inventario, int total) {
    if (!inventario || total <= MAX_ITENS_VISIVEIS)
        return 0;
    int selecionado = indiceSelecionado(inventario, total);
    int inicio = selecionado - MAX_ITENS_VISIVEIS / 2;
    if (inicio < 0) inicio = 0;
    int inicioMaximo = total - MAX_ITENS_VISIVEIS;
    if (inicio > inicioMaximo) inicio = inicioMaximo;
    return inicio;
}

static void desenharItem(const TelaInventario *inventario, int indiceReal, int indiceVisual) {
    float x = 28.0f + indiceVisual * ITEM_STEP;
    float y = 591.0f;
    int selecionado = indiceSelecionado(inventario, totalItens(inventario));
    bool estaSelecionado = indiceReal == selecionado;
    Rectangle area = {x, y, ITEM_W, ITEM_H};
    Color fundo = estaSelecionado ? (Color){48, 34, 25, 250} : (Color){7, 10, 16, 240};
    Color borda = estaSelecionado ? (Color){235, 190, 92, 255} : (Color){95, 72, 78, 255};
    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, estaSelecionado ? 2.5f : 1.0f, borda);
    DrawText(TextFormat("%d", indiceReal + 1), (int)x + 4, (int)y + 3, 9, GRAY);
    const ObjetoInventario *objeto = itemNaPosicao(inventario, indiceReal);
    if (!objeto)
        return;
    DrawText(nomeCurtoItem(objeto), (int)x + 4, (int)y + 24, 10, RAYWHITE);
    const char *estado = "DISPONIVEL";
    Color corEstado = LIME;
    if (objeto->estado == OBJETO_USADO) { estado = "USADO"; corEstado = GRAY; }
    else if (objeto->estado == OBJETO_INSTALADO) { estado = "INSTALADO"; corEstado = SKYBLUE; }
    DrawText(estado, (int)x + 4, (int)y + 46, 8, corEstado);
}

void desenharInventario(const TelaInventario *inventario, int largura, int altura) {
    if (!inventario || largura <= 0 || altura <= 0)
        return;
    int total = totalItens(inventario);
    if (total <= 0)
        return;
    int inicio = calcularInicioVisivel(inventario, total);
    int quantidadeVisivel = total - inicio;
    if (quantidadeVisivel > MAX_ITENS_VISIVEIS) quantidadeVisivel = MAX_ITENS_VISIVEIS;
    float escala = fminf((float)largura / BASE_W, (float)altura / BASE_H);
    Camera2D camera = {.offset = {(largura - BASE_W * escala) / 2.0f, (altura - BASE_H * escala) / 2.0f}, .target = {0.0f, 0.0f}, .rotation = 0.0f, .zoom = escala};
    float larguraCalculada = 30.0f + (quantidadeVisivel - 1) * ITEM_STEP + ITEM_W + 20.0f;
    float larguraPainel = fmaxf(larguraCalculada, PAINEL_LARGURA_MINIMA);

    BeginMode2D(camera);
    DrawRectangle((int)PAINEL_X, (int)PAINEL_Y, (int)larguraPainel, (int)PAINEL_H, (Color){4, 6, 12, 225});
    DrawRectangleLinesEx((Rectangle){PAINEL_X, PAINEL_Y, larguraPainel, PAINEL_H}, 1.5f, (Color){91, 43, 55, 230});
    DrawText("ITENS", 28, 569, 15, RAYWHITE);
    DrawText("Z/X: TROCAR", 88, 572, 10, LIGHTGRAY);

    for (int i = 0; i < quantidadeVisivel; i++) desenharItem(inventario, inicio + i, i);
    const ObjetoInventario *selecionado = inventarioObjetoSelecionado(inventario);

    if (selecionado && selecionado->id == ID_KIT_MEDICO && (selecionado->estado != OBJETO_USADO && selecionado->estado != OBJETO_INSTALADO)) DrawText("F: USAR KIT MEDICO | Q: ATALHO", 28, 670, 10, GOLD);

    else DrawText("E: INTERAGIR NO LOCAL CORRETO", 28, 670, 10, GOLD);

    if (total > MAX_ITENS_VISIVEIS){
        int selecionadoAtual = indiceSelecionado(inventario, total);
        DrawText(TextFormat("%d/%d", selecionadoAtual + 1, total), (int)(PAINEL_X + larguraPainel - 42), 570, 10, GOLD);
    }
    EndMode2D();
}