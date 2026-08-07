#include <assert.h>

#include "ball.h"
#include "collision.h"
#include "cpu.h"
#include "draw.h"
#include "match.h"
#include "paddle.h"
#include "rally.h"
#include "save.h"

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
    assert(updateBall(&ball, BALL_GOAL_ON_RIGHT) == BALL_IN_PLAY);
    assert(ball.x == initial_x);
    assert(ball.y == initial_y);

    serveBall(&ball, BALL_LEFT);
    assert(ball.in_play);
    assert(ball.vx == -BALL_INIT_SPEED);

    resetBall(&ball);
    serveBall(&ball, BALL_RIGHT);
    assert(ball.in_play);
    assert(ball.vx == BALL_INIT_SPEED);
}

static void test_ball_boundaries_and_goals(void)
{
    ball_t ball;
    initBall(&ball);
    serveBall(&ball, BALL_RIGHT);

    ball.y = (PLAYFIELD_TOP + BALL_RADIUS - 1) << FIXED_SHIFT;
    ball.vy = -BALL_INIT_SPEED;
    assert(updateBall(&ball, BALL_GOAL_ON_RIGHT) == BALL_IN_PLAY);
    assert(ball.y == (PLAYFIELD_TOP + BALL_RADIUS) << FIXED_SHIFT);
    assert(ball.vy == BALL_INIT_SPEED);

    ball.y = (PLAYFIELD_BOTTOM - BALL_RADIUS + 1) << FIXED_SHIFT;
    ball.vy = BALL_INIT_SPEED;
    assert(updateBall(&ball, BALL_GOAL_ON_RIGHT) == BALL_IN_PLAY);
    assert(ball.y == (PLAYFIELD_BOTTOM - BALL_RADIUS) << FIXED_SHIFT);
    assert(ball.vy == -BALL_INIT_SPEED);

    ball.x = (-BALL_RADIUS - 2) * (1 << FIXED_SHIFT);
    ball.vx = -BALL_INIT_SPEED;
    assert(updateBall(&ball, BALL_GOAL_ON_RIGHT) == BALL_OUT_LEFT);

    ball.x = (SCREEN_WIDTH + BALL_RADIUS + 2) << FIXED_SHIFT;
    ball.vx = BALL_INIT_SPEED;
    assert(updateBall(&ball, BALL_GOAL_ON_RIGHT) == BALL_OUT_RIGHT);

    ball.x = (SCREEN_WIDTH - BALL_RADIUS + 1) << FIXED_SHIFT;
    ball.vx = BALL_INIT_SPEED;
    assert(updateBall(&ball, BALL_WALL_ON_RIGHT) == BALL_IN_PLAY);
    assert(ball.x == (SCREEN_WIDTH - BALL_RADIUS) << FIXED_SHIFT);
    assert(ball.vx == -BALL_INIT_SPEED);
}

static void test_paddle_initialization_and_movement(void)
{
    paddle_t left;
    paddle_t right;
    initPaddle(&left, PADDLE_LEFT);
    initPaddle(&right, PADDLE_RIGHT);

    assert(left.pos.x == PADDLE_MARGIN);
    assert(left.pos.y == PADDLE_INIT_Y);
    assert(left.side == PADDLE_LEFT);
    assert(right.pos.x == SCREEN_WIDTH - PADDLE_MARGIN - PADDLE_WIDTH);
    assert(right.pos.y == PADDLE_INIT_Y);
    assert(right.side == PADDLE_RIGHT);

    left.pos.y = PLAYFIELD_TOP;
    movePaddle(&left, UP, PLAYER_PADDLE_SPEED);
    assert(left.pos.y == PLAYFIELD_TOP);

    left.pos.y = PLAYFIELD_BOTTOM - PADDLE_LEN;
    movePaddle(&left, DOWN, PLAYER_PADDLE_SPEED);
    assert(left.pos.y == PLAYFIELD_BOTTOM - PADDLE_LEN);

    left.pos.y = PADDLE_INIT_Y;
    movePaddle(&left, DOWN, PLAYER_PADDLE_SPEED);
    assert(left.pos.y == PADDLE_INIT_Y + PLAYER_PADDLE_SPEED);

    movePaddleToward(&right, 0, 1);
    assert(right.pos.y == PADDLE_INIT_Y - 1);
    movePaddleToward(&right, SCREEN_HEIGHT, 1);
    assert(right.pos.y == PADDLE_INIT_Y);
}

static void test_cpu_difficulties(void)
{
    paddle_t paddle;
    initPaddle(&paddle, PADDLE_RIGHT);

    assert(cpuDifficultyName(CPU_EASY)[0] == 'E');
    assert(cpuDifficultyName(CPU_NORMAL)[0] == 'N');
    assert(cpuDifficultyName(CPU_HARD)[0] == 'H');

    ball_t incoming = make_ball(
        100,
        180,
        BALL_INIT_SPEED,
        0
    );
    updateCpuPaddle(&paddle, &incoming, CPU_EASY);
    assert(paddle.pos.y == PADDLE_INIT_Y);

    updateCpuPaddle(&paddle, &incoming, CPU_NORMAL);
    assert(paddle.pos.y == PADDLE_INIT_Y + 2);

    initPaddle(&paddle, PADDLE_RIGHT);
    incoming.x = 200 << FIXED_SHIFT;
    updateCpuPaddle(&paddle, &incoming, CPU_EASY);
    assert(paddle.pos.y == PADDLE_INIT_Y + 1);

    ball_t bouncing = make_ball(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        BALL_INIT_SPEED,
        BALL_INIT_SPEED
    );
    int target_x = paddle.pos.x - BALL_RADIUS;
    assert(predictBallYAtX(&bouncing, target_x) == 154);

    bouncing.vy = -BALL_INIT_SPEED;
    assert(predictBallYAtX(&bouncing, target_x) == 86);

    initPaddle(&paddle, PADDLE_RIGHT);
    bouncing.vy = BALL_INIT_SPEED;
    updateCpuPaddle(&paddle, &bouncing, CPU_HARD);
    assert(paddle.pos.y == PADDLE_INIT_Y + 2);
}

static void test_left_paddle_collision(void)
{
    paddle_t paddle;
    initPaddle(&paddle, PADDLE_LEFT);
    int paddle_front = paddle.pos.x + PADDLE_WIDTH;
    int paddle_center = paddle.pos.y + PADDLE_LEN / 2;

    ball_t center_hit = make_ball(
        paddle_front + BALL_RADIUS,
        paddle_center,
        -BALL_INIT_SPEED,
        BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&center_hit, &paddle));
    assert(center_hit.vx == BALL_INIT_SPEED);
    assert(center_hit.vy == 0);
    assert(center_hit.x == (paddle_front + BALL_RADIUS) << FIXED_SHIFT);

    ball_t top_hit = make_ball(
        paddle_front + BALL_RADIUS,
        paddle.pos.y,
        -BALL_INIT_SPEED,
        0
    );
    assert(checkPaddleCollision(&top_hit, &paddle));
    assert(top_hit.vy == -BALL_MAX_VERTICAL_SPEED);

    ball_t bottom_hit = make_ball(
        paddle_front + BALL_RADIUS,
        paddle.pos.y + PADDLE_LEN,
        -BALL_INIT_SPEED,
        0
    );
    assert(checkPaddleCollision(&bottom_hit, &paddle));
    assert(bottom_hit.vy == BALL_MAX_VERTICAL_SPEED);

    ball_t moving_away = make_ball(
        paddle_front + BALL_RADIUS,
        paddle_center,
        BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&moving_away, &paddle));

    ball_t behind_paddle = make_ball(
        paddle_front - 1,
        paddle_center,
        -BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&behind_paddle, &paddle));
}

static void test_right_paddle_collision(void)
{
    paddle_t paddle;
    initPaddle(&paddle, PADDLE_RIGHT);
    int paddle_front = paddle.pos.x;
    int paddle_center = paddle.pos.y + PADDLE_LEN / 2;

    ball_t center_hit = make_ball(
        paddle_front - BALL_RADIUS,
        paddle_center,
        BALL_INIT_SPEED,
        -BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&center_hit, &paddle));
    assert(center_hit.vx == -BALL_INIT_SPEED);
    assert(center_hit.vy == 0);
    assert(center_hit.x == (paddle_front - BALL_RADIUS) << FIXED_SHIFT);

    ball_t moving_away = make_ball(
        paddle_front - BALL_RADIUS,
        paddle_center,
        -BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&moving_away, &paddle));

    ball_t vertical_miss = make_ball(
        paddle_front - BALL_RADIUS,
        paddle.pos.y + PADDLE_LEN + BALL_RADIUS + 1,
        BALL_INIT_SPEED,
        0
    );
    assert(!checkPaddleCollision(&vertical_miss, &paddle));
}

static void test_match_scoring(void)
{
    match_t match;
    initMatch(&match);

    assert(match.player_score == 0);
    assert(match.cpu_score == 0);
    assert(match.next_serve_direction == BALL_RIGHT);
    assert(!match.over);

    recordPoint(&match, BALL_OUT_LEFT);
    assert(match.cpu_score == 1);
    assert(match.player_score == 0);
    assert(match.next_serve_direction == BALL_LEFT);
    assert(!match.over);

    recordPoint(&match, BALL_OUT_RIGHT);
    assert(match.player_score == 1);
    assert(match.cpu_score == 1);
    assert(match.next_serve_direction == BALL_RIGHT);
    assert(!match.over);

    for (unsigned int point = match.player_score; point < WINNING_SCORE; point++) {
        recordPoint(&match, BALL_OUT_RIGHT);
    }
    assert(match.player_score == WINNING_SCORE);
    assert(match.over);

    initMatch(&match);
    for (unsigned int point = 0; point < WINNING_SCORE; point++) {
        recordPoint(&match, BALL_OUT_LEFT);
    }
    assert(match.cpu_score == WINNING_SCORE);
    assert(match.over);
}

static void test_rally_scoring(void)
{
    rally_t rally;
    initRally(&rally);

    assert(rally.score == 0);
    assert(rally.high_score == 0);

    recordRallyHit(&rally);
    recordRallyHit(&rally);
    assert(rally.score == 2);
    assert(rally.high_score == 2);

    resetRallyScore(&rally);
    assert(rally.score == 0);
    assert(rally.high_score == 2);

    recordRallyHit(&rally);
    assert(rally.score == 1);
    assert(rally.high_score == 2);
}

static void test_save_format(void)
{
    uint8_t data[SAVE_DATA_SIZE];
    unsigned int high_score = 0;

    encodeSaveData(0x123456, data);
    assert(data[0] == 'P');
    assert(data[1] == 'D');
    assert(data[2] == 'L');
    assert(data[3] == 'Z');
    assert(data[4] == SAVE_FORMAT_VERSION);
    assert(data[5] == 0x56);
    assert(data[6] == 0x34);
    assert(data[7] == 0x12);
    assert(decodeSaveData(data, sizeof(data), &high_score));
    assert(high_score == 0x123456);

    data[0] = 'X';
    assert(!decodeSaveData(data, sizeof(data), &high_score));
    data[0] = 'P';

    data[4] = SAVE_FORMAT_VERSION + 1;
    assert(!decodeSaveData(data, sizeof(data), &high_score));
    data[4] = SAVE_FORMAT_VERSION;

    assert(!decodeSaveData(data, sizeof(data) - 1, &high_score));
    assert(!decodeSaveData(data, sizeof(data), NULL));
}

int main(void)
{
    test_ball_reset_and_serve();
    test_ball_boundaries_and_goals();
    test_paddle_initialization_and_movement();
    test_cpu_difficulties();
    test_left_paddle_collision();
    test_right_paddle_collision();
    test_match_scoring();
    test_rally_scoring();
    test_save_format();
    return 0;
}
