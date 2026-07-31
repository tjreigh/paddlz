#include <stdbool.h>

#include <graphx.h>

#include "ball.h"
#include "draw.h"

void initBall(ball_t *ball)
{
    resetBall(ball);
}

bool updateBall(ball_t* ball)
{
    if (!ball->in_play) {
        return false;
    }

    // Update position
    ball->x += ball->vx;
    ball->y += ball->vy;

    // Convert to screen coordinates for collision checking
    int screen_x = ball->x >> FIXED_SHIFT;
    int screen_y = ball->y >> FIXED_SHIFT;

    // Bounce off top wall
    if (screen_y <= BALL_RADIUS) {
        ball->y = BALL_RADIUS << FIXED_SHIFT;
        if (ball->vy < 0) {
            ball->vy = -ball->vy;
        }
    }

    // Bounce off bottom wall (240 is screen height)
    if (screen_y >= SCREEN_HEIGHT - BALL_RADIUS) {
        ball->y = (SCREEN_HEIGHT - BALL_RADIUS) << FIXED_SHIFT;
        if (ball->vy > 0) {
            ball->vy = -ball->vy;
        }
    }

    // Leaving the left side ends the rally.
    if (screen_x + BALL_RADIUS < 0) {
        return true;
    }

    if (screen_x >= SCREEN_WIDTH - BALL_RADIUS) {
        ball->x = (SCREEN_WIDTH - BALL_RADIUS) << FIXED_SHIFT;
        if (ball->vx > 0) {
            ball->vx = -ball->vx;
        }
    }

    return false;
}

void drawBall(ball_t* ball)
{
    int screen_x = ball->x >> FIXED_SHIFT;
    int screen_y = ball->y >> FIXED_SHIFT;

    gfx_SetColor(BALL_COLOR);
    gfx_FillCircle(screen_x, screen_y, BALL_RADIUS);
}

void resetBall(ball_t* ball)
{
    ball->x = (SCREEN_WIDTH / 2) << FIXED_SHIFT;
    ball->y = (SCREEN_HEIGHT / 2) << FIXED_SHIFT;
    ball->vx = BALL_INIT_SPEED;
    ball->vy = BALL_INIT_SPEED;
    ball->in_play = false;
}

void serveBall(ball_t* ball)
{
    ball->in_play = true;
}
