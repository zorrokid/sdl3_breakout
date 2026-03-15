#ifndef TEXTURES_H
#define TEXTURES_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
struct GameContext;
void update_score_texture(struct GameContext *ctx);
void update_lives_texture(struct GameContext *ctx);
void render_score(struct GameContext *ctx);
void render_lives(struct GameContext *ctx);
void render_text_centered(struct GameContext *ctx, const char *text, float y,
                          SDL_Color color, int font_size);

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font,
                                 const char *text, SDL_Color color);

#endif // TEXTURES_H
