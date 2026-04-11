#include "particles.h"
#include <stdio.h>
#include <stdlib.h>

void spawn_brick_burst(Particle *particles, SDL_Color color, Coord location) {
  int spawned = 0;

  for (int i = 0; i < MAX_PARTICLES && spawned < 15; i++) {
    if (!particles[i].active) {
      particles[i].active = true;
      particles[i].rect =
          (SDL_FRect){location.x, location.y, 12.0f, 12.0f}; // Tiny squares

      // Random direction and speed
      // (rand() % 200 - 100) gives a range of -100 to 100
      particles[i].dx = (float)((rand() % 200) - 100);
      particles[i].dy = (float)((rand() % 200) - 100);

      particles[i].life_time =
          0.5f + ((rand() % 50) / 100.0f); // 0.5 to 1.0 seconds
      particles[i].color = color;
      spawned++;
    }
  }
}

void update_particles(Particle *particles, float delta_time) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      particles[i].rect.x += particles[i].dx * delta_time;
      particles[i].rect.y += particles[i].dy * delta_time;

      particles[i].life_time -= delta_time;

      if (particles[i].life_time <= 0) {
        particles[i].active = false; // Deactivate when life time is up
      }
    }
  }
}

void render_particles(SDL_Renderer *renderer, Particle *particles) {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      Uint8 alpha =
          (Uint8)((particles[i].life_time) * 255); // Fade out over time
      SDL_SetRenderDrawColor(renderer, particles[i].color.r,
                             particles[i].color.g, particles[i].color.b, alpha);
      SDL_RenderFillRect(renderer, &particles[i].rect);
    }
  }
}
