#ifndef VHS_WORLD_H
#define VHS_WORLD_H
#include "rooms.h"

typedef struct {
    unsigned int *sums;
    int width, height;
    Texture2D maskView;
} World;

bool WorldInit(World *world);
void WorldUnload(World *world);
Rectangle WorldFeet(Vector2 position);
bool WorldBlocked(const World *world, RoomId room, Vector2 position);
Vector2 WorldMove(const World *world, RoomId room, Vector2 position, Vector2 displacement);
bool WorldTransition(const World *world, RoomId *room, Vector2 *position, Vector2 movement, Direction *direction);
void WorldDrawDebug(const World *world, RoomId room, Vector2 position);
#endif
