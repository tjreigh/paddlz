#include <assert.h>
#include <stdbool.h>

#include "ball.h"
#include "collision.h"
#include "draw.h"
#include "paddle.h"

static ball_t make_ball(int x, int y, int vx, int vy)
{
    ball_t ball = {
        .x = x * (1 << FIXED_SHIFT),
        .y = y * (1 << FIXED_SHIFT),
        .vx = vx,
        .vy = vy,
        .in_play = true,
    };
    return ball;
}

static void test_ball_reset_and_serve(void)
{
    ball_t ball;
    initBall(&ball);

    assert(!ball.in_play);
    assert(ball.x == (SCREEN_WIDTH / 2) << FIXED_SHIFT);
    assert(ball.y == (SCREEN_HEIGHT / 2) << FIXED_SHIFT);
    assert(ball.vx == BALL_INIT_SPEED);
    assert(ball.vy == BALL_INIT_SPEED);

    int initial_x = ball.x;
    int initial_y = ball.y;
    assert(!updateBall(&ball));
    assert(ball.x == initial_x);
    assert(ball.y == initial_y);

    serveBall(&ball);
    assert(ball.in_play);

    resetBall(&ball);
    assert(!ball.in_play);
}

static void test_ball_boundaries(void)
{
    ball_t ball;
    initBall(&ball);
    serveBall(&ball);

    ball.y = (BALL_RADIUS - 1) << FIXED_SHIFT;
    ball.vy = -BALL_INIT_SPEED;
    assert(!updateBall(&ball));
    assert(ball.y == BALL_RADIUS << FIXED_SHIFT);
    assert(ball.vy == BALL_INIT_SPEED);

    ball.y = (SCREEN_HEIGHT - BALL_RADIUS + 1) << FIXED_SHIFT;
    ball.vy = BALL_INIT_SPEED;
    assert(!updateBall(&ball));
    assert(ball.y == (SCREEN_HEIGHT - BALL_RADIUS) << FIXED_SHIFT);
    assert(ball.vy == -BALL_INIT_SPEED);

    ball.x = (SCREEN_WIDTH - BALL_RADIUS + 1) << FIXED_SHIFT;
    ball.vx = BALL_INIT_SPEED;
    assert(!updateBall(&ball));
    assert(ball.x == (SCREEN_WIDTH - BALL_RADIUS) << FIXED_SHIFT);
    assert(ball.vx == -BALL_INIT_SPEED);

    ball.x = (-BALL_RADIUS - 2) * (1 << FIXED_SHIFT);
    ball.vx = -BALL_INIT_SPEED;
    assert(updateBall(&ball));
}

static void test_paddle_movement(void)
{
    paddle_t paddle;
    initPaddle(&paddle);

    assert(paddle.pos.x == INIT_X_LOC);
    assert(paddle.pos.y == INIT_Y_LOC);
    assert(!paddle.should_move);

    paddle.pos.y = 0;
    paddle.move_dir = UP;
    movePaddle(&paddle);
    assert(paddle.pos.y == 0);

    paddle.pos.y = SCREEN_HEIGHT - PADDLE_LEN;
    paddle.move_dir = DOWN;
    movePaddle(&paddle);
    assert(paddle.pos.y == SCREEN_HEIGHT - PADDLE_LEN);

    paddle.pos.y = INIT_Y_LOC;
    paddle.move_dir = DOWN;
    paddle.should_move = true;
    updatePaddle(&paddle);
    assert(paddle.pos.y == INIT_Y_LOC + PADDLE_SPEED);
    assert(!paddle.should_move);
}

static void test_paddle_collision(void)
{
    paddle_t paddle = {
        .pos = { .x = INIT_X_LOC, .y = 60 },
    };
    int paddle_right = paddle.pos.x + PADDLE_WIDTH;
    int paddle_center = paddle.pos.y + PADDLE_LEN / 2;

    ball_t center_hit = make_ball(
        paddle_right + BALL_RADIUS,
        paddle_center,
        -BALL_INIT_SPEED,
        BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&center_hit, &paddle));
    assert(center_hit.vx == BALL_INIT_SPEED);
    assert(center_hit.vy == 0);
    assert(center_hit.x == (paddle_right + BALL_RADIUS) << FIXED_SHIFT);

    ball_t top_hit = make_ball(
        paddle_right + BALL_RADIUS,
        paddle.pos.y,
        -BALL_INIT_SPEED,
        0
    );
    assert(checkPaddleCollision(&top_hit, &paddle));
    assert(top_hit.vy == -BALL_MAX_VERTICAL_SPEED);

    ball_t bottom_hit = make_ball(
        paddle_right + BALL_RADIUS,
        paddle.pos.y + PADDLE_LEN,
        -BALL_INIT_SPEED,
        0
    );
    assert(checkPaddleCollision(&bottom_hit, &paddle));
    assert(bottom_hit.vy == BALL_MAX_VERTICAL_SPEED);

    ball_t moving_away = make_ball(
        paddle_right + BALL_RADIUS,
        paddle_center,
        BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&moving_away, &paddle));

    ball_t vertical_miss = make_ball(
        paddle_right + BALL_RADIUS,
        paddle.pos.y + PADDLE_LEN + BALL_RADIUS + 1,
        -BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&vertical_miss, &paddle));

    ball_t behind_paddle = make_ball(
        paddle_right - 1,
        paddle_center,
        -BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&behind_paddle, &paddle));
}

int main(void)
{
    test_ball_reset_and_serve();
    test_ball_boundaries();
    test_paddle_movement();
    test_paddle_collision();
    return 0;
}
