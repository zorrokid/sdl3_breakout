#ifndef MAIN_H
#define MAIN_H
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "ball.h"
#include "bricks.h"
#include "paddle.h"
#include "particles.h"
#include "events.h"
#include "textures.h"
void reset_game(GameContext *ctx);

#endif // MAIN_H
