#ifndef PADDLE_H
#define PADDLE_H

#include "common.h"

void init_paddle(Paddle *paddle);
void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed,
                 float delta_time);
void render_paddle(SDL_Renderer *renderer, Paddle *paddle);
Coord get_paddle_center(const Paddle *paddle);

#endif // PADDLE_H
