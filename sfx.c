#include "sfx.h"

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

bool cleanup_audio(GameContext *ctx) {
  for (int i = 0; i < SFX_COUNT; i++) {
    if (ctx->sound_effects[i].stream != NULL) {
      SDL_DestroyAudioStream(ctx->sound_effects[i].stream);
      ctx->sound_effects[i].stream = NULL;
    }
    if (ctx->sound_effects[i].data != NULL) {
      SDL_free(ctx->sound_effects[i].data);
      ctx->sound_effects[i].data = NULL;
    }
  }
  if (ctx->audio_device) {
    SDL_CloseAudioDevice(ctx->audio_device);
  }
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
