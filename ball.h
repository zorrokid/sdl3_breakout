#ifndef BALL_H
#define BALL_H
#include "common.h"

void init_ball(Ball *ball);
void move_ball(Ball *ball, float delta_time);
void set_ball_on_paddle(Ball *ball, struct Paddle *paddle);
void render_ball(SDL_Renderer *renderer, struct Ball *ball);
bool is_ball_out(Ball *ball);
void check_wall_collision(Ball *ball);
bool check_paddle_collision(GameContext *ctx);
Coord get_ball_center(const Ball *ball);

#endif // BALL_H
