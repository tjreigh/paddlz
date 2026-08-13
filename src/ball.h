#ifndef BALL_H
#define BALL_H

#include <stdbool.h>

#define FIXED_SHIFT 8
#define BALL_RADIUS 8
#define BALL_INIT_SPEED (2 << FIXED_SHIFT)
#define BALL_MAX_DEFLECT_SPEED (3 << FIXED_SHIFT)

typedef enum ball_direction_t
{
    BALL_LEFT = -1,
    BALL_RIGHT = 1
} ball_direction_t;

typedef enum ball_update_t
{
    BALL_IN_PLAY,
    BALL_OUT_LEFT,
    BALL_OUT_RIGHT
} ball_update_t;

typedef enum ball_boundary_t
{
    BALL_GOAL_ON_RIGHT,
    BALL_WALL_ON_RIGHT
} ball_boundary_t;

typedef struct ball_t
{
    int x;
    int y;
    int vx;
    int vy;
    bool in_play;
} ball_t;

void initBall(ball_t* ball);
void advanceBall(ball_t* ball);
ball_update_t updateBall(ball_t* ball, ball_boundary_t right_boundary);
void drawBall(ball_t* ball);
void resetBall(ball_t* ball);
void serveBall(ball_t* ball, ball_direction_t direction);

#endif
