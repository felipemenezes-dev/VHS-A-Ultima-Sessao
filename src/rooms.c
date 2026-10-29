#include "rooms.h"

// Hand-authored footprints over the original images. N uses normalized coordinates.
// Walls are split around door openings; decorative paper/cables are not solid.
// The hall uses the upstream alpha mask instead of a second set of rectangles.
#define N(x,y,w,h) {(x)*VHS_WIDTH,(y)*VHS_HEIGHT,(w)*VHS_WIDTH,(h)*VHS_HEIGHT}
#define P(x,y) {(x)*VHS_WIDTH,(y)*VHS_HEIGHT}
#define COUNT(a) (sizeof(a)/sizeof((a)[0]))

static const Rectangle bathroom[] = {
    N(0,0,1,.33), N(0,0,.09,1), N(.875,0,.125,.51),
    N(.875,.74,.125,.26), N(0,.86,1,.14),
    N(.11,.29,.22,.32), N(.40,.32,.20,.095), N(.09,.72,.06,.09)
};
static const Rectangle office[] = {
    N(0,0,1,.25), N(0,0,.105,1), N(.895,0,.105,1),
    N(0,.82,.44,.18), N(.56,.82,.44,.18),
    N(.18,.25,.07,.07), N(.34,.20,.12,.11), N(.397,.29,.267,.145),
    N(.31,.34,.065,.10), N(.11,.26,.10,.49), N(.78,.33,.105,.40),
    N(.73,.68,.05,.07)
};
static const Rectangle storage[] = {
    N(0,0,.455,.23), N(.545,0,.455,.23), N(0,0,.08,1), N(.92,0,.08,1),
    N(0,.84,.45,.16), N(.55,.84,.45,.16),
    N(.10,.28,.06,.34), N(.235,.21,.085,.16), N(.31,.30,.108,.39),
    N(.65,.17,.18,.24), N(.85,.27,.055,.30), N(.745,.60,.11,.15),
    N(.11,.64,.17,.19), N(.85,.57,.06,.21)
};
static const Rectangle projection[] = {
    N(0,0,1,.25), N(0,0,.14,1), N(.86,0,.14,1),
    N(0,.85,.45,.15), N(.55,.85,.45,.15),
    N(.17,.22,.12,.54), N(.28,.25,.065,.20), N(.405,.30,.20,.15),
    N(.675,.46,.115,.14), N(.79,.30,.07,.49),
    N(.19,.69,.10,.11), N(.73,.73,.08,.10)
};

static const Door hallDoors[] = {
    {N(.01,.42,.06,.17), ROOM_BATHROOM, P(.80,.66), LEFT, LEFT},
    {N(.27,.09,.07,.115), ROOM_OFFICE, P(.50,.73), UP, UP},
    {N(.665,.09,.075,.115), ROOM_STORAGE, P(.50,.73), UP, UP}
};
static const Door bathroomDoors[] = {
    {N(.855,.51,.045,.23), ROOM_HALL, P(.085,.53), RIGHT, RIGHT}
};
static const Door officeDoors[] = {
    {N(.46,.84,.08,.13), ROOM_HALL, P(.30,.27), DOWN, DOWN}
};
static const Door storageDoors[] = {
    {N(.46,.09,.08,.15), ROOM_PROJECTION, P(.50,.73), UP, UP},
    {N(.46,.86,.08,.12), ROOM_HALL, P(.70,.27), DOWN, DOWN}
};
static const Door projectionDoors[] = {
    {N(.46,.86,.08,.12), ROOM_STORAGE, P(.50,.30), DOWN, DOWN}
};

static const Room rooms[] = {
    {"Salão principal", NULL, 0, hallDoors, COUNT(hallDoors), P(.50,.62)},
    {"Banheiro", bathroom, COUNT(bathroom), bathroomDoors, COUNT(bathroomDoors), P(.80,.66)},
    {"Escritório", office, COUNT(office), officeDoors, COUNT(officeDoors), P(.50,.73)},
    {"Estoque", storage, COUNT(storage), storageDoors, COUNT(storageDoors), P(.50,.73)},
    {"Sala de projeção", projection, COUNT(projection), projectionDoors, COUNT(projectionDoors), P(.50,.73)}
};

const Room *RoomsGet(RoomId id) {
    if (id < 0 || id >= ROOM_COUNT) return NULL;
    return &rooms[id];
}
