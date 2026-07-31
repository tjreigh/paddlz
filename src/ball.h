#ifndef BALL_H
#define BALL_H

#include <stdbool.h>

#define FIXED_SHIFT 8
#define BALL_RADIUS 8
#define BALL_INIT_SPEED (2 << FIXED_SHIFT)
#define BALL_MAX_VERTICAL_SPEED (3 << FIXED_SHIFT)

typedef struct ball_t
{
    int x;
    int y;
    int vx;
    int vy;
    bool in_play;
} ball_t;

void initBall(ball_t* ball);
bool updateBall(ball_t* ball);
void drawBall(ball_t* ball);
void resetBall(ball_t* ball);
void serveBall(ball_t* ball);

#endif
