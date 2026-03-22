#include <stdlib.h>

#include "gameplay.h"
#include "ball.h"
#include "bricks.h"
#include "paddle.h"
#include "gfx.h"
#include "particles.h"
#include "textures.h"

void reset_game(GameContext *ctx) {
  init_paddle(&ctx->paddle);
  init_ball(&ctx->ball);

  SDL_zero(ctx->brick_manager.grid);
  ctx->brick_manager.head_row = 0;
  ctx->brick_manager.scroll_offset = 0;
  ctx->brick_manager.scroll_speed = 20.0f;

  init_bricks(ctx);
  ctx->ball_launched = false;
  ctx->lives = 3;
  ctx->last_ticks = SDL_GetTicks();
  ctx->state = STATE_PLAYING;
}

void update_gameplay(GameContext *ctx) {
  // Calculate delta time
  uint64_t current_ticks = SDL_GetTicks();
  float delta_time = (current_ticks - ctx->last_ticks) / 1000.0f;
  ctx->last_ticks = current_ticks;

  // Cap delta time to prevent big jumps
  if (delta_time > 0.1f)
    delta_time = 0.1f;

  move_paddle(&ctx->paddle, ctx->left_pressed, ctx->right_pressed, delta_time);

  update_scrolling(ctx, delta_time);

  if (ctx->ball_launched) {
    move_ball(&ctx->ball, delta_time);

    // ball collision detection
    check_wall_collision(&ctx->ball);
    check_ball_brick_collision(ctx);
    if (check_paddle_collision(ctx)) {
      ctx->combo_count = 0; // Reset combo on paddle hit
    }
    if (is_ball_out(&ctx->ball)) {
      ctx->lives--;
      ctx->ball_launched = false;
      if (ctx->lives <= 0) {
        ctx->state = STATE_GAME_OVER;
      } else {
        // Just reset the ball and paddle
        init_paddle(&ctx->paddle);
        init_ball(&ctx->ball);
      }
    }

  } else
    set_ball_on_paddle(&ctx->ball, &ctx->paddle);

  update_particles(ctx->particles, delta_time);
  update_score_texture(ctx);
  update_lives_texture(ctx);

  if (check_win_condition(ctx)) {
    // TODO: Check if it's the last level, otherwise proceed to next level
    SDL_Log("You Win!");
    ctx->state = STATE_GAME_WON;
  }

  // Calculate shake offset if shaking
  SDL_Rect viewport = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  if (ctx->shake_timer_s > 0) {
    viewport.x = (int)((rand() % 100 / 100.0f) * ctx->shake_intensity_pixels);
    viewport.y = (int)((rand() % 100 / 100.0f) * ctx->shake_intensity_pixels);
    ctx->shake_timer_s -= delta_time;
  }

  // Set the viewport to shift the coordinate system for the shake effect
  SDL_SetRenderViewport(ctx->renderer, &viewport);
}
