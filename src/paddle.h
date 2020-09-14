typedef struct point {
	int x;
	int y;
	struct point* pt;
} point;

typedef struct dimen {
	int width;
	int len;
} dimen;

extern void initPaddle(dimen s);
extern void drawPaddle(point p);
extern void move();