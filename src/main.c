#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

#include "paddle.h"
#include "draw.h"

bool gameActive = true;
bool move = false;
dir_t moveDir;

void quit() {
	gameActive = false;
	gfx_End();
	exit(0);
}

void updateKeyboard()
{
	kb_key_t g1 = kb_Data[1];
	kb_key_t g7 = kb_Data[7];

	kb_Scan();

	if (g7 & kb_Down) {
		move = true;
		moveDir = DOWN;
	} else if (g7 & kb_Up) {
		move = true;
		moveDir = UP;
	}

	if (g1 & kb_2nd) {
		quit();
	}
}

int main(void)
{
	point_t *paddle = NULL;

	os_ClrHome();

	gfx_Begin();

	paddle = calloc(1, sizeof(point_t));
	initPaddle(paddle);

	do
	{
		updateKeyboard();
		updatePaddle(paddle);
	} while (gameActive);

	return 0;
}