#include "ball.h"
#include "bricks.h"
#include "paddle.h"
#include "particles.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

typedef enum {
  STATE_TITLE,
  STATE_PLAYING,
  STATE_GAME_OVER,
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
  int lives;
  uint64_t last_ticks;
  Particle particles[MAX_PARTICLES];
  float shake_timer_s;
  float shake_intensity_pixels;
  int score;
  int combo_count;
  GameState state;
} GameContext;
