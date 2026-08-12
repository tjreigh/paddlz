#include <graphx.h>

#include "paddle.h"
#include "draw.h"

void initPaddle(paddle_t *paddle, paddle_side_t side)
{
	int x = side == PADDLE_LEFT
		? PADDLE_MARGIN
		: SCREEN_WIDTH - PADDLE_MARGIN - PADDLE_WIDTH;
	point_t init_pos = {x, PADDLE_INIT_Y};
	paddle->pos = init_pos;
	paddle->side = side;
}

void movePaddle(paddle_t *paddle, dir_t direction, int amount)
{
	int movement = direction == UP ? -amount : amount;
	int new_pos = paddle->pos.y + movement;

	if (new_pos < PLAYFIELD_TOP)
	{
		new_pos = PLAYFIELD_TOP;
	}
	else if (new_pos + PADDLE_LEN > PLAYFIELD_BOTTOM)
	{
		new_pos = PLAYFIELD_BOTTOM - PADDLE_LEN;
	}

	paddle->pos.y = new_pos;
}

void movePaddleToward(paddle_t *paddle, int target_y, int amount)
{
	int paddle_center = paddle->pos.y + PADDLE_LEN / 2;

	if (target_y < paddle_center - PADDLE_TRACKING_DEAD_ZONE)
	{
		movePaddle(paddle, UP, amount);
	}
	else if (target_y > paddle_center + PADDLE_TRACKING_DEAD_ZONE)
	{
		movePaddle(paddle, DOWN, amount);
	}
}

void drawPaddle(paddle_t *paddle)
{
	gfx_SetColor(PADDLE_COLOR);
	gfx_FillRectangle_NoClip(paddle->pos.x, paddle->pos.y, PADDLE_WIDTH, PADDLE_LEN);
}
