#ifndef VHS_PLAYER_H
#define VHS_PLAYER_H
#include "assets.h"
#include "world.h"

typedef struct {
    Vector2 position;
    Direction direction;
    int frame;
    float animationTime;
    bool walking;
} Player;

void PlayerInit(Player *player, Vector2 position);
Vector2 PlayerUpdate(Player *player, const World *world, RoomId room, Vector2 movement, float delta);
void PlayerStopAnimation(Player *player);
void PlayerDraw(const Player *player, const Assets *assets);
#endif
