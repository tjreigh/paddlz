#include <stdio.h>
#include <stdbool.h>
#include <graphx.h>
#include <tice.h>

#include "paddle.h"

point poi;
dimen size;
bool gameActive = false;

int main(void) {
	os_ClrHome();

	gfx_Begin();

	gfx_SetColor(227);

	poi.x = 2;
	poi.y = 20;

	size.width = 8;
	size.len = 30;

	initPaddle(size);
	drawPaddle(poi);

	while (!os_GetCSC());

	while (gameActive) {

	}

	gfx_End();

	return 0;
}