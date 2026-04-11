#include "bricks.h"
#include "common.h"
#include "particles.h"
#include "ball.h"
#include <stdio.h>
#include <math.h>

// Calculate the starting x position to center the bricks on the screen
float get_brick_row_start_x() {
  float total_width = (BRICK_COLS * BRICK_TOTAL_WIDTH) - BRICK_PADDING;
  return (SCREEN_WIDTH - total_width) / 2.0f;
}

// Map the visual row index to actual array row index using the circular buffer
// rotation. E.g. if head_row is 5, then screen row 0 is actually row 5 in the
// array.
int get_actual_row_index(const BrickManager *bm, int visual_row_index) {
  return (bm->head_row + visual_row_index) % MAX_BRICK_ROWS;
}

void render_bricks(GameContext *ctx) {
  for (int i = 0; i < MAX_BRICK_ROWS; i++) {

    int actual_row_index = get_actual_row_index(&ctx->brick_manager, i);

    for (int c = 0; c < BRICK_COLS; c++) {
      Brick *brick = &ctx->brick_manager.grid[actual_row_index][c];
      if (brick->active) {
        // Calculate the position to draw the brick based on its column and row
        // index
        SDL_FRect draw_rect = brick->rect;
        draw_rect.x = get_brick_row_start_x() + c * BRICK_TOTAL_WIDTH;

        // The y position is based on the visual row index (i) and the scroll
        // offset. The topmost row (i=0) should be just above the screen, so we
        // start with -BRICK_HEIGHT
        draw_rect.y =
            ((i - 1) * BRICK_TOTAL_HEIGHT) + ctx->brick_manager.scroll_offset;

        draw_rect.w = BRICK_WIDTH;
        draw_rect.h = BRICK_HEIGHT;
        SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(ctx->renderer, &draw_rect);
      }
    }
  }
}

Coord get_brick_center(SDL_FRect brick_rect) {
  Coord center;
  center.x = brick_rect.x + (brick_rect.w / 2.0f);
  center.y = brick_rect.y + (brick_rect.h / 2.0f);
  return center;
}

CollisionSide check_ball_brick_collision_side(GameContext *ctx,
                                              Coord brick_center,
                                              Coord ball_center) {
  // Relative distance (vector from brick center to ball center)
  float dx = ball_center.x - brick_center.x;
  float dy = ball_center.y - brick_center.y;

  // Normalize the distance between center of the ball
  // and center of the brick: calculate the percentage
  // how far the ball center is from brick center

  float brick_width_half = BRICK_WIDTH / 2.0f;
  float brick_height_half = BRICK_HEIGHT / 2.0f;

  // percentage from side
  float dx_percentage = SDL_fabs(dx / brick_width_half);
  // percentage from top / bottom
  float dy_percentage = SDL_fabs(dy / brick_height_half);

  return dx_percentage > dy_percentage ? HIT_SIDE : HIT_TOP_BOTTOM;
}

void init_bricks(GameContext *ctx) {
  for (int i = 0; i < 6; i++) {
    spawn_new_row(ctx, get_actual_row_index(&ctx->brick_manager, i));
  }
}

int get_visual_brick_row(const BrickManager *bm, float screen_y) {
  // brick scroll offset need to be taken into account to map to correct visual
  // row for collision detection
  float relative_y = screen_y - bm->scroll_offset;

  // We add 1 to the visual row calculation because the topmost row starts just
  // above the screen (at -BRICK_HEIGHT).
  int visual_row = (int)SDL_floorf(relative_y / BRICK_TOTAL_HEIGHT) + 1;

  // check bounds
  if (visual_row < 0 || visual_row >= MAX_BRICK_ROWS)
    return -1;

  return visual_row;
}

void check_ball_brick_collision(GameContext *ctx) {

  Brick *brick =
      get_brick_at_screen_pos(ctx, ctx->ball.rect.x, ctx->ball.rect.y);

  if (brick) {

    int visual_row_index =
        get_visual_brick_row(&ctx->brick_manager, ctx->ball.rect.y);
    if (visual_row_index == -1)
      return; // Out of bounds, should not happen since we already got a brick
    int actual_row =
        get_actual_row_index(&ctx->brick_manager, visual_row_index);
    if (actual_row == -1)
      return; // Out of bounds, should not happen since we already got a brick

    int col = get_brick_col(ctx->ball.rect.x);

    float visual_y = ((visual_row_index - 1) * BRICK_TOTAL_HEIGHT) +
                     ctx->brick_manager.scroll_offset;

    float visual_x = get_brick_row_start_x() + col * BRICK_TOTAL_WIDTH;

    // Calculate a center of brick to center of the ball vector to detect
    // if the collision is top / bottom or side of a brick, and to use the
    // center of the brick for spawning particles.

    SDL_FRect physics_rect = {visual_x, visual_y, BRICK_WIDTH, BRICK_HEIGHT};
    Coord ball_center = get_ball_center(&ctx->ball);
    Coord brick_center = get_brick_center(physics_rect);

    CollisionSide side =
        check_ball_brick_collision_side(ctx, brick_center, ball_center);

    if (side == HIT_SIDE) {
      ctx->ball.dx *= -1.0f;
    } else {
      // top / bottom hit
      ctx->ball.dy *= -1.0f;
    }

    if (ctx->on_collision) {
      CollisionEvent event = {.type = EVENT_BRICK_HIT,
                              .position = brick_center,
                              .data.brick_hit = {.brick = brick, .side = side}};
      ctx->on_collision(ctx, &event);
    }

    brick->active = false;
  }
}

bool check_win_condition(GameContext *ctx) {
  for (int i = 0; i < MAX_BRICK_ROWS; i++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      if (ctx->brick_manager.grid[i][c].active)
        return false;
    }
  }
  return true;
}

void spawn_new_row(GameContext *ctx, int row_index) {
  printf("Spawning new row at index: %d\n", row_index);
  for (int c = 0; c < BRICK_COLS; c++) {
    Brick *brick = &ctx->brick_manager.grid[row_index][c];
    brick->active = true;
  }
  ctx->brick_manager.total_rows_spawned++;
}

void update_scrolling(GameContext *ctx, float dt) {
  // Update the scroll offset based on the scroll speed and delta time
  ctx->brick_manager.scroll_offset += ctx->brick_manager.scroll_speed * dt;

  // Reset the scroll offset and move bricks down when it exceeds the brick
  // height
  if (ctx->brick_manager.scroll_offset >= BRICK_TOTAL_HEIGHT) {
    ctx->brick_manager.scroll_offset = 0;

    // move the header pointer "up" for new row, and wrap around if it goes
    // past the top
    ctx->brick_manager.head_row--;
    printf("New head row: %d\n", ctx->brick_manager.head_row);
    if (ctx->brick_manager.head_row < 0) {
      ctx->brick_manager.head_row = MAX_BRICK_ROWS - 1;
    }

    spawn_new_row(ctx, ctx->brick_manager.head_row);
  }
}

int get_brick_array_row(const BrickManager *bm, float screen_y) {
  // This reverses the rendering math to find the visual row index:
  int visual_row = get_visual_brick_row(bm, screen_y);
  if (visual_row == -1)
    return -1; // Out of bounds
  return get_actual_row_index(bm, visual_row);
}

int get_brick_col(float screen_x) {
  int col = (int)(((screen_x - get_brick_row_start_x()) / BRICK_TOTAL_WIDTH));
  if (col < 0 || col >= BRICK_COLS)
    return -1;
  return col;
}

Brick *get_brick_at_screen_pos(GameContext *ctx, float x, float y) {
  int row = get_brick_array_row(&ctx->brick_manager, y);
  if (row == -1)
    return NULL; // Out of vertical bounds

  int col = get_brick_col(x);
  if (col == -1)
    return NULL; // Out of horizontal bounds

  Brick *brick = &ctx->brick_manager.grid[row][col];

  return brick->active ? brick : NULL;
}
