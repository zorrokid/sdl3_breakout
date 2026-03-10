#ifndef PADDLE_H
#define PADDLE_H

#include <SDL3/SDL_rect.h>

#define PADDLE_WIDTH 100.0f
#define PADDLE_HEIGHT 20.0f
#define PADDLE_Y 560.0f
#define PADDLE_SPEED 7.0f

typedef struct {
    SDL_FRect rect;
} Paddle;

void init_paddle(Paddle *paddle);
void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed);

#endif // PADDLE_H
