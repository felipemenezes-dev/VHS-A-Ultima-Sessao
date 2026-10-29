#ifndef VHS_ROOMS_H
#define VHS_ROOMS_H
#include "types.h"
#include <stddef.h>

typedef struct {
    Rectangle trigger;
    RoomId destination;
    Vector2 arrival;
    Direction movementDirection;
    Direction arrivalDirection;
} Door;

typedef struct {
    const char *name;
    const Rectangle *obstacles;
    size_t obstacleCount;
    const Door *doors;
    size_t doorCount;
    Vector2 spawn;
} Room;

const Room *RoomsGet(RoomId id);
#endif
