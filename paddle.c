#include "paddle.h"
#include "common.h"

void init_paddle(Paddle *paddle) {
  paddle->rect.w = PADDLE_WIDTH;
  paddle->rect.h = PADDLE_HEIGHT;
  paddle->rect.x = (SCREEN_WIDTH - paddle->rect.w) / 2.0f; // Center it
  paddle->rect.y = PADDLE_Y;                               // Near the bottom
  paddle->max_speed = PADDLE_SPEED;
  paddle->acceleration =
      PADDLE_SPEED * 5.0f; // How quickly it reaches max speed
  paddle->friction =
      PADDLE_SPEED * 4.0f; // How quickly it slows down when not pressed
  paddle->velocity = 0.0f; // Current speed
}

void move_paddle(Paddle *paddle, bool left_pressed, bool right_pressed,
                 float delta_time) {

  // Handle acceleration
  if (left_pressed) {
    paddle->velocity -= paddle->acceleration * delta_time;
  } else if (right_pressed) {
    paddle->velocity += paddle->acceleration * delta_time;
  } else {
    // Handle deceleration (friction)
    // If velocity is positive, we want to decrease it. If it's negative, we
    // want to increase it.
    if (paddle->velocity > 0) {
      paddle->velocity -= paddle->friction * delta_time;
      if (paddle->velocity < 0)
        paddle->velocity = 0; // Prevent overshooting to the opposite direction
    } else if (paddle->velocity < 0) {
      paddle->velocity += paddle->friction * delta_time;
      if (paddle->velocity > 0)
        paddle->velocity = 0; // Prevent overshooting to the opposite direction
    }
  }

  // cleam max speed
  if (paddle->velocity > paddle->max_speed)
    paddle->velocity = paddle->max_speed;
  if (paddle->velocity < -paddle->max_speed)
    paddle->velocity = -paddle->max_speed;

  // Apply velocity to position
  float paddle_move = paddle->velocity * delta_time;
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
