#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "main.h"

void on_brick_hit(struct GameContext *ctx, struct Brick *brick);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  srand((unsigned int)time(NULL)); // Seed random for particle effects
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Allocate memory for our game data
  GameContext *ctx = SDL_calloc(1, sizeof(GameContext));
  if (!ctx)
    return SDL_APP_FAILURE;
  *appstate = ctx;

  // Create a 800x600 window and a renderer with VSync
  if (!SDL_CreateWindowAndRenderer("Breakout SDL3", SCREEN_WIDTH, SCREEN_HEIGHT,
                                   0, &ctx->window, &ctx->renderer)) {
    return SDL_APP_FAILURE;
  }

  // Enable VSync so we don't melt the GPU
  SDL_SetRenderVSync(ctx->renderer, 1);

  init_paddle(&ctx->paddle);
  init_ball(&ctx->ball);
  init_bricks(ctx->bricks);

  ctx->ball_launched = false;
  ctx->lives = 3;
  ctx->last_ticks = SDL_GetTicks();

  // Prorably not necessary, since we used SDL_calloc, which zeroes the memory
  memset(ctx->particles, 0, sizeof(ctx->particles));

  return SDL_APP_CONTINUE;
}

// 3. Events: Handle "X" button or keys
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  GameContext *ctx = (GameContext *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; // This exits the app cleanly
  }

  if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
    bool is_down = (event->type == SDL_EVENT_KEY_DOWN);

    switch (event->key.key) {
    case SDLK_LEFT:
      ctx->left_pressed = is_down;
      break;
    case SDLK_RIGHT:
      ctx->right_pressed = is_down;
      break;
    case SDLK_SPACE:
      if (is_down)
        ctx->ball_launched = true;
      break;
    case SDLK_ESCAPE:
      return SDL_APP_SUCCESS;
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

void reset_game(GameContext *ctx) {
  init_paddle(&ctx->paddle);
  init_ball(&ctx->ball);
  init_bricks(ctx->bricks);
  ctx->ball_launched = false;
  ctx->lives = 3;
}

// 4. The Loop: This runs every single frame
SDL_AppResult SDL_AppIterate(void *appstate) {
  GameContext *ctx = (GameContext *)appstate;

  // Calculate delta time
  uint64_t current_ticks = SDL_GetTicks();
  float delta_time = (current_ticks - ctx->last_ticks) / 1000.0f;
  ctx->last_ticks = current_ticks;

  // Cap delta time to prevent big jumps
  if (delta_time > 0.1f)
    delta_time = 0.1f;

  move_paddle(&ctx->paddle, ctx->left_pressed, ctx->right_pressed, delta_time);

  if (ctx->ball_launched) {
    move_ball(&ctx->ball, delta_time);
    check_wall_collision(&ctx->ball);
    check_paddle_collision(&ctx->ball, &ctx->paddle);
  } else
    set_ball_on_paddle(&ctx->ball, &ctx->paddle);

  if (is_ball_out(&ctx->ball)) {
    ctx->lives--;
    ctx->ball_launched = false;
    SDL_Log("Ball lost! Lives remaining: %d", ctx->lives);
    if (ctx->lives <= 0) {
      SDL_Log("Game Over!");
      // Game over, reset everything
      // TODO: show game over screen
      reset_game(ctx);
    } else {
      // Just reset the ball and paddle
      init_paddle(&ctx->paddle);
      init_ball(&ctx->ball);
    }
  }

  check_ball_brick_collision(ctx, on_brick_hit);

  update_particles(ctx->particles, delta_time);

  if (check_win_condition(ctx->bricks)) {
    SDL_Log("You Win!");
    reset_game(ctx);
  }

  // A. Clear the screen (Black)
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
  SDL_RenderClear(ctx->renderer);

  render_paddle(ctx->renderer, &ctx->paddle);
  render_ball(ctx->renderer, &ctx->ball);
  render_bricks(ctx->renderer, ctx->bricks);
  render_particles(ctx->renderer, ctx->particles);

  // C. Show the result on screen
  SDL_RenderPresent(ctx->renderer);

  return SDL_APP_CONTINUE;
}

// 5. Cleanup: Runs when the app closes
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  GameContext *ctx = (GameContext *)appstate;
  if (ctx) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    SDL_free(ctx);
  }
}

void on_brick_hit(struct GameContext *ctx, struct Brick *brick) {
  spawn_brick_burst(ctx->particles, brick, (SDL_Color){255, 0, 0, 255});
  // TODO: add sound effect here
  // TODO: increment score here
}
