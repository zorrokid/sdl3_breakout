#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Brick;
struct GameContext;

typedef void (*BrickHitCallback)(struct GameContext *ctx, struct Brick *brick);

#endif // COMMON_H
