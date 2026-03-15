#ifndef BALL_H
#define BALL_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

#define BALL_SIZE 16.0f
#define BALL_SPEED 350.0f

typedef struct {
  SDL_FRect rect;
  float dx;
  float dy;
} Ball;

struct Paddle;

void init_ball(Ball *ball);
void move_ball(Ball *ball, float delta_time);
void set_ball_on_paddle(Ball *ball, struct Paddle *paddle);
void render_ball(SDL_Renderer *renderer, Ball *ball);
bool is_ball_out(Ball *ball);
void check_wall_collision(Ball *ball);
bool check_paddle_collision(Ball *ball, struct Paddle *paddle);

#endif // BALL_H
