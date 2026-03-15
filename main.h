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

typedef enum {
  STATE_TITLE,
  STATE_PLAYING,
  STATE_GAME_OVER,
  STATE_GAME_WON
} GameState;

typedef struct GameContext {
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool running;
  Paddle paddle;
  Ball ball;
  bool ball_launched;

  bool left_pressed;
  bool right_pressed;

  Brick bricks[BRICK_ROWS * BRICK_COLS];
  uint64_t last_ticks;
  Particle particles[MAX_PARTICLES];
  float shake_timer_s;
  float shake_intensity_pixels;

  int score;
  int last_score; // Used to track changes for updating the texture

  int lives;
  int last_lives; // Used to track changes for updating the texture

  int combo_count;
  GameState state;
  TTF_Font *font;
  SDL_Texture *score_texture;
  SDL_Texture *lives_texture;
} GameContext;

void reset_game(GameContext *ctx);

#endif // MAIN_H
