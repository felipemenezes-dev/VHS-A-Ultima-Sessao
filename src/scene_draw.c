#include "scene_draw.h"
#include <math.h>
#include <string.h>

// Visual silhouettes are independent of collision contours. Each cutout retains
// the original scene's pixels; everything outside its polygon is transparent.
typedef struct {
    Texture2D texture;
    Vector2 origin;
    float depth; // foot baseline; zero is a fixed foreground surface
    int room;
} Occluder;
static Occluder layers[256];
static int layerCount;

static bool inside(Vector2 p, const Vector2 *v, int count) {
    bool hit = false;
    for (int i = 0, j = count - 1; i < count; j = i++) {
        if ((v[i].y > p.y) != (v[j].y > p.y) &&
            p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x)
            hit = !hit;
    }
    return hit;
}

static bool addLayer(const Image *sources, int room, const char *name, float depth,
                     const Vector2 *points, int count) {
    if (layerCount >= (int)(sizeof layers / sizeof *layers) || count < 3)
        return false;
    float left = WORLD_W, top = WORLD_H, right = 0, bottom = 0;
    for (int i = 0; i < count; i++) {
        left = fminf(left, points[i].x); top = fminf(top, points[i].y);
        right = fmaxf(right, points[i].x); bottom = fmaxf(bottom, points[i].y);
    }
    int x = (int)fmaxf(0, floorf(left)), y = (int)fmaxf(0, floorf(top));
    int w = (int)fminf(WORLD_W, ceilf(right)) - x;
    int h = (int)fminf(WORLD_H, ceilf(bottom)) - y;
    if (w <= 0 || h <= 0) return false;
    Image cutout = GenImageColor(w, h, BLANK);
    if (!cutout.data) return false;
    Color *pixels = cutout.data;
    const Color *original = sources[room].data;
    for (int row = 0; row < h; row++)
        for (int col = 0; col < w; col++)
            if (inside((Vector2){x + col + .5f, y + row + .5f}, points, count))
                pixels[row * w + col] = original[(y + row) * WORLD_W + x + col];
    Texture2D texture = LoadTextureFromImage(cutout);
    UnloadImage(cutout);
    if (!texture.id) {
        TraceLog(LOG_ERROR, "Falha no recorte visual: %s", name);
        return false;
    }
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    layers[layerCount++] = (Occluder){texture, {(float)x, (float)y}, depth, room};
    return true;
}

void sceneDrawUnload(void) {
    for (int i = 0; i < layerCount; i++) UnloadTexture(layers[i].texture);
    memset(layers, 0, sizeof layers);
    layerCount = 0;
}

bool sceneDrawInit(void) {
    sceneDrawUnload();
    Image sources[NUM_CENARIOS] = {0};
    bool ok = true;
    for (int i = 0; i < NUM_CENARIOS; i++) {
        sources[i] = LoadImage(rooms[i].image);
        if (!sources[i].data || sources[i].width != WORLD_W || sources[i].height != WORLD_H) {
            ok = false;
            break;
        }
        ImageFormat(&sources[i], PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }
    if (ok) {
#define ADD(room, name, depth, ...) do { \
    const Vector2 shape[] = {__VA_ARGS__}; \
    if (ok) ok = addLayer(sources, room, name, depth, shape, (int)(sizeof shape / sizeof *shape)); \
} while (0)
#include "depth_office.inc"
#include "depth_hall_bath.inc"
#include "depth_storage_projection.inc"
#undef ADD
    }
    for (int i = 0; i < NUM_CENARIOS; i++)
        if (sources[i].data) UnloadImage(sources[i]);
    if (!ok) sceneDrawUnload();
    return ok;
}

void sceneDrawJohn(Texture2D background, Texture2D john, int room, Vector2 position) {
    (void)background; // Source pixels are cached as transparent cutouts during init.
    if (!john.id || john.height <= 0) return;
    float width = JOHN_HEIGHT * john.width / john.height;
    Rectangle sprite = {position.x - width / 2, position.y - JOHN_HEIGHT, width, JOHN_HEIGHT};
    DrawTexturePro(john, (Rectangle){0, 0, john.width, john.height}, sprite, (Vector2){0, 0}, 0, WHITE);
    // Objects behind the player are already in the background. Restore only the
    // silhouette pixels of objects in front. Fixed foreground is painted last.
    for (int fixed = 0; fixed < 2; fixed++) {
        for (int i = 0; i < layerCount; i++) {
            const Occluder *f = &layers[i];
            if (f->room != room || (f->depth == 0) != fixed ||
                (f->depth > 0 && position.y >= f->depth)) continue;
            Rectangle bounds = {f->origin.x, f->origin.y, f->texture.width, f->texture.height};
            if (CheckCollisionRecs(sprite, bounds))
                DrawTextureV(f->texture, f->origin, WHITE);
        }
    }
}
