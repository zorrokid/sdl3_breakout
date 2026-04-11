#include "ball.h"
#include "paddle.h"
#include <math.h>

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

float calculate_bounce_angle(Ball *ball, Paddle *paddle) {
  // Calculate the hit position relative to the paddle center
  Coord ball_center = get_ball_center(ball);
  Coord paddle_center = get_paddle_center(paddle);
  float ball_dist = ball_center.x - paddle_center.x;
  float half_width = paddle->rect.w / 2.0f;

  // Multiplier determines how much the ball's horizontal speed changes based
  // on where it hits the paddle. Hitting the center results in no horizontal
  // change, while hitting the edges results in maximum horizontal change.
  // Range -1.0 (left edge) to 1.0 (right edge)
  float multiplier = ball_dist / half_width;
  float base_angle = multiplier * MAX_BOUNCE_ANGLE;

  float paddle_speed_factor = paddle->last_movement / paddle->max_speed;

  return multiplier * MAX_BOUNCE_ANGLE;
}

bool check_paddle_collision(GameContext *ctx) {
  Ball *ball = &ctx->ball;
  Paddle *paddle = &ctx->paddle;
  if (SDL_HasRectIntersectionFloat(&ball->rect, &paddle->rect)) {
    // snap the ball to the top of the paddle to prevent sticking
    ball->rect.y = paddle->rect.y - ball->rect.h - 1.0f;

    // Get the base bounce angle based on where the ball hit the paddle
    float bounce_angle = calculate_bounce_angle(ball, paddle);

    // Incorporate paddle movement into the bounce angle
    if (paddle->last_movement > 0)
      bounce_angle += 0.15f; // Radian shift
    if (paddle->last_movement < 0)
      bounce_angle -= 0.15f; // Radian shift

    // clamp the bounce angle to prevent it from being too flat
    if (bounce_angle > MAX_BOUNCE_ANGLE)
      bounce_angle = MAX_BOUNCE_ANGLE;
    if (bounce_angle < -MAX_BOUNCE_ANGLE)
      bounce_angle = -MAX_BOUNCE_ANGLE;

    // Set velocity based on the bounce angle
    ball->dx = BALL_SPEED * SDL_sinf(bounce_angle);
    // We negate the Y velocity to change the direction upwards
    ball->dy = -BALL_SPEED * SDL_cosf(bounce_angle);

    CollisionEvent event = {.type = EVENT_PADDLE_HIT};
    ctx->on_collision(ctx, &event);

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

Coord get_ball_center(const Ball *ball) {
  Coord center;
  center.x = ball->rect.x + (ball->rect.w / 2.0f);
  center.y = ball->rect.y + (ball->rect.h / 2.0f);
  return center;
}
