#ifndef BRICKS_H
#define BRICKS_H

#include "ball.h"
#include <SDL3/SDL.h>

#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 75.0f
#define BRICK_HEIGHT 20.0f
#define BRICK_PADDING 5.0f

typedef struct {
    SDL_FRect rect;
    bool active;
} Brick;

void init_bricks(Brick *bricks);
void render_bricks(SDL_Renderer *renderer, Brick *bricks);

void check_ball_brick_collision(Ball *ball, Brick *bricks);

bool check_win_condition(Brick *bricks);
#endif // BRICKS_H
