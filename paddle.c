#include "paddle.h"
#include "common.h"

void move_paddle(SDL_FRect *paddle, bool left_pressed, bool right_pressed) {
    // Move the paddle
    float paddle_speed = PADDLE_SPEED; // TODO: add delta time
    if (left_pressed)
        paddle->x -= paddle_speed;
    if (right_pressed)
        paddle->x += paddle_speed;

    // Check screen boundaries
    if (paddle->x < 0)
        paddle->x = 0;
    if (paddle->x > SCREEN_WIDTH - paddle->w)
        paddle->x = SCREEN_WIDTH - paddle->w;
}
