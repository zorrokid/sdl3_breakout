#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 100.0f
#define PADDLE_HEIGHT 20.0f
#define PADDLE_Y 560.0f
#define PADDLE_SPEED 7.0f
#define BALL_SIZE 16.0f

#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 75.0f
#define BRICK_HEIGHT 20.0f
#define BRICK_PADDING 5.0f

typedef struct {
    SDL_FRect rect;
    float dx;
    float dy;
} Ball;

typedef struct {
    SDL_FRect rect;
    bool active;
} Brick;

// 1. The "Brain" of our game - holds all our data
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    SDL_FRect paddle;
    Ball ball;
    bool ball_launched;

    bool left_pressed;
    bool right_pressed;

    Brick bricks[BRICK_ROWS * BRICK_COLS];
} GameContext;

void init_bricks(GameContext *ctx) {
    float total_width = (BRICK_COLS * (BRICK_WIDTH + BRICK_PADDING)) - BRICK_PADDING;
    // Center the bricks horizontally
    float start_x = (SCREEN_WIDTH - total_width) / 2.0f;
    float start_y = 50.0f; // Start 50 pixels from the top

    for (int row = 0; row < BRICK_ROWS; ++row) {
        for (int col = 0; col < BRICK_COLS; ++col) {
            int i = row * BRICK_COLS + col;
            ctx->bricks[i].rect.x = start_x + (col * (BRICK_WIDTH + BRICK_PADDING));
            ctx->bricks[i].rect.y = start_y + (row * (BRICK_HEIGHT + BRICK_PADDING));
            ctx->bricks[i].rect.w = BRICK_WIDTH;
            ctx->bricks[i].rect.h = BRICK_HEIGHT;
            ctx->bricks[i].active = true;
        }
    }
}

void render_bricks(GameContext *ctx) {
    for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
        if (ctx->bricks[i].active) {
            int row = i / BRICK_COLS;
            // Color based on row
            SDL_SetRenderDrawColor(ctx->renderer, 255 - (row * 40), 0, row * 40, 255);
            SDL_RenderFillRect(ctx->renderer, &ctx->bricks[i].rect);
        }
    }
}

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
    if (!SDL_CreateWindowAndRenderer("Breakout SDL3", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &ctx->window,
                                     &ctx->renderer)) {
        return SDL_APP_FAILURE;
    }

    // Enable VSync so we don't melt the GPU
    SDL_SetRenderVSync(ctx->renderer, 1);

    // Initialize paddle
    ctx->paddle.w = PADDLE_WIDTH;
    ctx->paddle.h = PADDLE_HEIGHT;
    ctx->paddle.x = (SCREEN_WIDTH - ctx->paddle.w) / 2.0f; // Center it
    ctx->paddle.y = PADDLE_Y;                              // Near the bottom

    ctx->ball.rect = (SDL_FRect){0, 0, BALL_SIZE, BALL_SIZE};
    ctx->ball.dx = 4.0f;
    ctx->ball.dy = -4.0f;
    ctx->ball_launched = false;

    init_bricks(ctx);

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

    // Movement and collision

    // Move the paddle
    float paddle_speed = PADDLE_SPEED; // TODO: add delta time
    if (ctx->left_pressed)
        ctx->paddle.x -= paddle_speed;
    if (ctx->right_pressed)
        ctx->paddle.x += paddle_speed;

    // Move the ball
    if (!ctx->ball_launched) {
        // put the ball in the middle of the paddle
        ctx->ball.rect.x = ctx->paddle.x + (ctx->paddle.w / 2.0f) - (ctx->ball.rect.w / 2.0f);
        ctx->ball.rect.y = ctx->paddle.y - ctx->ball.rect.h - 1.0f; // just above the paddle

    } else {
        // Move the ball
        ctx->ball.rect.x += ctx->ball.dx;
        ctx->ball.rect.y += ctx->ball.dy;

        // Wall collision

        if (ctx->ball.rect.x <= 0 || ctx->ball.rect.x >= SCREEN_WIDTH - ctx->ball.rect.w)
            ctx->ball.dx *= -1;
        if (ctx->ball.rect.y <= 0)
            ctx->ball.dy *= -1;

        // Paddle collision
        if (SDL_HasRectIntersectionFloat(&ctx->ball.rect, &ctx->paddle)) {
            // Reverse the direction
            ctx->ball.dy *= -1.0f;

            // snap the ball to the top of the paddle to prevent sticking
            ctx->ball.rect.y = ctx->paddle.y - ctx->ball.rect.h - 1.0f;
        }

        if (ctx->ball.rect.y > SCREEN_HEIGHT) {
            // Ball fell below the screen, reset it
            ctx->ball_launched = false;
        }
    }

    // Collision uses the rect inside the ball struct
    if (SDL_HasRectIntersectionFloat(&ctx->ball.rect, &ctx->paddle)) {
        ctx->ball.dy *= -1.0f;
        ctx->ball.rect.y = ctx->paddle.y - ctx->ball.rect.h;
    }

    // Check screen boundaries
    if (ctx->paddle.x < 0)
        ctx->paddle.x = 0;
    if (ctx->paddle.x > SCREEN_WIDTH - ctx->paddle.w)
        ctx->paddle.x = SCREEN_WIDTH - ctx->paddle.w;

    // A. Clear the screen (Black)
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    // B. Draw the paddle
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255); // white
    SDL_RenderFillRect(ctx->renderer, &ctx->paddle);

    // Draw ball
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 0, 255); // yellow
    SDL_RenderFillRect(ctx->renderer, &ctx->ball.rect);

    render_bricks(ctx);

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
