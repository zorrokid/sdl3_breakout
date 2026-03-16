#ifndef SFX_H
#define SFX_H
#include "common.h"

bool init_audio(GameContext *ctx);
bool cleanup_audio(GameContext *ctx);
bool load_sfx(GameContext *ctx, SoundID id, const char *path,
              SDL_AudioDeviceID device);
void play_sfx(GameContext *ctx, SoundID id);

#endif // SFX_H
