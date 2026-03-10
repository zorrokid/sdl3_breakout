#ifndef BALL_H
#define BALL_H
#include <SDL3/SDL_rect.h>

#define BALL_SIZE 16.0f

typedef struct {
    SDL_FRect rect;
    float dx;
    float dy;
} Ball;

void init_ball(Ball *ball);
void move_ball(Ball *ball, SDL_FRect *paddle, bool *ball_launched);

#endif // BALL_H
