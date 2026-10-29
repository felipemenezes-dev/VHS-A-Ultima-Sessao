#include "assets.h"
#include <stdio.h>
#include <string.h>

static void UnloadIfValid(Texture2D *texture) {
    if (texture->id) UnloadTexture(*texture);
    *texture = (Texture2D){0};
}

void AssetsUnload(Assets *assets) {
    for (int r = 0; r < ROOM_COUNT; r++) UnloadIfValid(&assets->rooms[r]);
    for (int d = 0; d < VHS_DIRECTIONS; d++) {
        UnloadIfValid(&assets->standing[d]);
        for (int f = 0; f < VHS_FRAMES; f++) UnloadIfValid(&assets->walking[d][f]);
    }
    UnloadIfValid(&assets->inventoryBackground);
}

static bool Load(Texture2D *texture, const char *path) {
    *texture = LoadTexture(path);
    if (!texture->id) {
        TraceLog(LOG_ERROR, "Nao foi possivel carregar: %s", path);
        return false;
    }
    SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
    return true;
}

bool AssetsLoad(Assets *assets) {
    memset(assets, 0, sizeof(*assets));
    const char *rooms[] = {"salao_principal", "banheiro", "escritorio", "estoque", "sala_projecao"};
    const char *directions[] = {"baixo", "cima", "direita", "esquerda"};
    char path[256];
    for (int r = 0; r < ROOM_COUNT; r++) {
        snprintf(path, sizeof(path), "assets/cenarios/%s.png", rooms[r]);
        if (!Load(&assets->rooms[r], path)) goto failure;
    }
    for (int d = 0; d < VHS_DIRECTIONS; d++) {
        snprintf(path, sizeof(path), "assets/personagens/john/%s/john_parado_%s.png", directions[d], directions[d]);
        if (!Load(&assets->standing[d], path)) goto failure;
        for (int f = 0; f < VHS_FRAMES; f++) {
            snprintf(path, sizeof(path), "assets/personagens/john/%s/john_andando_%s%d.png", directions[d], directions[d], f + 1);
            if (!Load(&assets->walking[d][f], path)) goto failure;
        }
    }
    if (!Load(&assets->inventoryBackground, "assets/ui/inventario_fundo.png")) goto failure;
    return true;
failure:
    AssetsUnload(assets);
    return false;
}
