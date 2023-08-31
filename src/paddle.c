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

	gfx_SwapDraw();
	gfx_Wait();

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, 240, 240);
	gfx_SetColor(PADDLE_COLOR);
	gfx_FillRectangle_NoClip(INIT_X_LOC, INIT_Y_LOC, PADDLE_WIDTH, PADDLE_LEN);

	gfx_SwapDraw();
	gfx_Wait();

	return paddle;
}

void movePaddle(paddle_t *paddle)
{
	gfx_SwapDraw();
	gfx_Wait();

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, 240, 240);

	int amnt = (paddle->move_dir == 0) ? -2 : 2;
	int curr_pos = paddle->pos.y;
	int new_pos = curr_pos + amnt;

	dbg_sprintf(dbgout, "current paddle y pos is: %d\n new position would be: %d\n", curr_pos, new_pos);

	gfx_SetColor(PADDLE_COLOR);

	if (new_pos < 210 || new_pos > 1)
	{
		gfx_FillRectangle_NoClip(INIT_X_LOC, new_pos, PADDLE_WIDTH, PADDLE_LEN);
		dbg_sprintf(dbgout, "moving paddle to y pos: %d\n", new_pos);
		paddle->pos.y = new_pos;
	}
	else
		paddle->should_move = false;

	gfx_SwapDraw();
	gfx_Wait();
}

void updatePaddle(paddle_t *paddle)
{
	if (paddle->should_move)
	{
		movePaddle(paddle);
		paddle->should_move = false;
	}
}