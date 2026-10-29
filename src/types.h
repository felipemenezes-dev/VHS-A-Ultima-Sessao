#ifndef VHS_TYPES_H
#define VHS_TYPES_H

#include "raylib.h"

#define VHS_WIDTH 1280
#define VHS_HEIGHT 720
#define VHS_DIRECTIONS 4
#define VHS_FRAMES 4

typedef enum { DOWN, UP, RIGHT, LEFT } Direction;
typedef enum { ROOM_HALL, ROOM_BATHROOM, ROOM_OFFICE, ROOM_STORAGE, ROOM_PROJECTION, ROOM_COUNT } RoomId;

#endif
