#include "bricks.h"
#include "particles.h"

void init_bricks(Brick *bricks) {
  float total_width =
      (BRICK_COLS * (BRICK_WIDTH + BRICK_PADDING)) - BRICK_PADDING;
  // Center the bricks horizontally
  float start_x = (SCREEN_WIDTH - total_width) / 2.0f;
  float start_y = 50.0f; // Start 50 pixels from the top

  for (int row = 0; row < BRICK_ROWS; ++row) {
    for (int col = 0; col < BRICK_COLS; ++col) {
      int i = row * BRICK_COLS + col;
      bricks[i].rect.x = start_x + (col * (BRICK_WIDTH + BRICK_PADDING));
      bricks[i].rect.y = start_y + (row * (BRICK_HEIGHT + BRICK_PADDING));
      bricks[i].rect.w = BRICK_WIDTH;
      bricks[i].rect.h = BRICK_HEIGHT;
      bricks[i].active = true;
    }
  }
}

void render_bricks(SDL_Renderer *renderer, Brick *bricks) {
  for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
    if (bricks[i].active) {
      int row = i / BRICK_COLS;
      // Color based on row
      SDL_SetRenderDrawColor(renderer, 255 - (row * 40), 0, row * 40, 255);
      SDL_RenderFillRect(renderer, &bricks[i].rect);
    }
  }
}

void check_ball_brick_collision(struct GameContext *ctx) {
  for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
    Brick *brick = &ctx->bricks[i];
    if (brick->active &&
        SDL_HasRectIntersectionFloat(&ctx->ball.rect, &brick->rect)) {

      // calculate center of brick to center of ball vector to detect
      // if the collision is top / bottom or side of a brick.

      // Centers
      float ball_cx = ctx->ball.rect.x + (ctx->ball.rect.w / 2.0f);
      float ball_cy = ctx->ball.rect.y + (ctx->ball.rect.h / 2.0f);
      float brick_cx = brick->rect.x + (brick->rect.w / 2.0f);
      float brick_cy = brick->rect.y + (brick->rect.h / 2.0f);

      // relative distance (Vector from brick center to ball center)
      float dx = ball_cx - brick_cx;
      float dy = ball_cy - brick_cy;

      // we need to normalize the distance between center of the ball
      // and center of the brick by calculating the percentage
      // how far the ball center is from brick center
      float brick_width_half = brick->rect.w / 2.0f;
      float brick_height_half = brick->rect.h / 2.0f;

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
}

bool check_win_condition(Brick *bricks) {
  for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
    if (bricks[i].active)
      return false;
  }
  return true;
}
