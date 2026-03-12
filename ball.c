#include "ball.h"
#include "common.h"
#include "paddle.h"

void init_ball(Ball *ball) {
    ball->rect = (SDL_FRect){0, 0, BALL_SIZE, BALL_SIZE};
    ball->dx = BALL_SPEED_X;
    ball->dy = BALL_SPEED_Y;
}

void move_ball(Ball *ball, Paddle *paddle, float delta_time) {

    // Move the ball
    ball->rect.x += ball->dx * delta_time;
    ball->rect.y += ball->dy * delta_time;

    // Check wall collision

    if (ball->rect.x <= 0 || ball->rect.x >= SCREEN_WIDTH - ball->rect.w)
        ball->dx *= -1;
    if (ball->rect.y <= 0)
        ball->dy *= -1;

    // Check paddle collision
    if (SDL_HasRectIntersectionFloat(&ball->rect, &paddle->rect)) {
        ball->dy *= -1.0f;

        // snap the ball to the top of the paddle to prevent sticking
        ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;
    }
}

void set_ball_on_paddle(Ball *ball, Paddle *paddle) {
    // in the center of the paddle
    ball->rect.x = paddle->rect.x + (paddle->rect.w / 2.0f) - (ball->rect.w / 2.0f);
    // and just above the paddle
    ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;
}

bool is_ball_out(Ball *ball) { return ball->rect.y > SCREEN_HEIGHT; }

void render_ball(SDL_Renderer *renderer, Ball *ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
    SDL_RenderFillRect(renderer, &ball->rect);
}
