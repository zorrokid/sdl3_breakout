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

/**
 * Translates a Screen Y coordinate into the physical array row index.
 * Returns -1 if the Y coordinate is outside the logical grid range.
 */
int get_brick_array_row(const BrickManager *bm, float screen_y);
/**
 * Translates a Screen X coordinate into the brick column index.
 * Returns -1 if the X coordinate is outside the logical grid range.
 */
int get_brick_col(float screen_x);
/**
 * Returns a pointer to the brick at a specific screen coordinate.
 * Returns NULL if the coordinate is out of bounds or the brick is inactive.
 */
Brick *get_brick_at_screen_pos(GameContext *ctx, float x, float y);

bool check_win_condition(GameContext *ctx);
#endif // BRICKS_H
