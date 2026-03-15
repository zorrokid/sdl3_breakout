#ifndef PADDLE_H
#define PADDLE_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

#define PADDLE_WIDTH 100.0f
#define PADDLE_HEIGHT 20.0f
#define PADDLE_Y 560.0f
#define PADDLE_SPEED 700.0f

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

void init_paddle(Paddle *paddle);
void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed,
                 float delta_time);
void render_paddle(SDL_Renderer *renderer, Paddle *paddle);

#endif // PADDLE_H
