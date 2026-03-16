#ifndef BRICKS_H
#define BRICKS_H

#include "common.h"
#include <SDL3/SDL.h>

struct GameContext;

void init_bricks(Brick *bricks);
void render_bricks(SDL_Renderer *renderer, Brick *bricks);

void check_ball_brick_collision(struct GameContext *ctx);

bool check_win_condition(Brick *bricks);
#endif // BRICKS_H
