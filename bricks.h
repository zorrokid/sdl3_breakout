#ifndef BRICKS_H
#define BRICKS_H

#include "common.h"

struct GameContext;

// void init_bricks(Brick *bricks);
void render_bricks(GameContext *ctx);
void init_bricks(GameContext *ctx);
void spawn_new_row(GameContext *ctx, int row_index);
void update_scrolling(GameContext *ctx, float dt);
void check_ball_brick_collision(struct GameContext *ctx);

bool check_win_condition(GameContext *ctx);
#endif // BRICKS_H
