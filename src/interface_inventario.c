#include "interface_inventario.h"
#include <math.h>

static const Rectangle espacos[INVENTARIO_SLOTS] = {
    {90, 178, 204, 148},  {316, 178, 204, 148}, {542, 178, 204, 148}, {90, 416, 148, 140},
    {259, 416, 148, 140}, {428, 416, 148, 140}, {597, 416, 148, 140}};

bool iniciarTelaInventario(TelaInventario *inventario) {
    *inventario = (TelaInventario){0};
    inventario->fundo = LoadTexture("assets/ui/inventario_fundo.png");
    if (!inventario->fundo.id)
        return false;
    SetTextureFilter(inventario->fundo, TEXTURE_FILTER_POINT);
    return true;
}

void liberarTelaInventario(TelaInventario *inventario) {
    if (inventario->fundo.id)
        UnloadTexture(inventario->fundo);
    inventarioLimpar(&inventario->dados);
    *inventario = (TelaInventario){0};
}

bool atualizarTelaInventario(TelaInventario *inventario, bool tab, bool esc) {
    bool estavaAberto = inventario->aberto;
    if (esc)
        inventario->aberto = false;
    else if (tab)
        inventario->aberto = !inventario->aberto;
    return estavaAberto || inventario->aberto || tab;
}

void selecionarEspacoInventario(TelaInventario *inventario, int passoX, int passoY, bool clique,
                                Vector2 cursor, int largura, int altura) {
    if (!inventario->aberto || largura <= 0 || altura <= 0)
        return;
    int selecionado = inventario->selecionado;
    int inicio = selecionado < 3 ? 0 : 3;
    int fim = selecionado < 3 ? 2 : 6;

    if (passoX < 0)
        selecionado--;
    if (passoX > 0)
        selecionado++;
    if (selecionado < inicio)
        selecionado = fim;
    if (selecionado > fim)
        selecionado = inicio;
    if (passoY > 0 && selecionado < 3)
        selecionado += 3;
    else if (passoY < 0 && selecionado >= 3) {
        selecionado -= 3;
        if (selecionado > 2)
            selecionado = 2;
    }

    if (clique) {
        float escala = fminf(largura / 1280.f, altura / 720.f);
        cursor.x = (cursor.x - (largura - 1280 * escala) / 2) / escala;
        cursor.y = (cursor.y - (altura - 720 * escala) / 2) / escala;
        for (int i = 0; i < INVENTARIO_SLOTS; i++)
            if (CheckCollisionPointRec(cursor, espacos[i]))
                selecionado = i;
    }
    inventario->selecionado = selecionado;
}

static void desenharTexto(const char *texto, Rectangle area, float fonte, Color cor) {
    Vector2 tamanho = MeasureTextEx(GetFontDefault(), texto, fonte, 1);
    if (tamanho.x <= 0 || tamanho.y <= 0)
        return;
    float escala = fminf(1, fminf(area.width / tamanho.x, area.height / tamanho.y));
    Vector2 posicao = {area.x + (area.width - tamanho.x * escala) / 2, area.y};
    DrawTextEx(GetFontDefault(), texto, posicao, fonte * escala, escala, cor);
}

static void desenharEspaco(const TelaInventario *inventario, int indice) {
    Rectangle area = espacos[indice];
    Color borda = {91, 51, 63, 255};
    if (indice == inventario->selecionado)
        borda = (Color){237, 206, 143, 255};
    DrawRectangleRec(area, (Color){5, 7, 13, 230});
    DrawRectangleLinesEx(area, 2, borda);

    const char *nome = "VAZIO";
    const char *detalhe = "";
    if (indice < 3) {
        const Fita *fita = fitaNaPosicao(inventario->dados.fitas, indice);
        if (fita) {
            nome = fita->nome;
            detalhe = TextFormat("%d MHz", fita->frequencia);
        }
    } else {
        const ObjetoInventario *objeto = &inventario->dados.objetos[indice - 3];
        if (objeto->estado != OBJETO_VAZIO) {
            nome = objeto->nome;
            detalhe = objeto->estado == OBJETO_USADO ? "USADO" : "GUARDADO";
        }
    }
    desenharTexto(nome, (Rectangle){area.x + 12, area.y + 55, area.width - 24, 25}, 18, RAYWHITE);
    desenharTexto(detalhe, (Rectangle){area.x + 12, area.y + 94, area.width - 24, 22}, 16, borda);
}

static void desenharDetalhes(const TelaInventario *inventario) {
    const char *nome = "Espaço vazio";
    const char *descricao = "Nenhum item neste espaço.";
    int indice = inventario->selecionado;
    if (indice < 3) {
        const Fita *fita = fitaNaPosicao(inventario->dados.fitas, indice);
        if (fita) {
            nome = fita->nome;
            descricao = TextFormat("Frequência: %d MHz", fita->frequencia);
        }
    } else {
        const ObjetoInventario *objeto = &inventario->dados.objetos[indice - 3];
        if (objeto->estado != OBJETO_VAZIO) {
            nome = objeto->nome;
            descricao = objeto->descricao;
            DrawText(objeto->estado == OBJETO_USADO ? "USADO" : "GUARDADO", 900, 480, 19, GOLD);
        }
    }
    desenharTexto(nome, (Rectangle){899, 216, 280, 28}, 22, RAYWHITE);
    desenharTexto(descricao, (Rectangle){899, 264, 280, 180}, 18, LIGHTGRAY);
}

void desenharInventario(const TelaInventario *inventario, int largura, int altura) {
    if (!inventario->aberto || largura <= 0 || altura <= 0)
        return;
    float escala = fminf(largura / 1280.f, altura / 720.f);
    Camera2D camera = {.offset = {(largura - 1280 * escala) / 2, (altura - 720 * escala) / 2},
                       .zoom = escala};
    DrawRectangle(0, 0, largura, altura, Fade(BLACK, .85f));
    BeginMode2D(camera);
    DrawTexturePro(inventario->fundo,
                   (Rectangle){0, 0, inventario->fundo.width, inventario->fundo.height},
                   (Rectangle){0, 0, 1280, 720}, (Vector2){0}, 0, WHITE);
    DrawText("INVENTÁRIO", 194, 35, 34, RAYWHITE);
    DrawText("VHS / A ÚLTIMA SESSÃO", 815, 46, 18, LIGHTGRAY);
    DrawText(TextFormat("FITAS VHS  %d / 3", contarFitas(inventario->dados.fitas)), 90, 130, 22,
             RAYWHITE);
    DrawText(TextFormat("OBJETOS  %d / 4", inventarioContarObjetos(&inventario->dados)), 90, 372,
             22, RAYWHITE);
    for (int i = 0; i < INVENTARIO_SLOTS; i++)
        desenharEspaco(inventario, i);
    DrawText("MOCHILA", 900, 132, 24, RAYWHITE);
    DrawRectangle(899, 177, 280, 2, (Color){116, 44, 57, 255});
    desenharDetalhes(inventario);
    DrawText("O jogo está pausado.", 900, 540, 17, LIGHTGRAY);
    DrawText("SETAS / WASD / CLIQUE: selecionar", 90, 589, 18, LIGHTGRAY);
    DrawText("[TAB] FECHAR", 90, 642, 20, RAYWHITE);
    DrawText("[ESC] VOLTAR AO JOGO", 870, 643, 17, LIGHTGRAY);
    EndMode2D();
}
