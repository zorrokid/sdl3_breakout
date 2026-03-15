#include "events.h"
#include "main.h"

SDL_AppResult handle_title_events(GameContext *ctx, SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.key) {
    case SDLK_SPACE:
      reset_game(ctx);
      ctx->state = STATE_PLAYING;

      break;
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult handle_game_events(GameContext *ctx, SDL_Event *event) {
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

SDL_AppResult handle_game_over(GameContext *ctx, SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_SPACE) {
    ctx->state = STATE_TITLE;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult handle_game_won(GameContext *ctx, SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_SPACE) {
    ctx->state = STATE_TITLE;
  }
  return SDL_APP_CONTINUE;
}
