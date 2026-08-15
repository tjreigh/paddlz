#ifndef PADDLE_H
#define PADDLE_H

#include <stdbool.h>

#define PADDLE_WIDTH 8
#define PADDLE_LEN 60
#define PADDLE_MARGIN 2
#define PADDLE_INIT_Y 90
#define PLAYER_PADDLE_SPEED 2
#define PADDLE_TRACKING_DEAD_ZONE 4

typedef struct point_t
{
	int x;
	int y;
} point_t;

typedef enum dir_t
{
	UP,
	DOWN
} dir_t;

typedef enum paddle_side_t
{
	PADDLE_LEFT,
	PADDLE_RIGHT,
	PADDLE_TOP,
	PADDLE_BOTTOM
} paddle_side_t;

typedef struct paddle_t
{
	point_t pos;
	paddle_side_t side;
}
paddle_t;

bool paddleIsVertical(paddle_side_t);
void initPaddle(paddle_t*, paddle_side_t);
void initBattlePaddle(paddle_t*, paddle_side_t);
void movePaddle(paddle_t*, dir_t, int);
void movePaddleToward(paddle_t*, int, int);
void drawPaddle(paddle_t*);

#endif
