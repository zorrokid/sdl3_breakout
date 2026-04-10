#include "bricks.h"
#include "common.h"
#include "particles.h"
#include <stdio.h>
#include <math.h>

// Calculate the starting x position to center the bricks on the screen
float get_start_x() {
  float total_width = (BRICK_COLS * BRICK_TOTAL_WIDTH) - BRICK_PADDING;
  return (SCREEN_WIDTH - total_width) / 2.0f;
}

void render_bricks(GameContext *ctx) {
  for (int i = 0; i < MAX_BRICK_ROWS; i++) {

    // Map the visual row index to physical grid row index
    // E.g. if head_row is 5, then screen row 0 is actually row 5 in the array
    int actual_row_index = (ctx->brick_manager.head_row + i) % MAX_BRICK_ROWS;

    for (int c = 0; c < BRICK_COLS; c++) {
      Brick *brick = &ctx->brick_manager.grid[actual_row_index][c];
      if (brick->active) {
        // Calculate the position to draw the brick based on its column and row
        // index
        SDL_FRect draw_rect = brick->rect;
        draw_rect.x = get_start_x() + c * BRICK_TOTAL_WIDTH;

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

void init_bricks(GameContext *ctx) {
  for (int i = 0; i < 6; i++) {
    int actual_row_index = (ctx->brick_manager.head_row + i) % MAX_BRICK_ROWS;
    spawn_new_row(ctx, actual_row_index);
  }
}

void check_ball_brick_collision(GameContext *ctx) {

  Brick *brick =
      get_brick_at_screen_pos(ctx, ctx->ball.rect.x, ctx->ball.rect.y);

  if (brick) {

    int actual_row = get_brick_array_row(&ctx->brick_manager, ctx->ball.rect.y);
    int visual_row_index =
        (actual_row - ctx->brick_manager.head_row + MAX_BRICK_ROWS) %
        MAX_BRICK_ROWS;

    int col = get_brick_col(ctx->ball.rect.x);

    float visual_y = ((visual_row_index - 1) * BRICK_TOTAL_HEIGHT) +
                     ctx->brick_manager.scroll_offset;

    float visual_x = get_start_x() + col * BRICK_TOTAL_WIDTH;

    // calculate center of brick to center of ball vector to detect
    // if the collision is top / bottom or side of a brick.

    SDL_FRect physics_rect = {visual_x, visual_y, BRICK_WIDTH, BRICK_HEIGHT};

    // TODO: these calculations are not working as reliable as they were without
    // brics scrolling sometimes ball bounces up breaking whole column of bricks

    // Centers
    float ball_cx = ctx->ball.rect.x + (ctx->ball.rect.w / 2.0f);
    float ball_cy = ctx->ball.rect.y + (ctx->ball.rect.h / 2.0f);
    float brick_cx = physics_rect.x + (physics_rect.w / 2.0f);
    float brick_cy = physics_rect.y + (physics_rect.h / 2.0f);

    // relative distance (Vector from brick center to ball center)
    float dx = ball_cx - brick_cx;
    float dy = ball_cy - brick_cy;

    // we need to normalize the distance between center of the ball
    // and center of the brick by calculating the percentage
    // how far the ball center is from brick center
    float brick_width_half = physics_rect.w / 2.0f;
    float brick_height_half = physics_rect.h / 2.0f;

    // percentage from side
    float dx_percentage = SDL_fabs(dx / brick_width_half);
    // percentage from top / bottom
    float dy_percentage = SDL_fabs(dy / brick_height_half);

    if (dx_percentage > dy_percentage) {
      // side hit
      ctx->ball.dx *= -1.0f;
    } else {
      // top / bottom hit
      ctx->ball.dy *= -1.0f;
    }

    spawn_brick_burst(ctx->particles, brick, (SDL_Color){255, 0, 0, 255},
                      brick_cx, brick_cy);

    if (ctx->on_collision)
      ctx->on_collision(ctx, EVENT_BRICK_HIT);

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
  if (ctx->brick_manager.scroll_offset >= BRICK_HEIGHT) {
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

  // brick scroll offset need to be taken into account to map to correct visual
  // row for collision detection
  float relative_y = screen_y - bm->scroll_offset;

  // We add 1 to the visual row calculation because the topmost row starts just
  // above the screen (at -BRICK_HEIGHT).
  int visual_row = (int)SDL_floorf(relative_y / BRICK_TOTAL_HEIGHT) + 1;

  // check bounds
  if (visual_row < 0 || visual_row >= MAX_BRICK_ROWS)
    return -1;

  // Map the visual row index to the actual array index using the circular
  // buffer rotation
  int array_row = (bm->head_row + visual_row) % MAX_BRICK_ROWS;
  return array_row;
}

int get_brick_col(float screen_x) {
  int col = (int)(((screen_x - get_start_x()) / BRICK_TOTAL_WIDTH));
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
