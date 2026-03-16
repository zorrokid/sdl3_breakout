#ifndef PARTICLES_H
#define PARTICLES_H
#include "common.h"

void spawn_brick_burst(Particle *particles, Brick *brick, SDL_Color color);
void update_particles(Particle *particles, float delta_time);
void render_particles(SDL_Renderer *renderer, Particle *particles);

#endif // PARTICLES_H
