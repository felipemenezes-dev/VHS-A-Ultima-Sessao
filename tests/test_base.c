// Regression test of the actual game loop with real raylib and scripted inputs.
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static Texture2D TestLoadTexture(const char *path);
static bool TestIsKeyDown(int key);
static float TestGetFrameTime(void);
static bool TestWindowShouldClose(void);
static void TestDrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest,
                              Vector2 origin, float rotation, Color tint);
static void TestSetTargetFPS(int fps);
static void TestSetConfigFlags(unsigned int flags);

#define main GameMain
#define LoadTexture TestLoadTexture
#define IsKeyDown TestIsKeyDown
#define GetFrameTime TestGetFrameTime
#define WindowShouldClose TestWindowShouldClose
#define DrawTexturePro TestDrawTexturePro
#define SetTargetFPS TestSetTargetFPS
#define SetConfigFlags TestSetConfigFlags
#include "../src/main.c"
#undef main
#undef LoadTexture
#undef IsKeyDown
#undef GetFrameTime
#undef WindowShouldClose
#undef DrawTexturePro
#undef SetTargetFPS
#undef SetConfigFlags

static unsigned int backgrounds[5], animationIds[16], standingIds[4];
static int room = -1, roomMask, frame, stage, keys, transitions, spriteLoads;
static int directionsMask, animationMask, standingMask;
static bool failed;
static float x, y;
static const int expectedRooms[] = {0, 1, 0, 2, 0, 3, 4, 3, 0};

static Texture2D TestLoadTexture(const char *path) {
    Texture2D texture = LoadTexture(path);
    const char *names[] = {"salao_principal.png", "banheiro.png", "escritorio.png", "estoque.png", "sala_projecao.png"};
    const char *directions[] = {"baixo", "cima", "direita", "esquerda"};
    for (int i = 0; i < 5; i++) {
        if (strstr(path, names[i])) backgrounds[i] = texture.id;
    }
    if (strstr(path, "assets/personagens/john/")) spriteLoads++;
    for (int d = 0; d < 4; d++) {
        char suffix[80];
        snprintf(suffix, sizeof(suffix), "john_parado_%s.png", directions[d]);
        if (strstr(path, suffix)) standingIds[d] = texture.id;
        for (int f = 0; f < 4; f++) {
            snprintf(suffix, sizeof(suffix), "john_andando_%s%d.png", directions[d], f + 1);
            if (strstr(path, suffix)) animationIds[d * 4 + f] = texture.id;
        }
    }
    if (!texture.id) failed = true;
    return texture;
}

static void TestSetTargetFPS(int fps) { (void)fps; SetTargetFPS(0); }
static void TestSetConfigFlags(unsigned int flags) { SetConfigFlags(flags | FLAG_WINDOW_HIDDEN); }
static float TestGetFrameTime(void) { return 1.0f / 60.0f; }
static bool TestIsKeyDown(int key) {
    return (key == KEY_A && (keys & 1)) || (key == KEY_D && (keys & 2)) ||
           (key == KEY_W && (keys & 4)) || (key == KEY_S && (keys & 8));
}

static bool moveTo(float tx, float ty) {
    float dx = tx * (float)GetScreenWidth() - x;
    float dy = ty * (float)GetScreenHeight() - y;
    keys = 0;
    if (fabsf(dx) > 5.0f) keys |= dx < 0 ? 1 : 2;
    if (fabsf(dy) > 5.0f) keys |= dy < 0 ? 4 : 8;
    directionsMask |= keys;
    return keys == 0;
}

static bool TestWindowShouldClose(void) {
    keys = 0;
    if (failed || ++frame > 4000 || stage == 11) return true;
    if (room < 0) return false;
    switch (stage) {
        case 0: if (moveTo(.50f, .50f)) stage++; break;
        case 1: if (room == BANHEIRO) stage++; else moveTo(.02f, .50f); break;
        case 2: if (room == SALAO) stage++; else moveTo(.96f, .68f); break;
        case 3: if (moveTo(.30f, .53f)) stage++; break;
        case 4: if (room == ESCRITORIO) stage++; else moveTo(.30f, .10f); break;
        case 5: if (room == SALAO) stage++; else moveTo(.50f, .94f); break;
        case 6: if (moveTo(.70f, .30f)) stage++; break;
        case 7: if (room == ESTOQUE) stage++; else moveTo(.70f, .10f); break;
        case 8: if (room == SALA_PROJECAO) stage++; else moveTo(.50f, .15f); break;
        case 9: if (room == ESTOQUE) stage++; else moveTo(.50f, .94f); break;
        case 10: if (room == SALAO) stage++; else moveTo(.50f, .94f); break;
    }
    return false;
}

static void TestDrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest,
                              Vector2 origin, float rotation, Color tint) {
    bool background = false;
    for (int i = 0; i < 5; i++) {
        if (texture.id != backgrounds[i]) continue;
        background = true;
        roomMask |= 1 << i;
        if (room != i) {
            printf("TESTE: sala %d -> %d\n", room, i);
            if (transitions >= 9 || expectedRooms[transitions] != i) failed = true;
            transitions++;
            room = i;
        }
        break;
    }
    if (!background) {
        x = dest.x + dest.width * .5f;
        y = dest.y + dest.height;
        if (!isfinite(x) || !isfinite(y) || x < 0 || y < 0 ||
            x > GetScreenWidth() || y > GetScreenHeight()) failed = true;
        for (int i = 0; i < 16; i++) if (texture.id == animationIds[i]) animationMask |= 1 << i;
        for (int i = 0; i < 4; i++) if (texture.id == standingIds[i]) standingMask |= 1 << i;
    }
    DrawTexturePro(texture, source, dest, origin, rotation, tint);
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    char *args[] = {"teste-base", "--windowed", NULL};
    int result = GameMain(2, args);
    bool passed = result == 0 && !failed && stage == 11 && roomMask == 31 &&
                  transitions == 9 && spriteLoads == 20 && directionsMask == 15 &&
                  animationMask == 65535 && standingMask == 15;
    FilePathList files = LoadDirectoryFilesEx("assets", ".png", true);
    unsigned int imageCount = files.count;
    passed = passed && imageCount > 0;
    for (unsigned int i = 0; i < files.count; i++) {
        Image image = LoadImage(files.paths[i]);
        if (image.data == NULL || image.width <= 0 || image.height <= 0) {
            fprintf(stderr, "TESTE: imagem invalida: %s\n", files.paths[i]);
            passed = false;
        }
        UnloadImage(image);
    }
    UnloadDirectoryFiles(files);
    printf("TESTE: resultado=%s quadros=%d salas=%d entradas=%d sprites=%d direcoes=%d animacoes=%d poses=%d imagens=%u\n",
           passed ? "APROVADO" : "FALHOU", frame, roomMask, transitions, spriteLoads,
           directionsMask, animationMask, standingMask, imageCount);
    return passed ? 0 : 1;
}
