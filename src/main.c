#include <stdbool.h>
#include <graphx.h>
#include <tice.h>
#include <keypadc.h>

#include "paddle.h"

point poi;
dimen size;
bool gameActive = false;
kb_key_t key;

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

	do {
		kb_Scan();

		key = kb_Data[7];

		switch (key) {
			case kb_Down:
				movePaddle(DOWN);
				break;

			case kb_Right:
				movePaddle(RIGHT);
				break;

			case kb_Up:
				movePaddle(UP);
				break;

			case kb_Left:
				movePaddle(LEFT);
				break;

			default:
				break;
		}

	} while(kb_Data[1] != kb_2nd);

	gfx_End();

	return 0;
}