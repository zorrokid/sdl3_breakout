#include "bricks.h"
#include "common.h"
#include "particles.h"
#include <stdio.h>

float get_start_x() {
  float total_width =
      (BRICK_COLS * (BRICK_WIDTH + BRICK_PADDING)) - BRICK_PADDING;
  // Center the bricks horizontally
  float start_x = (SCREEN_WIDTH - total_width) / 2.0f;
  return start_x;
}

void render_bricks(GameContext *ctx) {
  for (int i = 0; i < MAX_BRICK_ROWS; i++) {

    // Map the visual row index to physical array row index
    // E.g. if head_row is 5, then screen row 0 is actually row 5 in the array
    int actual_row_index = (ctx->brick_manager.head_row + i) % MAX_BRICK_ROWS;

    for (int c = 0; c < BRICK_COLS; c++) {
      Brick *brick = &ctx->brick_manager.grid[actual_row_index][c];
      if (brick->active) {
        // Calculate the position to draw the brick based on its column and row
        // index
        SDL_FRect draw_rect = brick->rect;
        draw_rect.x = get_start_x() + c * (BRICK_WIDTH + BRICK_PADDING);
        draw_rect.y = ((i - 1) * (BRICK_HEIGHT + BRICK_PADDING)) +
                      ctx->brick_manager.scroll_offset -
                      (BRICK_HEIGHT + BRICK_PADDING); // Adjust for scrolling
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

  float relative_ball_y = ctx->ball.rect.y - ctx->brick_manager.scroll_offset;
  // TODO why need to add 2 rows here?
  int visual_row = (int)(relative_ball_y / (BRICK_HEIGHT + BRICK_PADDING)) + 2;

  int col = (int)((ctx->ball.rect.x / (BRICK_WIDTH + BRICK_PADDING)));

  if (visual_row < 0 || visual_row >= MAX_BRICK_ROWS || col < 0 ||
      col >= BRICK_COLS)
    return;

  int actual_row = (ctx->brick_manager.head_row + visual_row) % MAX_BRICK_ROWS;

  float brick_x = get_start_x() + col * (BRICK_WIDTH + BRICK_PADDING);
  float brick_y = (visual_row - 1) * (BRICK_HEIGHT + BRICK_PADDING) +
                  ctx->brick_manager.scroll_offset;

  Brick *brick = &ctx->brick_manager.grid[actual_row][col];

  if (brick->active) {

    // calculate center of brick to center of ball vector to detect
    // if the collision is top / bottom or side of a brick.

    SDL_FRect physics_rect = {brick_x, brick_y, BRICK_WIDTH, BRICK_HEIGHT};

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

  /*for (int i = 0; i < MAX_BRICK_ROWS; i++) {
    for (int c = 0; c < BRICK_COLS; c++) {
      Brick *brick = &ctx->brick_manager.grid[i][c];
      SDL_FRect physics_rect = brick->rect;
      physics_rect.y +=
          ctx->brick_manager.scroll_offset; // Adjust for scrolling
      if (brick->active &&
          SDL_HasRectIntersectionFloat(&ctx->ball.rect, &physics_rect)) {

        // calculate center of brick to center of ball vector to detect
        // if the collision is top / bottom or side of a brick.

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

        spawn_brick_burst(ctx->particles, brick, (SDL_Color){255, 0, 0, 255});

        if (ctx->on_collision)
          ctx->on_collision(ctx, EVENT_BRICK_HIT);

        brick->active = false;
        break;
      }
    }
  }*/
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
