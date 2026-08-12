#ifndef PADDLE_H
#define PADDLE_H

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
	PADDLE_RIGHT
} paddle_side_t;

typedef struct paddle_t
{
	point_t pos;
	paddle_side_t side;
}
paddle_t;

void initPaddle(paddle_t*, paddle_side_t);
void movePaddle(paddle_t*, dir_t, int);
void movePaddleToward(paddle_t*, int, int);
void drawPaddle(paddle_t*);

#endif
