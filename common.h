#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define BALL_SIZE 16.0f
#define BALL_SPEED 350.0f

#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 75.0f
#define BRICK_HEIGHT 20.0f
#define BRICK_PADDING 5.0f

#define PADDLE_WIDTH 100.0f
#define PADDLE_HEIGHT 20.0f
#define PADDLE_Y 560.0f
#define PADDLE_SPEED 700.0f

#define MAX_PARTICLES 200

typedef enum {
  STATE_TITLE,
  STATE_PLAYING,
  STATE_GAME_OVER,
  STATE_GAME_WON
} GameState;

typedef enum {
  SFX_BRICK_HIT,
  SFX_PADDLE_HIT,
  // NOTE: keep SFX_COUNT last
  SFX_COUNT
} SoundID;

typedef struct Paddle {
  SDL_FRect rect;
  // This is used to calculate the ball's bounce angle based on how fast and
  // what direction the paddle is moving
  float last_movement;
  // current speed
  float velocity;
  // speed limit
  float max_speed;
  // for speeding up
  float acceleration;
  // for slowing down
  float friction;
} Paddle;

typedef struct Ball {
  SDL_FRect rect;
  float dx;
  float dy;
} Ball;

typedef struct Brick {
  SDL_FRect rect;
  bool active;
} Brick;

typedef struct {
  SDL_FRect rect;
  float dx, dy;
  float life_time; // Time since the particle was created
  SDL_Color color;
  bool active;
} Particle;

typedef struct {
  Uint8 *data;
  Uint32 length;
  SDL_AudioSpec spec;
  SDL_AudioStream *stream;
} SoundEffect;

typedef void (*CollisionCallback)(void *userdata, int event_type);

typedef enum { EVENT_PADDLE_HIT, EVENT_BRICK_HIT, EVENT_WALL_HIT } GameEvent;

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

  SDL_AudioDeviceID audio_device;
  SoundEffect sound_effects[SFX_COUNT];

  CollisionCallback on_collision;

} GameContext;

static const char *SFX_PATHS[SFX_COUNT] = {"assets/sounds/brick_hit.wav",
                                           "assets/sounds/paddle_hit.wav"};

#endif // COMMON_H
       //
