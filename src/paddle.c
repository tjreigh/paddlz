#include <stdbool.h>
#include <graphx.h>
#include <keypadc.h>

#include "paddle.h"

point loc;
dimen siz;
point *loc_ptr = &loc;
dimen *size_ptr = &siz;

void initPaddle(dimen s) {
	gfx_FillRectangle(2, 2, s.width, s.len);
	size_ptr->width = s.width;
	size_ptr->len = s.len;
}

void drawPaddle(point p) {
	gfx_FillRectangle(p.x, p.y, siz.width, siz.len);

	loc_ptr->x = p.x;
	loc_ptr->y = p.y;
}

void move() {
	point newLoc;
	if (kb_Data[7] & kb_Up) {
		//loc.y += 2;
		//point newLoc;
		newLoc.x = loc.x;
		newLoc.y = loc.y + 2;
		drawPaddle(newLoc);
	}
	if (kb_Data[7] & kb_Down) {
		//loc.y -= 2;
		//point newLoc;
		newLoc.x = loc.x;
		newLoc.y = loc.y + 2;
		drawPaddle(newLoc);
	}
}