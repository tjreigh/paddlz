#include <graphx.h>

#include "paddle.h"
#include "draw.h"

void initPaddle(paddle_t *paddle)
{
	point_t init_pos = {INIT_X_LOC, INIT_Y_LOC};
	paddle->pos = init_pos;
	paddle->move_dir = UP;
	paddle->should_move = false;
}

void movePaddle(paddle_t *paddle)
{
	int amnt = (paddle->move_dir == UP) ? -PADDLE_SPEED : PADDLE_SPEED;
	int curr_pos = paddle->pos.y;
	int new_pos = curr_pos + amnt;

	if (new_pos < 0)
	{
		new_pos = 0;
	}
	else if (new_pos + PADDLE_LEN > SCREEN_HEIGHT)
	{
		new_pos = SCREEN_HEIGHT - PADDLE_LEN;
	}

	paddle->pos.y = new_pos;
}

void updatePaddle(paddle_t *paddle)
{
	if (paddle->should_move)
	{
		movePaddle(paddle);
		paddle->should_move = false;
	}
}

void drawPaddle(paddle_t *paddle)
{
	gfx_SetColor(PADDLE_COLOR);
	gfx_FillRectangle_NoClip(paddle->pos.x, paddle->pos.y, PADDLE_WIDTH, PADDLE_LEN);
}
