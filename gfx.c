#include "gfx.h"
#include "textures.h"
#include "paddle.h"
#include "bricks.h"
#include "ball.h"
#include "particles.h"

void render_gameplay(GameContext *ctx) {
  render_paddle(ctx->renderer, &ctx->paddle);
  render_ball(ctx->renderer, &ctx->ball);
  render_bricks(ctx->renderer, ctx->bricks);
  render_particles(ctx->renderer, ctx->particles);
  render_score(ctx);
  render_lives(ctx);
}

void render_game_over(GameContext *ctx) {
  render_paddle(ctx->renderer, &ctx->paddle);
  render_bricks(ctx->renderer, ctx->bricks);

  // Dim the screen with a semi-transparent overlay
  SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0,
                         150); // Semi-transparent black
  SDL_FRect full_screen_overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderFillRect(ctx->renderer, &full_screen_overlay);

  // Render "Game Over" text
  SDL_Color red = {255, 0, 0, 255};
  SDL_Color white = {255, 255, 255, 255};
  SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
  render_text_centered(ctx, "GAME OVER", SCREEN_HEIGHT / 2.0f - 40, red, 48);
}

void render_game_won(GameContext *ctx) {
  SDL_Color green = {0, 255, 0, 255};
  render_text_centered(ctx, "YOU WIN!", SCREEN_HEIGHT / 2.0f - 40, green, 48);
}

void render(GameContext *ctx) {
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
  SDL_RenderClear(ctx->renderer);
  switch (ctx->state) {
  case STATE_TITLE:
    render_title(ctx);
    break;
  case STATE_PLAYING:
    render_gameplay(ctx);
    break;
  case STATE_GAME_OVER:
    render_game_over(ctx);
    break;
  case STATE_GAME_WON:
    render_game_won(ctx);
    break;
  }

  SDL_RenderPresent(ctx->renderer);
}

void render_title(GameContext *ctx) {
  SDL_Color white = {255, 255, 255, 255};
  render_text_centered(ctx, "BREAKOUT", SCREEN_HEIGHT / 2.0f - 40, white, 48);
  render_text_centered(ctx, "Press SPACE to Start", SCREEN_HEIGHT / 2.0f + 20,
                       white, 24);
}
