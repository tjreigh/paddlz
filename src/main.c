#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

#include "main.h"
#include "paddle.h"
#include "draw.h"
#include "ball.h"
#include "collision.h"

static bool gameActive = true;
static bool secondWasDown = false;
static paddle_t paddle;
static ball_t ball;
static unsigned int score = 0;
static unsigned int bestScore = 0;

void render(void);

int main(void)
{
	os_ClrHome();
	gfx_Begin();
	gfx_SetDrawBuffer();

	initPaddle(&paddle);
	initBall(&ball);

	do
	{
		updateKeyboard();
		if (!gameActive)
		{
			break;
		}

		updatePaddle(&paddle);
		if (updateBall(&ball))
		{
			score = 0;
			resetBall(&ball);
		}
		else if (checkPaddleCollision(&ball, &paddle))
		{
			score++;
			if (score > bestScore)
			{
				bestScore = score;
			}
		}
		render();
	} while (gameActive);

	gfx_End();
	return 0;
}

void updateKeyboard(void)
{
	kb_Scan();

	kb_key_t g1 = kb_Data[1];
	kb_key_t g6 = kb_Data[6];
	kb_key_t g7 = kb_Data[7];

	if (g7 & kb_Down)
	{
		paddle.should_move = true;
		paddle.move_dir = DOWN;
	}
	else if (g7 & kb_Up)
	{
		paddle.should_move = true;
		paddle.move_dir = UP;
	}

	bool secondIsDown = g1 & kb_2nd;
	if (secondIsDown && !secondWasDown && !ball.in_play)
	{
		serveBall(&ball);
	}
	secondWasDown = secondIsDown;

	if (g6 & kb_Clear)
	{
		quit();
	}
}

void quit(void)
{
	gameActive = false;
}

void render(void)
{
	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	drawPaddle(&paddle);
	drawBall(&ball);

	gfx_SetTextFGColor(PADDLE_COLOR);
	gfx_PrintStringXY("SCORE", 120, 8);
	gfx_SetTextXY(164, 8);
	gfx_PrintUInt(score, 1);
	gfx_PrintStringXY("BEST", 215, 8);
	gfx_SetTextXY(251, 8);
	gfx_PrintUInt(bestScore, 1);

	if (!ball.in_play)
	{
		gfx_PrintStringXY("2nd: serve", 120, 112);
	}

	gfx_SwapDraw();
}
