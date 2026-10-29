#include "game.h"
#include "inventory_ui.h"
#include "world.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

bool GameInit(Game *game) {
    memset(game, 0, sizeof(*game));
    if (!AssetsLoad(&game->assets)) return false;
    if (!WorldInit(&game->world)) { AssetsUnload(&game->assets); return false; }
    game->canvas = LoadRenderTexture(VHS_WIDTH, VHS_HEIGHT);
    if (!game->canvas.id) { GameUnload(game); return false; }
    SetTextureFilter(game->canvas.texture, TEXTURE_FILTER_POINT);
    game->room = ROOM_HALL;
    PlayerInit(&game->player, RoomsGet(game->room)->spawn);
    return true;
}

void GameUnload(Game *game) {
    if (game->canvas.id) UnloadRenderTexture(game->canvas);
    WorldUnload(&game->world);
    AssetsUnload(&game->assets);
    memset(game, 0, sizeof(*game));
}

void GameStep(Game *game, GameInput input, float delta) {
    if (game->exitRequested) return;
    if (input.toggleInventory) {
        game->inventoryOpen = !game->inventoryOpen;
        return;
    }
    if (game->inventoryOpen) {
        if (input.escape) game->inventoryOpen = false;
        return;
    }
    if (input.escape) { game->exitRequested = true; return; }
    if (input.toggleDebug) game->debugCollisions = !game->debugCollisions;
    game->debugMouse = input.mouse;
    game->debugMouseValid = input.mouseValid;
    if (game->debugCollisions && input.debugClick && input.mouseValid) {
        printf("Ponto: X = %.2f%%, Y = %.2f%% | Pixel virtual: %.0f, %.0f\n",
               input.mouse.x/VHS_WIDTH*100, input.mouse.y/VHS_HEIGHT*100, input.mouse.x, input.mouse.y);
        fflush(stdout);
    }
    if (!isfinite(delta) || delta < 0) delta = 0;
    if (delta > .1f) delta = .1f;
    Vector2 movement = PlayerUpdate(&game->player, &game->world, game->room, input.movement, delta);
    if (WorldTransition(&game->world, &game->room, &game->player.position, movement, &game->player.direction)) {
        PlayerStopAnimation(&game->player);
    }
}

void GameUpdate(Game *game, float delta) {
    GameInput input = {0};
    input.movement.x = (IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT)) - (IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT));
    input.movement.y = (IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN)) - (IsKeyDown(KEY_W)||IsKeyDown(KEY_UP));
    input.toggleInventory = IsKeyPressed(KEY_TAB);
    input.escape = IsKeyPressed(KEY_ESCAPE);
    input.toggleDebug = IsKeyPressed(KEY_F1);
    float scale = fminf((float)GetScreenWidth()/VHS_WIDTH, (float)GetScreenHeight()/VHS_HEIGHT);
    if (scale > 0) {
        Vector2 mouse = GetMousePosition();
        input.mouse = (Vector2){(mouse.x-(GetScreenWidth()-VHS_WIDTH*scale)/2)/scale,
                               (mouse.y-(GetScreenHeight()-VHS_HEIGHT*scale)/2)/scale};
        input.mouseValid = input.mouse.x >= 0 && input.mouse.x < VHS_WIDTH && input.mouse.y >= 0 && input.mouse.y < VHS_HEIGHT;
        input.debugClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }
    GameStep(game, input, delta);
}

void GameDraw(const Game *game) {
    BeginTextureMode(game->canvas);
    ClearBackground(BLACK);
    Texture2D background = game->assets.rooms[game->room];
    DrawTexturePro(background, (Rectangle){0,0,(float)background.width,(float)background.height},
                   (Rectangle){0,0,VHS_WIDTH,VHS_HEIGHT}, (Vector2){0}, 0, WHITE);
    PlayerDraw(&game->player, &game->assets);
    if (game->debugCollisions) WorldDrawDebug(&game->world, game->room, game->player.position);
    DrawRectangle(24,20,370,34,Fade(BLACK,.76f));
    DrawText(RoomsGet(game->room)->name, 35, 28, 18, RAYWHITE);
    DrawRectangle(24,670,215,28,Fade(BLACK,.76f));
    DrawText("[TAB] Inventário", 35, 676, 17, RAYWHITE);
    if (game->debugCollisions) {
        DrawRectangle(24,58,690,55,Fade(BLACK,.85f));
        DrawText("F1: ocultar colisões | Clique: registrar ponto no terminal",35,65,17,RAYWHITE);
        if (game->debugMouseValid) {
            DrawText(TextFormat("Mouse: X %.2f%% | Y %.2f%%",game->debugMouse.x/VHS_WIDTH*100,game->debugMouse.y/VHS_HEIGHT*100),35,88,17,YELLOW);
            DrawCircleV(game->debugMouse,4,YELLOW);
        }
    }
    if (game->inventoryOpen) InventoryDraw(&game->assets);
    EndTextureMode();
    float scale = fminf((float)GetScreenWidth()/VHS_WIDTH, (float)GetScreenHeight()/VHS_HEIGHT);
    Rectangle target = {(GetScreenWidth()-VHS_WIDTH*scale)/2,(GetScreenHeight()-VHS_HEIGHT*scale)/2,VHS_WIDTH*scale,VHS_HEIGHT*scale};
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(game->canvas.texture, (Rectangle){0,0,VHS_WIDTH,-VHS_HEIGHT}, target, (Vector2){0}, 0, WHITE);
    EndDrawing();
}
