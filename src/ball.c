#include <stdbool.h>

#include <graphx.h>

#include "ball.h"
#include "draw.h"

void initBall(ball_t *ball)
{
    resetBall(ball);
}

void advanceBall(ball_t* ball)
{
    ball->x += ball->vx;
    ball->y += ball->vy;
}

ball_update_t updateBall(ball_t* ball, ball_boundary_t right_boundary)
{
    if (!ball->in_play) {
        return BALL_IN_PLAY;
    }

    advanceBall(ball);

    // Convert to screen coordinates for collision checking
    int screen_x = ball->x >> FIXED_SHIFT;
    int screen_y = ball->y >> FIXED_SHIFT;

    // Bounce off top wall
    if (screen_y <= PLAYFIELD_TOP + BALL_RADIUS) {
        ball->y = (PLAYFIELD_TOP + BALL_RADIUS) << FIXED_SHIFT;
        if (ball->vy < 0) {
            ball->vy = -ball->vy;
        }
    }

    if (screen_y >= PLAYFIELD_BOTTOM - BALL_RADIUS) {
        ball->y = (PLAYFIELD_BOTTOM - BALL_RADIUS) << FIXED_SHIFT;
        if (ball->vy > 0) {
            ball->vy = -ball->vy;
        }
    }

    // Leaving the left side ends the rally.
    if (screen_x + BALL_RADIUS < 0) {
        return BALL_OUT_LEFT;
    }

    if (right_boundary == BALL_WALL_ON_RIGHT
        && screen_x >= SCREEN_WIDTH - BALL_RADIUS) {
        ball->x = (SCREEN_WIDTH - BALL_RADIUS) << FIXED_SHIFT;
        if (ball->vx > 0) {
            ball->vx = -ball->vx;
        }
    } else if (screen_x - BALL_RADIUS > SCREEN_WIDTH) {
        return BALL_OUT_RIGHT;
    }

    return BALL_IN_PLAY;
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

void serveBall(ball_t* ball, ball_direction_t direction)
{
    ball->vx = direction * BALL_INIT_SPEED;
    ball->in_play = true;
}
