#include <stdio.h>
#include "textures.h"
#include "main.h"

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

void update_score_texture(GameContext *ctx) {
  if (!ctx->font) {
    SDL_Log("Font not loaded, cannot render score.");
    return;
  }

  if (ctx->score == ctx->last_score && ctx->score_texture)
    return;

  char score_text[32];
  snprintf(score_text, sizeof(score_text), "SCORE: %d", ctx->score);
  SDL_Color white = {255, 255, 255, 255};
  SDL_Texture *new_texture =
      create_text_texture(ctx->renderer, ctx->font, score_text, white);

  if (new_texture) {
    // Destroy the old texture
    if (ctx->score_texture) {
      SDL_DestroyTexture(ctx->score_texture);
    }
    ctx->score_texture = new_texture;
    ctx->last_score = ctx->score;
  } else {
    SDL_Log("Failed to create score texture.");
  }
}

void render_score(GameContext *ctx) {
  // Render the score texture if it exists
  if (ctx->score_texture) {
    float w, h;
    SDL_GetTextureSize(ctx->score_texture, &w, &h);
    SDL_FRect dst = {20.0f, 20.0f, w, h};
    SDL_RenderTexture(ctx->renderer, ctx->score_texture, NULL, &dst);
  }
}

void update_lives_texture(GameContext *ctx) {
  if (!ctx->font) {
    SDL_Log("Font not loaded, cannot render score.");
    return;
  }

  if (ctx->lives == ctx->last_lives && ctx->lives_texture)
    return;

  char lives_text[32];
  snprintf(lives_text, sizeof(lives_text), "LIVES: %d", ctx->lives);

  SDL_Color white = {255, 255, 255, 255};

  SDL_Texture *new_texture =
      create_text_texture(ctx->renderer, ctx->font, lives_text, white);

  if (new_texture) {
    // Destroy the old texture
    if (ctx->lives_texture) {
      SDL_DestroyTexture(ctx->lives_texture);
    }
    ctx->lives_texture = new_texture;
    ctx->last_lives = ctx->lives;
  }
}

void render_lives(GameContext *ctx) {
  // Render the lives texture if it exists
  if (ctx->lives_texture) {
    float w, h;
    SDL_GetTextureSize(ctx->lives_texture, &w, &h);
    SDL_FRect dst = {400.0f, 20.0f, w, h};
    SDL_RenderTexture(ctx->renderer, ctx->lives_texture, NULL, &dst);
  }
}

void render_text_centered(GameContext *ctx, const char *text, float y,
                          SDL_Color color, int font_size) {
  SDL_Texture *texture =
      create_text_texture(ctx->renderer, ctx->font, text, color);
  if (texture) {
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_FRect dst = {(SCREEN_WIDTH - w) / 2.0f, y, w, h};
    SDL_RenderTexture(ctx->renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
  } else {
    SDL_Log("Failed to create texture for text: '%s'", text);
  }
}

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font,
                                 const char *text, SDL_Color color) {
  if (!text || text[0] == '\0') {
    SDL_Log("Empty text provided for texture creation.");
    return NULL;
  }

  // Render the text to a surface
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  if (!surface) {
    SDL_Log("Failed to create text surface");
    return NULL;
  }
  // Convert to texture
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  // Clean up the surface
  SDL_DestroySurface(surface);
  return texture;
}

void cleanup_textures(GameContext *ctx) {
  if (ctx->score_texture) {
    SDL_DestroyTexture(ctx->score_texture);
  }
  if (ctx->lives_texture) {
    SDL_DestroyTexture(ctx->lives_texture);
  }
  if (ctx->font) {
    TTF_CloseFont(ctx->font);
  }
  TTF_Quit();
}
