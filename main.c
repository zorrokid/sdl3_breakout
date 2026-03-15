#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

void on_brick_hit(struct GameContext *ctx, struct Brick *brick);
void reset_game(GameContext *ctx);
void update_gameplay(GameContext *ctx);
void render_title(GameContext *ctx);
void render_gameplay(GameContext *ctx);
void render_game_over(GameContext *ctx);
void render_game_won(GameContext *ctx);
void render(GameContext *ctx);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;                      // Unused
  (void)argv;                      // Unused
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

  ctx->state = STATE_TITLE;
  // reset_game(ctx);

  // Prorably not necessary, since we used SDL_calloc, which zeroes the memory
  memset(ctx->particles, 0, sizeof(ctx->particles));

  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Load a font
  ctx->font =
      TTF_OpenFont("assets/fonts/Press_Start_2P/PressStart2P-Regular.ttf", 24);

  if (!ctx->font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  GameContext *ctx = (GameContext *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; // This exits the app cleanly
  }

  switch (ctx->state) {
  case STATE_TITLE:
    return handle_title_events(ctx, event);
    break;
  case STATE_PLAYING:
    return handle_game_events(ctx, event);
    break;
  case STATE_GAME_OVER:
    return handle_game_over(ctx, event);
    break;
  case STATE_GAME_WON:
    return handle_game_won(ctx, event);
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  GameContext *ctx = (GameContext *)appstate;
  if (ctx->state == STATE_PLAYING) {
    update_gameplay(ctx);
  }
  render(ctx);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result; // Unused
  GameContext *ctx = (GameContext *)appstate;
  if (ctx) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    SDL_free(ctx);
    if (ctx->score_texture) {
      SDL_DestroyTexture(ctx->score_texture);
    }
    if (ctx->lives_texture) {
      SDL_DestroyTexture(ctx->lives_texture);
    }
    if (ctx->font) {
      TTF_CloseFont(ctx->font);
    }
    TTF_Quit();
  }
}

void on_brick_hit(struct GameContext *ctx, struct Brick *brick) {
  spawn_brick_burst(ctx->particles, brick, (SDL_Color){255, 0, 0, 255});
  ctx->shake_timer_s = 0.3f; // Shake for 0.3 seconds
  ctx->shake_intensity_pixels =
      5.0f + (ctx->combo_count * 2.0f); // Increase shake with combo
  // combo and score
  ctx->combo_count++;
  int base_score = 100;
  int earned = base_score * ctx->combo_count; // More points for combos
  ctx->score += earned;
  SDL_Log("Brick hit! Combo: %d, Score Earned: %d, Total Score: %d",
          ctx->combo_count, earned, ctx->score);
  // TODO: add sound effect here
  // TODO: increment score here
}

void reset_game(GameContext *ctx) {
  init_paddle(&ctx->paddle);
  init_ball(&ctx->ball);
  init_bricks(ctx->bricks);
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

  if (ctx->ball_launched) {
    move_ball(&ctx->ball, delta_time);
    check_wall_collision(&ctx->ball);
    if (check_paddle_collision(&ctx->ball, &ctx->paddle)) {
      ctx->combo_count = 0; // Reset combo on paddle hit
    }
  } else
    set_ball_on_paddle(&ctx->ball, &ctx->paddle);

  if (is_ball_out(&ctx->ball)) {
    ctx->lives--;
    ctx->ball_launched = false;
    SDL_Log("Ball lost! Lives remaining: %d", ctx->lives);
    if (ctx->lives <= 0) {
      SDL_Log("Game Over!");
      ctx->state = STATE_GAME_OVER;
    } else {
      // Just reset the ball and paddle
      init_paddle(&ctx->paddle);
      init_ball(&ctx->ball);
    }
  }

  check_ball_brick_collision(ctx, on_brick_hit);

  update_particles(ctx->particles, delta_time);
  update_score_texture(ctx);
  update_lives_texture(ctx);

  if (check_win_condition(ctx->bricks)) {
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

void render_title(GameContext *ctx) {
  SDL_Color white = {255, 255, 255, 255};
  render_text_centered(ctx, "BREAKOUT", SCREEN_HEIGHT / 2.0f - 40, white, 48);
  render_text_centered(ctx, "Press SPACE to Start", SCREEN_HEIGHT / 2.0f + 20,
                       white, 24);
}

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
