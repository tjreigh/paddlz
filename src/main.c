#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <tice.h>

#include "ball.h"
#include "collision.h"
#include "cpu.h"
#include "draw.h"
#include "main.h"
#include "match.h"
#include "paddle.h"
#include "rally.h"
#include "save.h"

typedef enum screen_t
{
	MENU_SCREEN,
	GAME_SCREEN
} screen_t;

typedef enum game_mode_t
{
	CPU_MODE,
	HIGH_SCORE_MODE
} game_mode_t;

static bool gameActive = true;
static bool secondWasDown = false;
static bool upWasDown = false;
static bool downWasDown = false;
static bool leftWasDown = false;
static bool rightWasDown = false;
static bool clearWasDown = false;
static screen_t currentScreen = MENU_SCREEN;
static game_mode_t selectedMode = CPU_MODE;
static game_mode_t activeMode = CPU_MODE;
static cpu_difficulty_t selectedDifficulty = CPU_NORMAL;
static cpu_difficulty_t activeDifficulty = CPU_NORMAL;
static paddle_t playerPaddle;
static paddle_t cpuPaddle;
static ball_t ball;
static match_t match;
static rally_t rally;
static unsigned int savedHighScore = 0;

static void startSelectedMode(void);
static void updateGame(void);
static void render(void);
static void renderMenu(void);
static void renderModeOption(int y, const char* title, const char* description, bool selected);
static void renderGame(void);
static void renderServeOverlay(void);
static void renderMatchOverOverlay(void);
static void persistHighScoreIfNeeded(void);
static void beforeGarbageCollection(void);
static void afterGarbageCollection(void);

int main(void)
{
	os_ClrHome();
	gfx_Begin();
	gfx_SetDrawBuffer();
	ti_SetGCBehavior(beforeGarbageCollection, afterGarbageCollection);

	initPaddle(&playerPaddle, PADDLE_LEFT);
	initPaddle(&cpuPaddle, PADDLE_RIGHT);
	initBall(&ball);
	initMatch(&match);
	initRally(&rally);
	savedHighScore = loadHighScore();
	rally.high_score = savedHighScore;

	do
	{
		updateKeyboard();
		if (!gameActive)
		{
			break;
		}

		if (currentScreen == GAME_SCREEN)
		{
			updateGame();
		}
		render();
	} while (gameActive);

	gfx_End();
	return 0;
}

void updateKeyboard(void)
{
	kb_Scan();

	bool secondIsDown = kb_Data[1] & kb_2nd;
	bool clearIsDown = kb_Data[6] & kb_Clear;
	bool downIsDown = kb_Data[7] & kb_Down;
	bool leftIsDown = kb_Data[7] & kb_Left;
	bool rightIsDown = kb_Data[7] & kb_Right;
	bool upIsDown = kb_Data[7] & kb_Up;

	if (currentScreen == MENU_SCREEN)
	{
		if ((upIsDown && !upWasDown) || (downIsDown && !downWasDown))
		{
			selectedMode = selectedMode == CPU_MODE
				? HIGH_SCORE_MODE
				: CPU_MODE;
		}

		if (selectedMode == CPU_MODE && leftIsDown && !leftWasDown)
		{
			selectedDifficulty = selectedDifficulty == CPU_EASY
				? CPU_HARD
				: selectedDifficulty - 1;
		}
		else if (selectedMode == CPU_MODE && rightIsDown && !rightWasDown)
		{
			selectedDifficulty = selectedDifficulty == CPU_HARD
				? CPU_EASY
				: selectedDifficulty + 1;
		}

		if (secondIsDown && !secondWasDown)
		{
			startSelectedMode();
		}
		else if (clearIsDown && !clearWasDown)
		{
			quit();
		}
	}
	else
	{
		if (downIsDown)
		{
			movePaddle(&playerPaddle, DOWN, PLAYER_PADDLE_SPEED);
		}
		else if (upIsDown)
		{
			movePaddle(&playerPaddle, UP, PLAYER_PADDLE_SPEED);
		}

		if (secondIsDown && !secondWasDown && !ball.in_play)
		{
			if (activeMode == CPU_MODE && match.over)
			{
				initMatch(&match);
				initPaddle(&playerPaddle, PADDLE_LEFT);
				initPaddle(&cpuPaddle, PADDLE_RIGHT);
			}

			ball_direction_t direction = activeMode == CPU_MODE
				? match.next_serve_direction
				: BALL_RIGHT;
			serveBall(&ball, direction);
		}

		if (clearIsDown && !clearWasDown)
		{
			persistHighScoreIfNeeded();
			currentScreen = MENU_SCREEN;
			resetBall(&ball);
		}
	}

	secondWasDown = secondIsDown;
	upWasDown = upIsDown;
	downWasDown = downIsDown;
	leftWasDown = leftIsDown;
	rightWasDown = rightIsDown;
	clearWasDown = clearIsDown;
}

void quit(void)
{
	gameActive = false;
}

static void startSelectedMode(void)
{
	activeMode = selectedMode;
	activeDifficulty = selectedDifficulty;
	currentScreen = GAME_SCREEN;
	initPaddle(&playerPaddle, PADDLE_LEFT);
	initPaddle(&cpuPaddle, PADDLE_RIGHT);
	resetBall(&ball);

	if (activeMode == CPU_MODE)
	{
		initMatch(&match);
	}
	else
	{
		resetRallyScore(&rally);
	}
}

static void updateGame(void)
{
	if (activeMode == CPU_MODE && match.over)
	{
		return;
	}

	if (activeMode == CPU_MODE)
	{
		updateCpuPaddle(&cpuPaddle, &ball, activeDifficulty);
	}

	ball_boundary_t rightBoundary = activeMode == CPU_MODE
		? BALL_GOAL_ON_RIGHT
		: BALL_WALL_ON_RIGHT;
	ball_update_t ballUpdate = updateBall(&ball, rightBoundary);

	if (activeMode == CPU_MODE)
	{
		if (ballUpdate != BALL_IN_PLAY)
		{
			recordPoint(&match, ballUpdate);
			resetBall(&ball);
		}
		else
		{
			checkPaddleCollision(&ball, &playerPaddle);
			checkPaddleCollision(&ball, &cpuPaddle);
		}
	}
	else if (ballUpdate == BALL_OUT_LEFT)
	{
		persistHighScoreIfNeeded();
		resetRallyScore(&rally);
		resetBall(&ball);
	}
	else if (checkPaddleCollision(&ball, &playerPaddle))
	{
		recordRallyHit(&rally);
	}
}

static void render(void)
{
	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	gfx_SetTextFGColor(PADDLE_COLOR);

	if (currentScreen == MENU_SCREEN)
	{
		renderMenu();
	}
	else
	{
		renderGame();
	}

	gfx_SwapDraw();
}

static void renderMenu(void)
{
	gfx_SetTextScale(2, 2);
	gfx_PrintStringXY("Paddlz", 112, 34);
	gfx_SetTextScale(1, 1);
	gfx_PrintStringXY("CHOOSE A MODE", 108, 72);

	renderModeOption(
		92,
		"CPU MATCH",
		selectedMode == CPU_MODE
			? cpuDifficultyName(selectedDifficulty)
			: "First to 5",
		selectedMode == CPU_MODE
	);
	if (selectedMode == CPU_MODE)
	{
		gfx_PrintStringXY("<", 72, 114);
		gfx_PrintStringXY(">", 144, 114);
	}
	renderModeOption(
		140,
		"HIGH SCORE",
		"Keep the rally alive",
		selectedMode == HIGH_SCORE_MODE
	);

	gfx_PrintStringXY("UP/DOWN: MODE", 104, 194);
	if (selectedMode == CPU_MODE)
	{
		gfx_PrintStringXY("LEFT/RIGHT: CPU LEVEL", 76, 210);
	}
	else
	{
		gfx_PrintStringXY("BEST SCORE IS SAVED", 84, 210);
	}
	gfx_PrintStringXY("2nd: SELECT  Clear: QUIT", 56, 226);
}

static void renderModeOption(
	int y,
	const char* title,
	const char* description,
	bool selected
)
{
	if (selected)
	{
		gfx_SetColor(MENU_HIGHLIGHT_COLOR);
		gfx_FillRectangle(60, y, 200, 40);
	}

	gfx_SetColor(PADDLE_COLOR);
	gfx_Rectangle(60, y, 200, 40);
	gfx_PrintStringXY(selected ? ">" : " ", 72, y + 6);
	gfx_PrintStringXY(title, 88, y + 6);
	gfx_PrintStringXY(description, 88, y + 22);
}

static void renderGame(void)
{
	gfx_SetColor(PADDLE_COLOR);
	gfx_HorizLine(0, PLAYFIELD_TOP - 1, SCREEN_WIDTH);
	gfx_HorizLine(0, PLAYFIELD_BOTTOM, SCREEN_WIDTH);
	gfx_PrintStringXY("UP/DOWN: MOVE", 8, 224);
	gfx_PrintStringXY("Clear: MENU", 216, 224);

	if (activeMode == CPU_MODE)
	{
		for (int y = PLAYFIELD_TOP; y < PLAYFIELD_BOTTOM; y += 16)
		{
			gfx_FillRectangle(SCREEN_WIDTH / 2 - 1, y, 2, 8);
		}
		drawPaddle(&cpuPaddle);

		gfx_PrintStringXY("CPU MATCH", 8, 8);
		gfx_PrintStringXY(cpuDifficultyName(activeDifficulty), 88, 8);
		gfx_PrintStringXY("YOU", 168, 8);
		gfx_SetTextXY(200, 8);
		gfx_PrintUInt(match.player_score, 1);
		gfx_PrintStringXY(":", 216, 8);
		gfx_SetTextXY(232, 8);
		gfx_PrintUInt(match.cpu_score, 1);
		gfx_PrintStringXY("CPU", 248, 8);
	}
	else
	{
		gfx_PrintStringXY("HIGH SCORE", 8, 8);
		gfx_PrintStringXY("SCORE", 160, 8);
		gfx_SetTextXY(204, 8);
		gfx_PrintUInt(rally.score, 1);
		gfx_PrintStringXY("BEST", 232, 8);
		gfx_SetTextXY(268, 8);
		gfx_PrintUInt(rally.high_score, 1);
	}

	drawPaddle(&playerPaddle);
	drawBall(&ball);

	if (activeMode == CPU_MODE && match.over)
	{
		renderMatchOverOverlay();
	}
	else if (!ball.in_play)
	{
		renderServeOverlay();
	}

}

static void renderServeOverlay(void)
{
	const char *servePrompt = activeMode == CPU_MODE
		&& match.next_serve_direction == BALL_LEFT
		? "SERVE: TO YOU <"
		: activeMode == CPU_MODE
			? "SERVE: TO CPU >"
			: "SERVE: TO WALL >";

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(87, 88, 146, 64);
	gfx_SetColor(PADDLE_COLOR);
	gfx_Rectangle(87, 88, 146, 64);
	gfx_PrintStringXY(servePrompt, 96, 106);
	gfx_PrintStringXY("Press 2nd", 124, 126);
}

static void renderMatchOverOverlay(void)
{
	const char *winner = match.player_score > match.cpu_score
		? "YOU WIN"
		: "CPU WINS";

	gfx_SetColor(BG_COLOR);
	gfx_FillRectangle(87, 88, 146, 64);
	gfx_SetColor(PADDLE_COLOR);
	gfx_Rectangle(87, 88, 146, 64);
	gfx_PrintStringXY(winner, 128, 104);
	gfx_PrintStringXY("2nd: NEW MATCH", 100, 126);
}

static void persistHighScoreIfNeeded(void)
{
	if (rally.high_score > savedHighScore
		&& saveHighScore(rally.high_score)) {
		savedHighScore = rally.high_score;
	}
}

static void beforeGarbageCollection(void)
{
	gfx_End();
}

static void afterGarbageCollection(void)
{
	gfx_Begin();
	gfx_SetDrawBuffer();
}
