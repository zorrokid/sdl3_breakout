#include "paddle.h"
#include "common.h"

void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed) {
    // Move the paddle
    float paddle_speed = PADDLE_SPEED; // TODO: add delta time
    if (left_pressed)
        paddle->rect.x -= paddle_speed;
    if (right_pressed)
        paddle->rect.x += paddle_speed;

    // Check screen boundaries
    if (paddle->rect.x < 0)
        paddle->rect.x = 0;
    if (paddle->rect.x > SCREEN_WIDTH - paddle->rect.w)
        paddle->rect.x = SCREEN_WIDTH - paddle->rect.w;
}
