#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include "ball.h"
#include "bricks.h"
#include "common.h"
#include "paddle.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    Paddle paddle;
    Ball ball;
    bool ball_launched;

    bool left_pressed;
    bool right_pressed;

    Brick bricks[BRICK_ROWS * BRICK_COLS];
} GameContext;

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
    if (!SDL_CreateWindowAndRenderer("Breakout SDL3", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &ctx->window,
                                     &ctx->renderer)) {
        return SDL_APP_FAILURE;
    }

    // Enable VSync so we don't melt the GPU
    SDL_SetRenderVSync(ctx->renderer, 1);

    init_paddle(&ctx->paddle);
    init_ball(&ctx->ball);
    init_bricks(ctx->bricks);

    ctx->ball_launched = false;

    return SDL_APP_CONTINUE;
}

// 3. Events: Handle "X" button or keys
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    GameContext *ctx = (GameContext *)appstate;

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // This exits the app cleanly
    }

    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
        bool is_down = (event->type == SDL_EVENT_KEY_DOWN);

        switch (event->key.key) {
        case SDLK_LEFT:
            ctx->left_pressed = is_down;
            break;
        case SDLK_RIGHT:
            ctx->right_pressed = is_down;
            break;
        case SDLK_SPACE:
            if (is_down)
                ctx->ball_launched = true;
            break;
        case SDLK_ESCAPE:
            return SDL_APP_SUCCESS;
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

// 4. The Loop: This runs every single frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    GameContext *ctx = (GameContext *)appstate;

    move_paddle(&ctx->paddle, ctx->left_pressed, ctx->right_pressed);
    move_ball(&ctx->ball, &ctx->paddle, &ctx->ball_launched);
    check_ball_brick_collision(&ctx->ball, ctx->bricks);

    // A. Clear the screen (Black)
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    render_paddle(ctx->renderer, &ctx->paddle);
    render_ball(ctx->renderer, &ctx->ball);
    render_bricks(ctx->renderer, ctx->bricks);

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
