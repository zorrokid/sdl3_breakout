#ifndef TEXTURES_H
#define TEXTURES_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
struct GameContext;
void render_score(struct GameContext *ctx);

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font,
                                 const char *text, SDL_Color color);

#endif // TEXTURES_H
