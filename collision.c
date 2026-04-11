#include "collision.h"
#include "particles.h"
#include "sfx.h"

void handle_brick_hit(GameContext *ctx, Coord position) {
  ctx->shake_timer_s = 0.3f; // Shake for 0.3 seconds
  ctx->shake_intensity_pixels =
      5.0f + (ctx->combo_count * 2.0f); // Increase shake with combo
  // combo and score
  ctx->combo_count++;
  int base_score = 100;
  int earned = base_score * ctx->combo_count; // More points for combos
  ctx->score += earned;

  // particle effect
  spawn_brick_burst(ctx->particles, (SDL_Color){255, 0, 0, 255}, position);

  SDL_AudioStream *stream = ctx->sound_effects[SFX_BRICK_HIT].stream;

  if (stream) {
    // Calculate sound frequency based on combo count
    float ratio = 1.0f + (ctx->combo_count * 0.05f);

    // Cap the ratio to double speed
    if (ratio > 2.0f)
      ratio = 2.0f;

    SDL_SetAudioStreamFrequencyRatio(stream, ratio);

    SDL_Log("Playing SFX with frequency ratio: %.2f for combo count: %d", ratio,
            ctx->combo_count);
  }

  play_sfx(ctx, SFX_BRICK_HIT);
}

void handle_paddle_hit(GameContext *ctx) { play_sfx(ctx, SFX_PADDLE_HIT); }

void handle_wall_hit(GameContext *ctx) {
  // Optional: play a wall hit sound
}

void handle_collision_logic(GameContext *ctx, const CollisionEvent *event) {
  switch (event->type) {
  case EVENT_PADDLE_HIT:
    handle_paddle_hit(ctx);
    break;
  case EVENT_BRICK_HIT:
    handle_brick_hit(ctx, event->position);
    break;
  case EVENT_WALL_HIT:
    handle_wall_hit(ctx);
    break;
  }
}
