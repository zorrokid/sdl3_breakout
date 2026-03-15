#ifndef BRICKS_H
#define BRICKS_H

#include "common.h"
#include <SDL3/SDL.h>

#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 75.0f
#define BRICK_HEIGHT 20.0f
#define BRICK_PADDING 5.0f

typedef struct Brick {
  SDL_FRect rect;
  bool active;
} Brick;

struct GameContext;

void init_bricks(Brick *bricks);
void render_bricks(SDL_Renderer *renderer, Brick *bricks);

void check_ball_brick_collision(struct GameContext *ctx,
                                BrickHitCallback on_brick_hit);

bool check_win_condition(Brick *bricks);
#endif // BRICKS_H
