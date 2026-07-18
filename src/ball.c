#include <stdlib.h>
#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <debug.h>

#include "ball.h"
#include "draw.h"

ball_t *initBall()
{
    dbg_printf("init ball\n");
    ball_t *ball = calloc(1, sizeof(ball_t));
    ball->x = 120 << FIXED_SHIFT;
    ball->y = 120 << FIXED_SHIFT;
    ball->vx = BALL_INIT_SPEED;
    ball->vy = BALL_INIT_SPEED;
    ball->in_play = false;

    return ball;
}

void updateBall(ball_t* ball)
{
    if (!ball->in_play) {
        return;
    }

    // Update position
    ball->x += ball->vx;
    ball->y += ball->vy;

    // Convert to screen coordinates for collision checking
    int screen_x = ball->x >> FIXED_SHIFT;
    int screen_y = ball->y >> FIXED_SHIFT;

    // Bounce off top wall
    if (screen_y <= BALL_SIZE) {
        ball->y = BALL_SIZE << FIXED_SHIFT;
        ball->vy = -ball->vy;
        if (ball->vy < 0) ball->vy = -ball->vy;  // Ensure moving down
    }

    // Bounce off bottom wall (240 is screen height)
    if (screen_y >= 240 - BALL_SIZE) {
        ball->y = (240 - BALL_SIZE) << FIXED_SHIFT;
        ball->vy = -ball->vy;
        if (ball->vy > 0) ball->vy = -ball->vy;  // Ensure moving up
    }

    // Bounce off left wall
    if (screen_x <= BALL_SIZE) {
        ball->x = BALL_SIZE << FIXED_SHIFT;
        ball->vx = -ball->vx;
        if (ball->vx < 0) ball->vx = -ball->vx;  // Ensure moving right
    }

    // Bounce off right wall (320 is screen width)
    if (screen_x >= 320 - BALL_SIZE) {
        ball->x = (320 - BALL_SIZE) << FIXED_SHIFT;
        ball->vx = -ball->vx;
        if (ball->vx > 0) ball->vx = -ball->vx;  // Ensure moving left
    }
}

void drawBall(ball_t* ball)
{
    int screen_x = ball->x >> FIXED_SHIFT;
    int screen_y = ball->y >> FIXED_SHIFT;

    gfx_SetColor(BALL_COLOR);
    gfx_FillCircle(screen_x, screen_y, BALL_SIZE);
}

void resetBall(ball_t* ball)
{
    ball->x = 120 << FIXED_SHIFT;
    ball->y = 120 << FIXED_SHIFT;
    ball->vx = BALL_INIT_SPEED;
    ball->vy = BALL_INIT_SPEED;
    ball->in_play = false;
}

void serveBall(ball_t* ball)
{
    ball->in_play = true;
}