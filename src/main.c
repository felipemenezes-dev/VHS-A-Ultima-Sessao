#include "game.h"

int main(int argc, char **argv) {
    bool windowed = argc <= 1 || !TextIsEqual(argv[1], "--fullscreen");
    SetConfigFlags(windowed ? FLAG_WINDOW_RESIZABLE : FLAG_FULLSCREEN_MODE);
    InitWindow(windowed ? VHS_WIDTH : 0, windowed ? VHS_HEIGHT : 0, "VHS - A Ultima Sessao");
    if (!IsWindowReady()) return 1;
    // Escape closes the overlay first; the game decides when to request exit.
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    Game game;
    if (!GameInit(&game)) { CloseWindow(); return 1; }
    while (!WindowShouldClose() && !game.exitRequested) {
        GameUpdate(&game, GetFrameTime());
        GameDraw(&game);
    }
    GameUnload(&game);
    CloseWindow();
    return 0;
}
