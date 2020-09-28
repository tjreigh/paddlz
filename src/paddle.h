typedef struct point {
	int x;
	int y;
	struct point* pt;
} point;

typedef struct dimen {
	int width;
	int len;
} dimen;

typedef enum dir {
	UP, 
	DOWN, 
	LEFT, 
	RIGHT 
} dir;

extern void initPaddle(dimen s);
extern void drawPaddle(point p);
extern void movePaddle(dir d);