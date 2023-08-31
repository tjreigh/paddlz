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

typedef struct paddle_t
{
	point_t pos;
	dir_t move_dir;
	bool should_move;
	bool is_colliding;
	bool hit_wall;
}
paddle_t;

paddle_t* initPaddle();
void movePaddle(paddle_t*);
void updatePaddle(paddle_t*);