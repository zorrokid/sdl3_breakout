#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>
#include "sfx.h"
#include "gfx.h"
#include "main.h"
#include "collision.h"
#include "gameplay.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;                      // Unused
  (void)argv;                      // Unused
  srand((unsigned int)time(NULL)); // Seed random for particle effects

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

  SDL_SetRenderVSync(ctx->renderer, 1);

  ctx->state = STATE_TITLE;
  ctx->on_collision = handle_collision_logic;

  // Prorably not necessary, since we used SDL_calloc, which zeroes the memory
  memset(ctx->particles, 0, sizeof(ctx->particles));

  if (!init_ttf(ctx) || !init_audio(ctx))
    return SDL_APP_FAILURE;

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
    cleanup_textures(ctx);
    cleanup_audio(ctx);
    SDL_free(ctx);
  }
}
