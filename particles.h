#ifndef PARTICLES_H
#define PARTICLES_H
#include <SDL3/SDL.h>

#define MAX_PARTICLES 200

typedef struct {
  SDL_FRect rect;
  float dx, dy;
  float life_time; // Time since the particle was created
  SDL_Color color;
  bool active;
} Particle;

struct Brick;

void spawn_brick_burst(Particle *particles, struct Brick *brick,
                       SDL_Color color);
void update_particles(Particle *particles, float delta_time);
void render_particles(SDL_Renderer *renderer, Particle *particles);

#endif // PARTICLES_H
