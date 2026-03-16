#include "ball.h"
#include "common.h"
#include "paddle.h"
#include "main.h"

void init_ball(Ball *ball) {
  ball->rect = (SDL_FRect){0, 0, BALL_SIZE, BALL_SIZE};
  ball->dx = BALL_SPEED;
  ball->dy = -BALL_SPEED;
}

void move_ball(Ball *ball, float delta_time) {
  ball->rect.x += ball->dx * delta_time;
  ball->rect.y += ball->dy * delta_time;
}

void check_wall_collision(Ball *ball) {

  // Left wall
  if (ball->rect.x <= 0) {
    // snap the ball to the wall to prevent sticking
    ball->rect.x = 0;
    ball->dx *= -1.0f;
  }
  // Right wall
  if (ball->rect.x + ball->rect.w >= SCREEN_WIDTH) {
    // snap the ball to the wall to prevent sticking
    ball->rect.x = SCREEN_WIDTH - ball->rect.w;
    ball->dx *= -1.0f;
  }

  // Top wall
  if (ball->rect.y <= 0) {
    // snap the ball to the wall to prevent sticking
    ball->rect.y = 0;
    ball->dy *= -1.0f;
  }
}

bool check_paddle_collision(struct GameContext *ctx) {
  Ball *ball = &ctx->ball;
  Paddle *paddle = &ctx->paddle;
  if (SDL_HasRectIntersectionFloat(&ball->rect, &paddle->rect)) {
    ball->dy *= -1.0f;

    // snap the ball to the top of the paddle to prevent sticking
    ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;

    // Calculate the it position relative to the paddle center
    float ball_center_x = ball->rect.x + (ball->rect.w / 2.0f);
    float paddle_center_x = paddle->rect.x + (paddle->rect.w / 2.0f);
    float ball_dist = ball_center_x - paddle_center_x;

    // Range -0.5 (left edge) to 0.5 (right edge)
    float multiplier = ball_dist / paddle->rect.w;

    // Apply the "English"
    float max_bounce_angle = 2.0f;
    ball->dx = multiplier * (BALL_SPEED * max_bounce_angle);

    // Adjust the ball's horizontal speed based on the paddle's movement to add
    // more "English"
    if (paddle->last_movement > 0)
      ball->dx += (PADDLE_SPEED * 0.2f);
    if (paddle->last_movement < 0)
      ball->dx -= (PADDLE_SPEED * 0.2f);

    // Normalize the ball's speed to maintain a consistent overall speed
    float current_speed = SDL_sqrtf(ball->dx * ball->dx + ball->dy * ball->dy);
    ball->dx = (ball->dx / current_speed) * BALL_SPEED;
    ball->dy = (ball->dy / current_speed) * BALL_SPEED;
    ctx->on_collision(ctx, EVENT_PADDLE_HIT);
    return true;
  }
  return false;
}

void set_ball_on_paddle(Ball *ball, struct Paddle *paddle) {
  // in the center of the paddle
  ball->rect.x =
      paddle->rect.x + (paddle->rect.w / 2.0f) - (ball->rect.w / 2.0f);
  // and just above the paddle
  ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;
}

bool is_ball_out(Ball *ball) { return ball->rect.y > SCREEN_HEIGHT; }

void render_ball(SDL_Renderer *renderer, Ball *ball) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
  SDL_RenderFillRect(renderer, &ball->rect);
}
