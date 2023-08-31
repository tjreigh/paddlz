#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

#include "main.h"
#include "paddle.h"
#include "draw.h"

bool gameActive = true;
paddle_t* paddle;

int main(void)
{
	os_ClrHome();
	gfx_Begin();

	paddle = initPaddle();

	do
	{
		updateKeyboard();
		updatePaddle(paddle);
	} while (gameActive);

	return 0;
}

void updateKeyboard()
{
	kb_key_t g1 = kb_Data[1];
	kb_key_t g7 = kb_Data[7];

	kb_Scan();

	if (g7 & kb_Down)
	{
		paddle->should_move = true;
		paddle->move_dir = DOWN;
	}
	else if (g7 & kb_Up)
	{
		paddle->should_move = true;
		paddle->move_dir = UP;
	}

	if (g1 & kb_2nd)
	{
		quit();
	}
}

void quit()
{
	gameActive = false;
	gfx_End();

	free(paddle);

	exit(0);
}