#include "ball.h"
#include "common.h"
#include "paddle.h"

void init_ball(Ball *ball) {
    ball->rect = (SDL_FRect){0, 0, BALL_SIZE, BALL_SIZE};
    ball->dx = 4.0f;
    ball->dy = -4.0f;
}

void move_ball(Ball *ball, Paddle *paddle, bool *ball_launched) {
    if (!*ball_launched) {
        // put the ball in the middle of the paddle
        ball->rect.x = paddle->rect.x + (paddle->rect.w / 2.0f) - (ball->rect.w / 2.0f);
        ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f; // just above the paddle

    } else {
        // Move the ball
        ball->rect.x += ball->dx;
        ball->rect.y += ball->dy;

        // Wall collision

        if (ball->rect.x <= 0 || ball->rect.x >= SCREEN_WIDTH - ball->rect.w)
            ball->dx *= -1;
        if (ball->rect.y <= 0)
            ball->dy *= -1;

        // Paddle collision
        if (SDL_HasRectIntersectionFloat(&ball->rect, &paddle->rect)) {
            // Reverse the direction
            ball->dy *= -1.0f;

            // snap the ball to the top of the paddle to prevent sticking
            ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;
        }

        if (ball->rect.y > SCREEN_HEIGHT) {
            // Ball fell below the screen, reset it
            *ball_launched = false;
        }
    }
    // Collision uses the rect inside the ball struct
    if (SDL_HasRectIntersectionFloat(&ball->rect, &paddle->rect)) {
        ball->dy *= -1.0f;
        ball->rect.y = paddle->rect.y - ball->rect.h;
    }
}

void render_ball(SDL_Renderer *renderer, Ball *ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
    SDL_RenderFillRect(renderer, &ball->rect);
}
