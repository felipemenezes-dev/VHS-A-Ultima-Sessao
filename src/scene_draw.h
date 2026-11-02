#ifndef VHS_SCENE_DRAW_H
#define VHS_SCENE_DRAW_H
#include "world.h"
// Requires an active Raylib graphics context. Build cutouts once, free before CloseWindow.
bool sceneDrawInit(void);
void sceneDrawUnload(void);
void sceneDrawJohn(Texture2D background, Texture2D john, int room, Vector2 feetPosition);
#endif
