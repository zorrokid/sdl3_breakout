#include "bricks.h"
#include "common.h"

void init_bricks(Brick bricks[]) {
    float total_width = (BRICK_COLS * (BRICK_WIDTH + BRICK_PADDING)) - BRICK_PADDING;
    // Center the bricks horizontally
    float start_x = (SCREEN_WIDTH - total_width) / 2.0f;
    float start_y = 50.0f; // Start 50 pixels from the top

    for (int row = 0; row < BRICK_ROWS; ++row) {
        for (int col = 0; col < BRICK_COLS; ++col) {
            int i = row * BRICK_COLS + col;
            bricks[i].rect.x = start_x + (col * (BRICK_WIDTH + BRICK_PADDING));
            bricks[i].rect.y = start_y + (row * (BRICK_HEIGHT + BRICK_PADDING));
            bricks[i].rect.w = BRICK_WIDTH;
            bricks[i].rect.h = BRICK_HEIGHT;
            bricks[i].active = true;
        }
    }
}

void render_bricks(SDL_Renderer *renderer, Brick bricks[]) {
    for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
        if (bricks[i].active) {
            int row = i / BRICK_COLS;
            // Color based on row
            SDL_SetRenderDrawColor(renderer, 255 - (row * 40), 0, row * 40, 255);
            SDL_RenderFillRect(renderer, &bricks[i].rect);
        }
    }
}

void check_ball_brick_collision(Ball *ball, Brick bricks[]) {
    for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
        if (bricks[i].active && SDL_HasRectIntersectionFloat(&ball->rect, &bricks[i].rect)) {
            bricks[i].active = false; // Deactivate the brick
            ball->dy *= -1.0f;        // Reverse the ball's vertical direction
            break;                    // Only handle one collision per frame
        }
    }
}
