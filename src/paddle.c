#include <stdlib.h>
#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <debug.h>

#include "paddle.h"

#define BG_COLOR     255
#define PADDLE_COLOR 0

void initPaddle(point_t *point)
{
	dbg_printf("init\n");
	gfx_SwapDraw();

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, 240, 240);

	point->y = INIT_Y_LOC;

	gfx_SetColor(PADDLE_COLOR);
	gfx_FillRectangle_NoClip(INIT_X_LOC, point->y, PADDLE_WIDTH, PADDLE_LEN);

	gfx_SwapDraw();
}

void movePaddle(dir_t d, point_t *point)
{
	gfx_SwapDraw();

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, 240, 240);

	int amnt = (d == 0) ? -2 : 2;

	point->y = point->y + amnt;

	gfx_SetColor(PADDLE_COLOR);
	gfx_FillRectangle_NoClip(INIT_X_LOC, point->y, PADDLE_WIDTH, PADDLE_LEN);

	gfx_SwapDraw();
}

void updatePaddle()
{
	
}