#include "world.h"
#include <math.h>

#define ROOM(n, f, x, y)                                                                                     \
    {                                                                                                        \
        n, "assets/cenarios/" f ".png", "assets/cenarios/colisoes_" f ".png", { x, y }                       \
    }
const Room rooms[NUM_CENARIOS] = {{"Salao principal",
                                   "assets/cenarios/salao_principal.png",
                                   "assets/cenarios/colisoes_salao.png",
                                   {836, 583}},
                                  ROOM("Banheiro", "banheiro", 1320, 620),
                                  ROOM("Escritorio", "escritorio", 835, 704),
                                  ROOM("Estoque", "estoque", 845, 727),
                                  ROOM("Sala de projecao", "sala_projecao", 825, 704)};

// All coordinates use the original 1672 x 941 artwork. Arrivals lie outside
// the return trigger; outward movement + cooldown prevent doorway ping-pong.
const Door doors[] = {
    {SALAO, BANHEIRO, {74, 463, 90, 98}, {1320, 620}, ESQUERDA, "Banheiro", {110, 460}, {61, 566}, {-1, 0}},
    {BANHEIRO,
     SALAO,
     {1360, 590, 70, 110},
     {185, 520},
     DIREITA,
     "Salao principal",
     {1411, 579},
     {1457, 751},
     {1, 0}},
    {SALAO, ESCRITORIO, {450, 202, 102, 60}, {835, 704}, CIMA, "Escritorio", {447, 197}, {568, 197}, {0, -1}},
    {ESCRITORIO, SALAO, {775, 756, 126, 140}, {500, 290}, BAIXO, "Salao principal", {0, 0}, {0, 0}, {0, 1}},
    {SALAO, ESTOQUE, {1125, 202, 105, 60}, {845, 727}, CIMA, "Estoque", {1117, 198}, {1229, 198}, {0, -1}},
    {ESTOQUE, SALAO, {765, 784, 165, 130}, {1174, 293}, BAIXO, "Salao principal", {0, 0}, {0, 0}, {0, 1}},
    {ESTOQUE,
     SALA_PROJECAO,
     {778, 202, 132, 67},
     {825, 704},
     CIMA,
     "Sala de projecao",
     {0, 0},
     {0, 0},
     {0, -1}},
    {SALA_PROJECAO, ESTOQUE, {773, 765, 100, 113}, {845, 305}, BAIXO, "Estoque", {0, 0}, {0, 0}, {0, 1}}};
const int doorCount = sizeof doors / sizeof *doors;

// Outer floor silhouettes, independent of furniture and visual occlusion.
// Door recesses are included and capped INSIDE their transition triggers. This
// keeps a delayed/large movement inside the room until worldTransition runs.
static const Vector2 floorHall[] = {
    {235,196}, {1390,196}, {1550,350}, {1630,735}, {1530,835},
    {978,835}, {978,728}, {700,728}, {700,835}, {132,835},
    {50,740}, {50,570}, {110,460}, {170,300}
};
static const Vector2 floorBathroom[] = {
    {230,330}, {1345,330}, {1360,550}, {1410,580}, {1450,750},
    {1450,800}, {1165,800}, {1165,845}, {1010,845}, {1010,800},
    {150,800}, {185,560}, {230,560}
};
static const Vector2 floorOffice[] = {
    {208,245}, {1458,245}, {1475,759}, {938,759}, {938,740},
    {905,740}, {905,886}, {772,886}, {772,740}, {738,740},
    {738,759}, {208,759}
};
static const Vector2 floorStorage[] = {
    {150,155}, {240,155}, {240,225}, {774,225}, {774,200},
    {912,200}, {912,225}, {1435,225}, {1435,155}, {1520,155},
    {1540,707}, {1460,707}, {1460,780}, {973,780}, {973,765},
    {935,765}, {935,896}, {760,896}, {760,765}, {724,765},
    {724,780}, {205,780}, {205,708}, {118,708}
};
static const Vector2 floorProjection[] = {
    {335,240}, {1330,240}, {1380,770}, {925,770}, {925,746},
    {904,746}, {904,868}, {754,868}, {754,746}, {730,746},
    {730,770}, {265,770}
};
#define FLOOR(points) {points, (int)(sizeof points / sizeof *points)}
const FloorBoundary roomFloors[NUM_CENARIOS] = {
    FLOOR(floorHall), FLOOR(floorBathroom), FLOOR(floorOffice),
    FLOOR(floorStorage), FLOOR(floorProjection)
};
#undef FLOOR

bool worldFloorContains(int room, Vector2 p) {
    if (room < 0 || room >= NUM_CENARIOS || !isfinite(p.x) || !isfinite(p.y)) return false;
    const FloorBoundary *floor = &roomFloors[room];
    bool inside = false;
    for (int i = 0, j = floor->count - 1; i < floor->count; j = i++) {
        Vector2 a = floor->points[i], b = floor->points[j];
        if ((a.y > p.y) != (b.y > p.y) &&
            p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)
            inside = !inside;
    }
    return inside;
}

Rectangle feet(Vector2 p) { return (Rectangle){p.x - FOOT_W / 2, p.y - FOOT_H, FOOT_W, FOOT_H}; }
Rectangle viewport(int w, int h) {
    float scale = fminf((float)w / WORLD_W, (float)h / WORLD_H);
    return (Rectangle){(w - WORLD_W * scale) / 2, (h - WORLD_H * scale) / 2, WORLD_W * scale,
                       WORLD_H * scale};
}
Vector2 screenToWorld(Vector2 p, Rectangle v) {
    return (Vector2){(p.x - v.x) * WORLD_W / v.width, (p.y - v.y) * WORLD_H / v.height};
}
Vector2 worldToScreen(Vector2 p, Rectangle v) {
    return (Vector2){v.x + p.x * v.width / WORLD_W, v.y + p.y * v.height / WORLD_H};
}
int worldDoorAt(int room, Vector2 p, Vector2 movement) {
    for (int i = 0; i < doorCount; i++) {
        const Door *d = &doors[i];
        if (d->from == room && CheckCollisionPointRec(p, d->area) &&
            movement.x * d->direction.x + movement.y * d->direction.y > 0)
            return i;
    }
    return -1;
}
bool worldTransition(int *room, Vector2 *p, int *facing, Vector2 movement, float *cooldown) {
    if (*cooldown > 0)
        return false;
    int i = worldDoorAt(*room, *p, movement);
    if (i < 0)
        return false;
    *room = doors[i].to;
    *p = doors[i].arrival;
    *facing = doors[i].facing;
    *cooldown = .25f;
    return true;
}
