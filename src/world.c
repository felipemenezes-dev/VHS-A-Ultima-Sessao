#include "world.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

bool WorldInit(World *world) {
    memset(world, 0, sizeof(*world));
    Image image = LoadImage("assets/cenarios/colisoes_salao.png");
    if (!image.data || image.width <= 0 || image.height <= 0 || image.width > 8192 || image.height > 8192) {
        if (image.data) UnloadImage(image);
        TraceLog(LOG_ERROR, "Mascara de colisao do salao ausente ou invalida.");
        return false;
    }
    world->width = image.width;
    world->height = image.height;
    size_t stride = (size_t)image.width + 1;
    world->sums = calloc(stride * ((size_t)image.height + 1), sizeof(*world->sums));
    Color *pixels = LoadImageColors(image);
    if (!world->sums || !pixels) {
        if (pixels) UnloadImageColors(pixels);
        UnloadImage(image);
        WorldUnload(world);
        return false;
    }
    // Integral alpha map: exactly the upstream alpha > 0 rule, with O(1) rectangle checks.
    for (int y = 0; y < image.height; y++) {
        unsigned int row = 0;
        for (int x = 0; x < image.width; x++) {
            row += pixels[(size_t)y * image.width + x].a > 0;
            world->sums[((size_t)y+1)*stride+x+1] = world->sums[(size_t)y*stride+x+1] + row;
        }
    }
    UnloadImageColors(pixels);
    world->maskView = LoadTextureFromImage(image);
    UnloadImage(image);
    if (!world->maskView.id) { WorldUnload(world); return false; }
    SetTextureFilter(world->maskView, TEXTURE_FILTER_POINT);
    return true;
}

void WorldUnload(World *world) {
    free(world->sums);
    if (world->maskView.id) UnloadTexture(world->maskView);
    memset(world, 0, sizeof(*world));
}

Rectangle WorldFeet(Vector2 position) { return (Rectangle){position.x - 12, position.y - 12, 24, 12}; }

static bool MaskBlocked(const World *world, Rectangle feet) {
    if (!world || !world->sums) return true;
    float scaleX = (float)world->width / VHS_WIDTH;
    float scaleY = (float)world->height / VHS_HEIGHT;
    int x0 = (int)floorf(feet.x*scaleX), y0 = (int)floorf(feet.y*scaleY);
    int x1 = (int)ceilf((feet.x+feet.width)*scaleX), y1 = (int)ceilf((feet.y+feet.height)*scaleY);
    if (x0 < 0 || y0 < 0 || x1 > world->width || y1 > world->height) return true;
    size_t stride = (size_t)world->width + 1;
    unsigned int count = world->sums[(size_t)y1*stride+x1] - world->sums[(size_t)y0*stride+x1]
                       - world->sums[(size_t)y1*stride+x0] + world->sums[(size_t)y0*stride+x0];
    return count > 0;
}

bool WorldBlocked(const World *world, RoomId room, Vector2 position) {
    const Room *definition = RoomsGet(room);
    if (!definition || !isfinite(position.x) || !isfinite(position.y)) return true;
    Rectangle feet = WorldFeet(position);
    if (feet.x < 0 || feet.y < 0 || feet.x + feet.width > VHS_WIDTH || position.y > VHS_HEIGHT) return true;
    if (room == ROOM_HALL) return MaskBlocked(world, feet);
    for (size_t i = 0; i < definition->obstacleCount; i++) {
        if (CheckCollisionRecs(feet, definition->obstacles[i])) return true;
    }
    return false;
}

Vector2 WorldMove(const World *world, RoomId room, Vector2 position, Vector2 displacement) {
    if (!isfinite(displacement.x) || !isfinite(displacement.y) || WorldBlocked(world, room, position)) return position;
    float distance = fmaxf(fabsf(displacement.x), fabsf(displacement.y));
    if (distance > 16384.0f) return position;
    int steps = (int)ceilf(distance / 4.0f);
    if (steps < 1) return position;
    Vector2 step = {displacement.x / steps, displacement.y / steps};
    for (int i = 0; i < steps; i++) {
        Vector2 candidate = {position.x + step.x, position.y};
        if (!WorldBlocked(world, room, candidate)) position.x = candidate.x;
        candidate = (Vector2){position.x, position.y + step.y};
        if (!WorldBlocked(world, room, candidate)) position.y = candidate.y;
    }
    return position;
}

static bool Toward(Direction direction, Vector2 movement) {
    switch (direction) {
        case LEFT: return movement.x < 0;
        case RIGHT: return movement.x > 0;
        case UP: return movement.y < 0;
        case DOWN: return movement.y > 0;
    }
    return false;
}

bool WorldTransition(const World *world, RoomId *room, Vector2 *position, Vector2 movement, Direction *direction) {
    const Room *definition = RoomsGet(*room);
    if (!definition) return false;
    for (size_t i = 0; i < definition->doorCount; i++) {
        const Door *door = &definition->doors[i];
        if (Toward(door->movementDirection, movement) && CheckCollisionPointRec(*position, door->trigger)) {
            if (WorldBlocked(world, door->destination, door->arrival)) return false;
            *room = door->destination;
            *position = door->arrival;
            *direction = door->arrivalDirection;
            return true;
        }
    }
    return false;
}

void WorldDrawDebug(const World *world, RoomId room, Vector2 position) {
    const Room *definition = RoomsGet(room);
    if (room == ROOM_HALL) {
        Texture2D texture = world->maskView;
        DrawTexturePro(texture, (Rectangle){0,0,(float)texture.width,(float)texture.height},
                       (Rectangle){0,0,VHS_WIDTH,VHS_HEIGHT}, (Vector2){0},0,(Color){26,214,123,220});
    } else {
        for (size_t i = 0; i < definition->obstacleCount; i++) {
            DrawRectangleRec(definition->obstacles[i], Fade(RED, .25f));
            DrawRectangleLinesEx(definition->obstacles[i], 1, RED);
        }
    }
    for (size_t i = 0; i < definition->doorCount; i++) DrawRectangleRec(definition->doors[i].trigger, Fade(GREEN, .3f));
    DrawRectangleLinesEx(WorldFeet(position), 2, SKYBLUE);
}
