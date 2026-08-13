#include <graphx.h>

#include "paddle.h"
#include "draw.h"

bool paddleIsVertical(paddle_side_t side)
{
	return side == PADDLE_LEFT || side == PADDLE_RIGHT;
}

void initPaddle(paddle_t *paddle, paddle_side_t side)
{
	int x = side == PADDLE_LEFT
		? PADDLE_MARGIN
		: SCREEN_WIDTH - PADDLE_MARGIN - PADDLE_WIDTH;
	point_t init_pos = {x, PADDLE_INIT_Y};
	paddle->pos = init_pos;
	paddle->side = side;
}

void initBattlePaddle(paddle_t *paddle, paddle_side_t side)
{
	point_t init_pos;

	switch (side)
	{
		case PADDLE_LEFT:
			init_pos.x = BATTLE_LEFT + PADDLE_MARGIN;
			init_pos.y = PADDLE_INIT_Y;
			break;
		case PADDLE_RIGHT:
			init_pos.x = BATTLE_RIGHT - PADDLE_MARGIN - PADDLE_WIDTH;
			init_pos.y = PADDLE_INIT_Y;
			break;
		case PADDLE_TOP:
			init_pos.x = (BATTLE_LEFT + BATTLE_RIGHT) / 2 - PADDLE_LEN / 2;
			init_pos.y = BATTLE_TOP + PADDLE_MARGIN;
			break;
		case PADDLE_BOTTOM:
		default:
			init_pos.x = (BATTLE_LEFT + BATTLE_RIGHT) / 2 - PADDLE_LEN / 2;
			init_pos.y = BATTLE_BOTTOM - PADDLE_MARGIN - PADDLE_WIDTH;
			break;
	}

	paddle->pos = init_pos;
	paddle->side = side;
}

void movePaddle(paddle_t *paddle, dir_t direction, int amount)
{
	int movement = direction == UP ? -amount : amount;

	if (paddleIsVertical(paddle->side))
	{
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
	else
	{
		int new_pos = paddle->pos.x + movement;

		if (new_pos < BATTLE_LEFT)
		{
			new_pos = BATTLE_LEFT;
		}
		else if (new_pos + PADDLE_LEN > BATTLE_RIGHT)
		{
			new_pos = BATTLE_RIGHT - PADDLE_LEN;
		}

		paddle->pos.x = new_pos;
	}
}

void movePaddleToward(paddle_t *paddle, int target, int amount)
{
	bool vertical = paddleIsVertical(paddle->side);
	int paddle_center = (vertical ? paddle->pos.y : paddle->pos.x) + PADDLE_LEN / 2;

	if (target < paddle_center - PADDLE_TRACKING_DEAD_ZONE)
	{
		movePaddle(paddle, UP, amount);
	}
	else if (target > paddle_center + PADDLE_TRACKING_DEAD_ZONE)
	{
		movePaddle(paddle, DOWN, amount);
	}
}

void drawPaddle(paddle_t *paddle)
{
	gfx_SetColor(PADDLE_COLOR);

	if (paddleIsVertical(paddle->side))
	{
		gfx_FillRectangle_NoClip(paddle->pos.x, paddle->pos.y, PADDLE_WIDTH, PADDLE_LEN);
	}
	else
	{
		gfx_FillRectangle_NoClip(paddle->pos.x, paddle->pos.y, PADDLE_LEN, PADDLE_WIDTH);
	}
}
