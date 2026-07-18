#include <stdlib.h>
#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <debug.h>

#include "paddle.h"
#include "draw.h"

paddle_t *initPaddle()
{
	dbg_printf("init\n");
	paddle_t *paddle = calloc(1, sizeof(paddle_t));
	point_t init_pos = {INIT_X_LOC, INIT_Y_LOC};
	paddle->pos = init_pos;
	paddle->is_colliding = false;

	return paddle;
}

void movePaddle(paddle_t *paddle)
{
	int amnt = (paddle->move_dir == 0) ? -2 : 2;
	int curr_pos = paddle->pos.y;
	int new_pos = curr_pos + amnt;

	dbg_sprintf(dbgout, "current paddle y pos is: %d\n new position would be: %d\n", curr_pos, new_pos);

	// Check boundaries (paddle length is PADDLE_LEN)
	if (new_pos > 0 && new_pos + PADDLE_LEN < 240)
	{
		dbg_sprintf(dbgout, "moving paddle to y pos: %d\n", new_pos);
		paddle->pos.y = new_pos;
	}
	else
	{
		paddle->should_move = false;
	}
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