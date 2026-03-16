#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

void reset_game(GameContext *ctx);
void update_gameplay(GameContext *ctx);
void render_title(GameContext *ctx);
void render_gameplay(GameContext *ctx);
void render_game_over(GameContext *ctx);
void render_game_won(GameContext *ctx);
void render(GameContext *ctx);
void play_hit_sound(GameContext *ctx);
bool init_ttf(GameContext *ctx);
bool init_audio(GameContext *ctx);
bool load_sfx(GameContext *ctx, SoundID id, const char *path,
              SDL_AudioDeviceID device);
void play_sfx(GameContext *ctx, SoundID id);
void handle_collision_logic(void *userdata, int event_type);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  (void)argc;                      // Unused
  (void)argv;                      // Unused
  srand((unsigned int)time(NULL)); // Seed random for particle effects

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Allocate memory for our game data
  GameContext *ctx = SDL_calloc(1, sizeof(GameContext));
  if (!ctx)
    return SDL_APP_FAILURE;
  *appstate = ctx;

  // Create a 800x600 window and a renderer with VSync
  if (!SDL_CreateWindowAndRenderer("Breakout SDL3", SCREEN_WIDTH, SCREEN_HEIGHT,
                                   0, &ctx->window, &ctx->renderer)) {
    return SDL_APP_FAILURE;
  }

  SDL_SetRenderVSync(ctx->renderer, 1);

  ctx->state = STATE_TITLE;
  ctx->on_collision = handle_collision_logic;

  // Prorably not necessary, since we used SDL_calloc, which zeroes the memory
  memset(ctx->particles, 0, sizeof(ctx->particles));

  if (!init_ttf(ctx) || !init_audio(ctx))
    return SDL_APP_FAILURE;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  GameContext *ctx = (GameContext *)appstate;

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; // This exits the app cleanly
  }

  switch (ctx->state) {
  case STATE_TITLE:
    return handle_title_events(ctx, event);
    break;
  case STATE_PLAYING:
    return handle_game_events(ctx, event);
    break;
  case STATE_GAME_OVER:
    return handle_game_over(ctx, event);
    break;
  case STATE_GAME_WON:
    return handle_game_won(ctx, event);
    break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  GameContext *ctx = (GameContext *)appstate;
  if (ctx->state == STATE_PLAYING) {
    update_gameplay(ctx);
  }
  render(ctx);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)result; // Unused
  GameContext *ctx = (GameContext *)appstate;
  if (ctx) {
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    if (ctx->score_texture) {
      SDL_DestroyTexture(ctx->score_texture);
    }
    if (ctx->lives_texture) {
      SDL_DestroyTexture(ctx->lives_texture);
    }
    if (ctx->font) {
      TTF_CloseFont(ctx->font);
    }
    for (int i = 0; i < SFX_COUNT; i++) {
      SDL_Log("Checking SoundID %d | Stream: %p | Data: %p", i,
              (void *)ctx->sound_effects[i].stream,
              (void *)ctx->sound_effects[i].data);
      if (ctx->sound_effects[i].stream != NULL) {
        SDL_DestroyAudioStream(ctx->sound_effects[i].stream);
        ctx->sound_effects[i].stream = NULL;
        SDL_Log("Destroyed audio stream for SoundID %d", i);
      }
      if (ctx->sound_effects[i].data != NULL) {
        SDL_free(ctx->sound_effects[i].data);
        ctx->sound_effects[i].data = NULL;
        SDL_Log("Freed audio data for SoundID %d", i);
      }
      SDL_Log("Finished cleaning SoundID %d", i);
    }
    if (ctx->audio_device) {
      SDL_Log("Closing audio device: %d", ctx->audio_device);
      SDL_CloseAudioDevice(ctx->audio_device);
    }
    TTF_Quit();
    SDL_free(ctx);
  }
}

void reset_game(GameContext *ctx) {
  init_paddle(&ctx->paddle);
  init_ball(&ctx->ball);
  init_bricks(ctx->bricks);
  ctx->ball_launched = false;
  ctx->lives = 3;
  ctx->last_ticks = SDL_GetTicks();
  ctx->state = STATE_PLAYING;
}

void update_gameplay(GameContext *ctx) {
  // Calculate delta time
  uint64_t current_ticks = SDL_GetTicks();
  float delta_time = (current_ticks - ctx->last_ticks) / 1000.0f;
  ctx->last_ticks = current_ticks;

  // Cap delta time to prevent big jumps
  if (delta_time > 0.1f)
    delta_time = 0.1f;

  move_paddle(&ctx->paddle, ctx->left_pressed, ctx->right_pressed, delta_time);

  if (ctx->ball_launched) {
    move_ball(&ctx->ball, delta_time);

    // ball collision detection
    check_wall_collision(&ctx->ball);
    check_ball_brick_collision(ctx);
    if (check_paddle_collision(ctx)) {
      ctx->combo_count = 0; // Reset combo on paddle hit
    }
    if (is_ball_out(&ctx->ball)) {
      ctx->lives--;
      ctx->ball_launched = false;
      if (ctx->lives <= 0) {
        ctx->state = STATE_GAME_OVER;
      } else {
        // Just reset the ball and paddle
        init_paddle(&ctx->paddle);
        init_ball(&ctx->ball);
      }
    }

  } else
    set_ball_on_paddle(&ctx->ball, &ctx->paddle);

  update_particles(ctx->particles, delta_time);
  update_score_texture(ctx);
  update_lives_texture(ctx);

  if (check_win_condition(ctx->bricks)) {
    // TODO: Check if it's the last level, otherwise proceed to next level
    SDL_Log("You Win!");
    ctx->state = STATE_GAME_WON;
  }

  // Calculate shake offset if shaking
  SDL_Rect viewport = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  if (ctx->shake_timer_s > 0) {
    viewport.x = (int)((rand() % 100 / 100.0f) * ctx->shake_intensity_pixels);
    viewport.y = (int)((rand() % 100 / 100.0f) * ctx->shake_intensity_pixels);
    ctx->shake_timer_s -= delta_time;
  }

  // Set the viewport to shift the coordinate system for the shake effect
  SDL_SetRenderViewport(ctx->renderer, &viewport);
}

void render_title(GameContext *ctx) {
  SDL_Color white = {255, 255, 255, 255};
  render_text_centered(ctx, "BREAKOUT", SCREEN_HEIGHT / 2.0f - 40, white, 48);
  render_text_centered(ctx, "Press SPACE to Start", SCREEN_HEIGHT / 2.0f + 20,
                       white, 24);
}

void render_gameplay(GameContext *ctx) {
  render_paddle(ctx->renderer, &ctx->paddle);
  render_ball(ctx->renderer, &ctx->ball);
  render_bricks(ctx->renderer, ctx->bricks);
  render_particles(ctx->renderer, ctx->particles);
  render_score(ctx);
  render_lives(ctx);
}

void render_game_over(GameContext *ctx) {
  render_paddle(ctx->renderer, &ctx->paddle);
  render_bricks(ctx->renderer, ctx->bricks);

  // Dim the screen with a semi-transparent overlay
  SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0,
                         150); // Semi-transparent black
  SDL_FRect full_screen_overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderFillRect(ctx->renderer, &full_screen_overlay);

  // Render "Game Over" text
  SDL_Color red = {255, 0, 0, 255};
  SDL_Color white = {255, 255, 255, 255};
  SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);
  render_text_centered(ctx, "GAME OVER", SCREEN_HEIGHT / 2.0f - 40, red, 48);
}

void render_game_won(GameContext *ctx) {
  SDL_Color green = {0, 255, 0, 255};
  render_text_centered(ctx, "YOU WIN!", SCREEN_HEIGHT / 2.0f - 40, green, 48);
}

void render(GameContext *ctx) {
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
  SDL_RenderClear(ctx->renderer);
  switch (ctx->state) {
  case STATE_TITLE:
    render_title(ctx);
    break;
  case STATE_PLAYING:
    render_gameplay(ctx);
    break;
  case STATE_GAME_OVER:
    render_game_over(ctx);
    break;
  case STATE_GAME_WON:
    render_game_won(ctx);
    break;
  }

  SDL_RenderPresent(ctx->renderer);
}

bool init_ttf(GameContext *ctx) {
  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
    return false;
  }

  ctx->font =
      TTF_OpenFont("assets/fonts/Press_Start_2P/PressStart2P-Regular.ttf", 24);
  if (!ctx->font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
    return false;
  }
  return true;
}

bool init_audio(GameContext *ctx) {
  SDL_memset(ctx->sound_effects, 0,
             sizeof(ctx->sound_effects)); // Ensure all sound effects are
                                          // initialized to zero
  ctx->audio_device =
      SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (!ctx->audio_device) {
    SDL_Log("Failed to open audio device: %s", SDL_GetError());
    return false;
  }

  for (int i = 0; i < SFX_COUNT; i++) {
    if (!load_sfx(ctx, i, SFX_PATHS[i], ctx->audio_device)) {
      SDL_Log("Failed to load sound effect for SoundID %d from path: %s", i,
              SFX_PATHS[i]);
      return false;
    }
  }
  SDL_ResumeAudioDevice(ctx->audio_device);
  SDL_Log("Using audio driver: %s", SDL_GetCurrentAudioDriver());
  return true;
}

bool load_sfx(GameContext *ctx, SoundID id, const char *path,
              SDL_AudioDeviceID device) {
  ctx->sound_effects[id].data = NULL;   // Ensure data is NULL before Loading
  ctx->sound_effects[id].stream = NULL; // Ensure stream is NULL before Loading
  SDL_Log("Loading sound effect for SoundID %d from path: %s", id, path);
  if (!path) {
    SDL_Log("No path provided for SoundID: %d", id);
    return false;
  }

  if (!SDL_LoadWAV(path, &ctx->sound_effects[id].spec,
                   &ctx->sound_effects[id].data,
                   &ctx->sound_effects[id].length)) {
    SDL_Log("Failed to load sound: %s", SDL_GetError());
    return false;
  }

  SDL_AudioStream *stream =
      SDL_CreateAudioStream(&ctx->sound_effects[id].spec, NULL);
  if (!stream) {
    SDL_Log("Failed to create audio stream for SoundID %d: %s", id,
            SDL_GetError());
    return false;
  }
  ctx->sound_effects[id].stream = stream;
  SDL_SetAudioStreamGain(ctx->sound_effects[id].stream, 1.0f);
  SDL_BindAudioStream(device, ctx->sound_effects[id].stream);
  return true;
}

void play_sfx(GameContext *ctx, SoundID id) {
  SDL_Log("Playing sound effect for SoundID %d", id);
  if (id < 0 || id >= SFX_COUNT) {
    SDL_Log("Invalid SoundID: %d", id);
    return;
  }
  SoundEffect *sfx = &ctx->sound_effects[id];
  if (sfx->stream && sfx->data) {
    SDL_Log("Putting audio data for SoundID %d: %d bytes", id, sfx->length);
    SDL_PutAudioStreamData(sfx->stream, sfx->data, sfx->length);
  }
}

void handle_brick_hit(GameContext *ctx) {
  ctx->shake_timer_s = 0.3f; // Shake for 0.3 seconds
  ctx->shake_intensity_pixels =
      5.0f + (ctx->combo_count * 2.0f); // Increase shake with combo
  // combo and score
  ctx->combo_count++;
  int base_score = 100;
  int earned = base_score * ctx->combo_count; // More points for combos
  ctx->score += earned;

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

void handle_collision_logic(void *userdata, int event_type) {
  GameContext *ctx = (GameContext *)userdata;
  switch (event_type) {
  case EVENT_PADDLE_HIT:
    handle_paddle_hit(ctx);
    break;
  case EVENT_BRICK_HIT:
    handle_brick_hit(ctx);
    break;
  case EVENT_WALL_HIT:
    handle_wall_hit(ctx);
    break;
  }
}
