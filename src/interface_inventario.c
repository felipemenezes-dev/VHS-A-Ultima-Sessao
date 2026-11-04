#include "interface_inventario.h"
#include <math.h>

#define LARGURA_INTERFACE 1280.0f
#define ALTURA_INTERFACE 720.0f

static const Rectangle espacos[INVENTARIO_MAX_OBJETOS] = {
    {105, 175, 285, 145},
    {410, 175, 285, 145},
    {105, 345, 285, 145},
    {410, 345, 285, 145}
};

bool iniciarTelaInventario(TelaInventario *inventario) {
    if (!inventario)
        return false;

    *inventario = (TelaInventario){0};
    return true;
}

void liberarTelaInventario(TelaInventario *inventario) {
    if (!inventario)
        return;

    inventarioLimpar(&inventario->dados);
    *inventario = (TelaInventario){0};
}

bool atualizarTelaInventario(TelaInventario *inventario, bool teclaTab, bool teclaEsc) {
    if (!inventario)
        return false;

    bool estavaAberto = inventario->aberto;

    if (teclaEsc)
        inventario->aberto = false;
    else if (teclaTab)
        inventario->aberto = !inventario->aberto;

    return estavaAberto || inventario->aberto || teclaTab;
}

void selecionarEspacoInventario(
    TelaInventario *inventario,
    int passoHorizontal,
    int passoVertical,
    bool clique,
    Vector2 cursor,
    int largura,
    int altura
) {
    if (!inventario || !inventario->aberto || largura <= 0 || altura <= 0)
        return;

    int selecionado = inventario->selecionado;

    if (passoHorizontal < 0)
        selecionado = (selecionado % 2 == 0) ? selecionado + 1 : selecionado - 1;
    else if (passoHorizontal > 0)
        selecionado = (selecionado % 2 == 0) ? selecionado + 1 : selecionado - 1;

    if (passoVertical < 0)
        selecionado = selecionado < 2 ? selecionado + 2 : selecionado - 2;
    else if (passoVertical > 0)
        selecionado = selecionado < 2 ? selecionado + 2 : selecionado - 2;

    if (clique) {
        float escala = fminf(
            (float)largura / LARGURA_INTERFACE,
            (float)altura / ALTURA_INTERFACE
        );

        float deslocamentoX = (largura - LARGURA_INTERFACE * escala) / 2.0f;
        float deslocamentoY = (altura - ALTURA_INTERFACE * escala) / 2.0f;

        cursor.x = (cursor.x - deslocamentoX) / escala;
        cursor.y = (cursor.y - deslocamentoY) / escala;

        for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++) {
            if (CheckCollisionPointRec(cursor, espacos[i])) {
                selecionado = i;
                break;
            }
        }
    }

    inventario->selecionado = selecionado;
}

static void desenharTextoCentralizado(
    const char *texto,
    Rectangle area,
    float tamanhoFonte,
    Color cor
) {
    Vector2 tamanho = MeasureTextEx(GetFontDefault(), texto, tamanhoFonte, 1);

    if (tamanho.x <= 0 || tamanho.y <= 0)
        return;

    float escala = fminf(
        1.0f,
        fminf(area.width / tamanho.x, area.height / tamanho.y)
    );

    Vector2 posicao = {
        area.x + (area.width - tamanho.x * escala) / 2.0f,
        area.y + (area.height - tamanho.y * escala) / 2.0f
    };

    DrawTextEx(GetFontDefault(), texto, posicao, tamanhoFonte * escala, escala, cor);
}

static void desenharEspaco(const TelaInventario *inventario, int indice) {
    Rectangle area = espacos[indice];
    Color borda = indice == inventario->selecionado
        ? (Color){237, 206, 143, 255}
        : (Color){91, 51, 63, 255};

    DrawRectangleRec(area, (Color){5, 7, 13, 235});
    DrawRectangleLinesEx(area, 2, borda);

    const ObjetoInventario *objeto = &inventario->dados.objetos[indice];

    if (objeto->estado == OBJETO_VAZIO) {
        desenharTextoCentralizado(
            "ESPACO VAZIO",
            (Rectangle){area.x + 15, area.y + 35, area.width - 30, 42},
            20,
            GRAY
        );
        return;
    }

    desenharTextoCentralizado(
        objeto->nome,
        (Rectangle){area.x + 15, area.y + 30, area.width - 30, 48},
        22,
        RAYWHITE
    );

    desenharTextoCentralizado(
        objeto->estado == OBJETO_USADO ? "UTILIZADO" : "GUARDADO",
        (Rectangle){area.x + 15, area.y + 88, area.width - 30, 28},
        17,
        borda
    );
}

static void desenharDetalhes(const TelaInventario *inventario) {
    const ObjetoInventario *objeto =
        &inventario->dados.objetos[inventario->selecionado];

    DrawText("DETALHES DO ITEM", 770, 178, 23, RAYWHITE);
    DrawRectangle(770, 215, 390, 2, (Color){116, 44, 57, 255});

    if (objeto->estado == OBJETO_VAZIO) {
        DrawText("Nenhum item selecionado.", 790, 250, 19, LIGHTGRAY);
        DrawText("Explore os cenarios para encontrar objetos.", 790, 285, 16, GRAY);
        return;
    }

    desenharTextoCentralizado(
        objeto->nome,
        (Rectangle){790, 245, 350, 55},
        25,
        RAYWHITE
    );

    DrawTextEx(
        GetFontDefault(),
        objeto->descricao,
        (Vector2){790, 325},
        19,
        1,
        LIGHTGRAY
    );

    DrawText(
        objeto->consumivel ? "ITEM CONSUMIVEL" : "ITEM PERMANENTE",
        790,
        475,
        17,
        GOLD
    );

    DrawText(
        objeto->estado == OBJETO_USADO ? "ESTADO: UTILIZADO" : "ESTADO: GUARDADO",
        790,
        510,
        17,
        GOLD
    );
}

void desenharInventario(const TelaInventario *inventario, int largura, int altura) {
    if (!inventario || !inventario->aberto || largura <= 0 || altura <= 0)
        return;

    float escala = fminf(
        (float)largura / LARGURA_INTERFACE,
        (float)altura / ALTURA_INTERFACE
    );

    Camera2D camera = {
        .offset = {
            (largura - LARGURA_INTERFACE * escala) / 2.0f,
            (altura - ALTURA_INTERFACE * escala) / 2.0f
        },
        .target = {0, 0},
        .rotation = 0,
        .zoom = escala
    };

    DrawRectangle(0, 0, largura, altura, Fade(BLACK, 0.88f));
    BeginMode2D(camera);

    DrawRectangle(48, 34, 1184, 652, (Color){13, 10, 17, 250});
    DrawRectangleLinesEx(
        (Rectangle){48, 34, 1184, 652},
        3,
        (Color){116, 44, 57, 255}
    );

    DrawText("INVENTARIO", 100, 70, 36, RAYWHITE);
    DrawText("OBJETOS COLETADOS", 105, 130, 22, LIGHTGRAY);
    DrawText(
        TextFormat("%d / %d OBJETOS", inventarioContarObjetos(&inventario->dados), INVENTARIO_MAX_OBJETOS),
        480,
        133,
        18,
        GOLD
    );

    for (int i = 0; i < INVENTARIO_MAX_OBJETOS; i++)
        desenharEspaco(inventario, i);

    DrawRectangle(750, 145, 1, 465, (Color){116, 44, 57, 255});
    desenharDetalhes(inventario);

    DrawText("SETAS / WASD / CLIQUE: SELECIONAR", 105, 590, 17, LIGHTGRAY);
    DrawText("TAB: FECHAR", 790, 590, 17, RAYWHITE);

    EndMode2D();
}
