#include "paddle.h"
#include "common.h"

void init_paddle(Paddle *paddle) {
    paddle->rect.w = PADDLE_WIDTH;
    paddle->rect.h = PADDLE_HEIGHT;
    paddle->rect.x = (SCREEN_WIDTH - paddle->rect.w) / 2.0f; // Center it
    paddle->rect.y = PADDLE_Y;                               // Near the bottom
}

void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed, float delta_time) {
    float paddle_move = 0.0f;
    if (left_pressed)
        paddle_move -= PADDLE_SPEED * delta_time;
    if (right_pressed)
        paddle_move += PADDLE_SPEED * delta_time;

    paddle->rect.x += paddle_move;
    paddle->last_movement = paddle_move;

    // Check screen boundaries
    if (paddle->rect.x < 0)
        paddle->rect.x = 0;
    if (paddle->rect.x > SCREEN_WIDTH - paddle->rect.w)
        paddle->rect.x = SCREEN_WIDTH - paddle->rect.w;
}

void render_paddle(SDL_Renderer *renderer, Paddle *paddle) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white
    SDL_RenderFillRect(renderer, &paddle->rect);
}
