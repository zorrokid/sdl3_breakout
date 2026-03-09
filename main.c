#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// 1. The "Brain" of our game - holds all our data
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    SDL_FRect paddle;
} GameContext;

// 2. Setup: This runs exactly once when the program starts
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Allocate memory for our game data
    GameContext *ctx = SDL_calloc(1, sizeof(GameContext));
    if (!ctx)
        return SDL_APP_FAILURE;
    *appstate = ctx;

    // Create a 800x600 window and a renderer with VSync
    if (!SDL_CreateWindowAndRenderer("Breakout SDL3", 800, 600, 0, &ctx->window, &ctx->renderer)) {
        return SDL_APP_FAILURE;
    }

    // Enable VSync so we don't melt the GPU
    SDL_SetRenderVSync(ctx->renderer, 1);

    // Initialize paddle
    ctx->paddle.w = 100.0f;
    ctx->paddle.h = 20.0f;
    ctx->paddle.x = (800.0f - ctx->paddle.w) / 2.0f; // Center it
    ctx->paddle.y = 560.0f;                          // Near the bottom

    return SDL_APP_CONTINUE;
}

// 3. Events: Handle "X" button or keys
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // This exits the app cleanly
    }
    return SDL_APP_CONTINUE;
}

// 4. The Loop: This runs every single frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    GameContext *ctx = (GameContext *)appstate;

    // A. Clear the screen (Black)
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    // B. Draw the paddle
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255); // white
    SDL_RenderFillRect(ctx->renderer, &ctx->paddle);

    // C. Show the result on screen
    SDL_RenderPresent(ctx->renderer);

    return SDL_APP_CONTINUE;
}

// 5. Cleanup: Runs when the app closes
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    GameContext *ctx = (GameContext *)appstate;
    if (ctx) {
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        SDL_free(ctx);
    }
}
