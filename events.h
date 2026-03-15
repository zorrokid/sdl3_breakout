#ifndef EVENTS_H
#define EVENTS_H
#include <SDL3/SDL.h>

struct GameContext;

SDL_AppResult handle_game_events(struct GameContext *ctx, SDL_Event *event);
SDL_AppResult handle_title_events(struct GameContext *ctx, SDL_Event *event);
SDL_AppResult handle_game_over(struct GameContext *ctx, SDL_Event *event);

#endif // EVENTS_H
