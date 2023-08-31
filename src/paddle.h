#define PADDLE_WIDTH 8
#define PADDLE_LEN 30
#define INIT_X_LOC 2
#define INIT_Y_LOC 30

typedef struct point_t
{
	int x;
	int y;
} point_t;

typedef enum dir_t
{
	UP,
	DOWN
} dir_t;

void initPaddle(point_t *point);
void movePaddle(dir_t, point_t *point);
void updatePaddle();