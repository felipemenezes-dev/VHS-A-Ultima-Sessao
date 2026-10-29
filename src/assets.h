#ifndef VHS_ASSETS_H
#define VHS_ASSETS_H
#include "types.h"

typedef struct {
    Texture2D rooms[ROOM_COUNT];
    Texture2D walking[VHS_DIRECTIONS][VHS_FRAMES];
    Texture2D standing[VHS_DIRECTIONS];
    Texture2D inventoryBackground;
} Assets;

bool AssetsLoad(Assets *assets);
void AssetsUnload(Assets *assets);
#endif
