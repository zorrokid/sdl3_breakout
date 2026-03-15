#include <stdio.h>
#include "textures.h"
#include "main.h"

void render_score(struct GameContext *ctx) {
  if (!ctx->font) {
    SDL_Log("Font not loaded, cannot render score.");
    return;
  }
  if (ctx->score != ctx->last_score || ctx->score_texture == NULL) {
    if (ctx->score_texture) {
      SDL_DestroyTexture(ctx->score_texture);
    }
    char score_text[32];
    snprintf(score_text, sizeof(score_text), "SCORE: %d", ctx->score);

    SDL_Color white = {255, 255, 255, 255};
    ctx->score_texture =
        create_text_texture(ctx->renderer, ctx->font, score_text, white);
    ctx->last_score = ctx->score;
  }
  if (ctx->score_texture) {
    float w, h;
    SDL_GetTextureSize(ctx->score_texture, &w, &h);
    SDL_FRect dst = {20.0f, 20.0f, w, h};
    SDL_RenderTexture(ctx->renderer, ctx->score_texture, NULL, &dst);
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
