#ifndef VHS_GAME_H
#define VHS_GAME_H
#include "player.h"

typedef struct {
    Vector2 movement;
    bool toggleInventory;
    bool escape;
    bool toggleDebug;
    Vector2 mouse;
    bool mouseValid;
    bool debugClick;
} GameInput;

typedef struct {
    Assets assets;
    World world;
    RenderTexture2D canvas;
    Player player;
    RoomId room;
    bool inventoryOpen;
    bool debugCollisions;
    bool exitRequested;
    Vector2 debugMouse;
    bool debugMouseValid;
} Game;

bool GameInit(Game *game);
void GameUnload(Game *game);
void GameStep(Game *game, GameInput input, float delta);
void GameUpdate(Game *game, float delta);
void GameDraw(const Game *game);
#endif
