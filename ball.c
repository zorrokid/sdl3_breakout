#include "ball.h"
#include "common.h"

void move_ball(Ball *ball, SDL_FRect *paddle, bool *ball_launched) {
    if (!*ball_launched) {
        // put the ball in the middle of the paddle
        ball->rect.x = paddle->x + (paddle->w / 2.0f) - (ball->rect.w / 2.0f);
        ball->rect.y = paddle->y - ball->rect.h - 1.0f; // just above the paddle

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
        if (SDL_HasRectIntersectionFloat(&ball->rect, paddle)) {
            // Reverse the direction
            ball->dy *= -1.0f;

            // snap the ball to the top of the paddle to prevent sticking
            ball->rect.y = paddle->y - ball->rect.h - 1.0f;
        }

        if (ball->rect.y > SCREEN_HEIGHT) {
            // Ball fell below the screen, reset it
            ball_launched = false;
        }
    }
    // Collision uses the rect inside the ball struct
    if (SDL_HasRectIntersectionFloat(&ball->rect, paddle)) {
        ball->dy *= -1.0f;
        ball->rect.y = paddle->y - ball->rect.h;
    }
}
