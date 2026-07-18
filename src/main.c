#include <stdbool.h>

#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

#include "main.h"
#include "paddle.h"
#include "draw.h"
#include "ball.h"
#include "collision.h"

bool gameActive = true;
paddle_t* paddle;
ball_t* ball;

void render(void);

int main(void)
{
	os_ClrHome();
	gfx_Begin();
	gfx_SetDrawBuffer();

	paddle = initPaddle();
	ball = initBall();

	serveBall(ball);

	do
	{
		updateKeyboard();
		updatePaddle(paddle);
		updateBall(ball);
		bool ballPaddleCollision = checkPaddleCollision(ball, paddle);
		render();
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
	free(ball);

	exit(0);
}

void render(void)
{
	// Clear screen (320x240 for TI-84 CE)
	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, 320, 240);

	// Draw all entities
	drawPaddle(paddle);
	drawBall(ball);

	// Display everything
	gfx_BlitBuffer();
}