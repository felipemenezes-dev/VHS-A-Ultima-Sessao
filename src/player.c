#include "player.h"
#include "world.h"
#include <math.h>

void PlayerInit(Player *player, Vector2 position) {
    *player = (Player){.position = position, .direction = DOWN};
}

void PlayerStopAnimation(Player *player) {
    player->walking = false;
    player->frame = 0;
    player->animationTime = 0;
}

Vector2 PlayerUpdate(Player *player, const World *world, RoomId room, Vector2 movement, float delta) {
    float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
    if (length <= 0 || !isfinite(length)) {
        PlayerStopAnimation(player);
        return (Vector2){0};
    }
    movement.x /= length;
    movement.y /= length;
    if (fabsf(movement.y) > fabsf(movement.x)) player->direction = movement.y < 0 ? UP : DOWN;
    else player->direction = movement.x < 0 ? LEFT : RIGHT;
    Vector2 previous = player->position;
    player->position = WorldMove(world, room, previous, (Vector2){movement.x * 252 * delta, movement.y * 252 * delta});
    player->walking = fabsf(player->position.x - previous.x) + fabsf(player->position.y - previous.y) > .001f;
    if (player->walking) {
        player->animationTime += delta;
        while (player->animationTime >= .11f) {
            player->animationTime -= .11f;
            player->frame = (player->frame + 1) % VHS_FRAMES;
        }
    } else PlayerStopAnimation(player);
    return movement;
}

void PlayerDraw(const Player *player, const Assets *assets) {
    Texture2D texture = player->walking ? assets->walking[player->direction][player->frame] : assets->standing[player->direction];
    const float height = 108;
    float width = height * (float)texture.width / texture.height;
    DrawTexturePro(texture, (Rectangle){0,0,(float)texture.width,(float)texture.height},
                   (Rectangle){player->position.x-width/2,player->position.y-height,width,height},
                   (Vector2){0}, 0, WHITE);
}
