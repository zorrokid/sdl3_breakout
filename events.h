#ifndef EVENTS_H
#define EVENTS_H
#include "common.h"

SDL_AppResult handle_game_events(GameContext *ctx, SDL_Event *event);
SDL_AppResult handle_title_events(GameContext *ctx, SDL_Event *event);
SDL_AppResult handle_game_over(GameContext *ctx, SDL_Event *event);
SDL_AppResult handle_game_won(GameContext *ctx, SDL_Event *event);

#endif // EVENTS_H
