#include <assert.h>

#include "ball.h"
#include "battle.h"
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

static void test_paddle_orientation(void)
{
    assert(paddleIsVertical(PADDLE_LEFT));
    assert(paddleIsVertical(PADDLE_RIGHT));
    assert(!paddleIsVertical(PADDLE_TOP));
    assert(!paddleIsVertical(PADDLE_BOTTOM));
}

static void test_battle_paddle_initialization(void)
{
    paddle_t paddle;

    initBattlePaddle(&paddle, PADDLE_LEFT);
    assert(paddle.pos.x == BATTLE_LEFT + PADDLE_MARGIN);
    assert(paddle.pos.y == PADDLE_INIT_Y);
    assert(paddle.side == PADDLE_LEFT);

    initBattlePaddle(&paddle, PADDLE_RIGHT);
    assert(paddle.pos.x == BATTLE_RIGHT - PADDLE_MARGIN - PADDLE_WIDTH);
    assert(paddle.pos.y == PADDLE_INIT_Y);
    assert(paddle.side == PADDLE_RIGHT);

    initBattlePaddle(&paddle, PADDLE_TOP);
    assert(paddle.pos.x == (BATTLE_LEFT + BATTLE_RIGHT) / 2 - PADDLE_LEN / 2);
    assert(paddle.pos.y == BATTLE_TOP + PADDLE_MARGIN);
    assert(paddle.side == PADDLE_TOP);

    initBattlePaddle(&paddle, PADDLE_BOTTOM);
    assert(paddle.pos.x == (BATTLE_LEFT + BATTLE_RIGHT) / 2 - PADDLE_LEN / 2);
    assert(paddle.pos.y == BATTLE_BOTTOM - PADDLE_MARGIN - PADDLE_WIDTH);
    assert(paddle.side == PADDLE_BOTTOM);
}

static void test_battle_paddle_horizontal_movement(void)
{
    paddle_t top;
    initBattlePaddle(&top, PADDLE_TOP);
    int start_x = top.pos.x;

    top.pos.x = BATTLE_LEFT;
    movePaddle(&top, UP, PLAYER_PADDLE_SPEED);
    assert(top.pos.x == BATTLE_LEFT);

    top.pos.x = BATTLE_RIGHT - PADDLE_LEN;
    movePaddle(&top, DOWN, PLAYER_PADDLE_SPEED);
    assert(top.pos.x == BATTLE_RIGHT - PADDLE_LEN);

    top.pos.x = start_x;
    movePaddle(&top, DOWN, PLAYER_PADDLE_SPEED);
    assert(top.pos.x == start_x + PLAYER_PADDLE_SPEED);

    top.pos.x = start_x;
    movePaddleToward(&top, 0, 1);
    assert(top.pos.x == start_x - 1);
    movePaddleToward(&top, SCREEN_WIDTH, 1);
    assert(top.pos.x == start_x);
}

static void test_compute_deflection(void)
{
    int max_offset = PADDLE_LEN / 2;

    assert(computeDeflection(0, max_offset) == 0);
    assert(computeDeflection(max_offset, max_offset) == BALL_MAX_DEFLECT_SPEED);
    assert(computeDeflection(-max_offset, max_offset) == -BALL_MAX_DEFLECT_SPEED);
    assert(computeDeflection(max_offset + 10, max_offset) == BALL_MAX_DEFLECT_SPEED);
    assert(computeDeflection(-max_offset - 10, max_offset) == -BALL_MAX_DEFLECT_SPEED);
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

static void test_cpu_horizontal_tracking(void)
{
    paddle_t paddle;
    initBattlePaddle(&paddle, PADDLE_TOP);
    int paddle_center_x = (BATTLE_LEFT + BATTLE_RIGHT) / 2 - PADDLE_LEN / 2;

    ball_t incoming = make_ball(100, 180, 0, -BALL_INIT_SPEED);

    updateCpuPaddle(&paddle, &incoming, CPU_EASY);
    assert(paddle.pos.x == paddle_center_x);

    updateCpuPaddle(&paddle, &incoming, CPU_NORMAL);
    assert(paddle.pos.x == paddle_center_x - 2);

    initBattlePaddle(&paddle, PADDLE_TOP);
    incoming.y = 100 << FIXED_SHIFT;
    updateCpuPaddle(&paddle, &incoming, CPU_EASY);
    assert(paddle.pos.x == paddle_center_x - 1);

    initBattlePaddle(&paddle, PADDLE_TOP);
    updateCpuPaddle(&paddle, &incoming, CPU_HARD);
    assert(paddle.pos.x == paddle_center_x - 2);
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
    assert(top_hit.vy == -BALL_MAX_DEFLECT_SPEED);

    ball_t bottom_hit = make_ball(
        paddle_front + BALL_RADIUS,
        paddle.pos.y + PADDLE_LEN,
        -BALL_INIT_SPEED,
        0
    );
    assert(checkPaddleCollision(&bottom_hit, &paddle));
    assert(bottom_hit.vy == BALL_MAX_DEFLECT_SPEED);

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

static void test_top_paddle_collision(void)
{
    paddle_t paddle;
    initBattlePaddle(&paddle, PADDLE_TOP);
    int paddle_front = paddle.pos.y + PADDLE_WIDTH;
    int paddle_center = paddle.pos.x + PADDLE_LEN / 2;

    ball_t center_hit = make_ball(
        paddle_center,
        paddle_front + BALL_RADIUS,
        0,
        -BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&center_hit, &paddle));
    assert(center_hit.vy == BALL_INIT_SPEED);
    assert(center_hit.vx == 0);
    assert(center_hit.y == (paddle_front + BALL_RADIUS) << FIXED_SHIFT);

    ball_t left_hit = make_ball(
        paddle.pos.x,
        paddle_front + BALL_RADIUS,
        0,
        -BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&left_hit, &paddle));
    assert(left_hit.vx == -BALL_MAX_DEFLECT_SPEED);

    ball_t right_hit = make_ball(
        paddle.pos.x + PADDLE_LEN,
        paddle_front + BALL_RADIUS,
        0,
        -BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&right_hit, &paddle));
    assert(right_hit.vx == BALL_MAX_DEFLECT_SPEED);

    ball_t moving_away = make_ball(
        paddle_center,
        paddle_front + BALL_RADIUS,
        0,
        BALL_INIT_SPEED
    );
    assert(!checkPaddleCollision(&moving_away, &paddle));

    ball_t behind_paddle = make_ball(
        paddle_center,
        paddle_front - 1,
        0,
        -BALL_INIT_SPEED
    );
    assert(!checkPaddleCollision(&behind_paddle, &paddle));
}

static void test_bottom_paddle_collision(void)
{
    paddle_t paddle;
    initBattlePaddle(&paddle, PADDLE_BOTTOM);
    int paddle_front = paddle.pos.y;
    int paddle_center = paddle.pos.x + PADDLE_LEN / 2;

    ball_t center_hit = make_ball(
        paddle_center,
        paddle_front - BALL_RADIUS,
        0,
        BALL_INIT_SPEED
    );
    assert(checkPaddleCollision(&center_hit, &paddle));
    assert(center_hit.vy == -BALL_INIT_SPEED);
    assert(center_hit.vx == 0);
    assert(center_hit.y == (paddle_front - BALL_RADIUS) << FIXED_SHIFT);

    ball_t moving_away = make_ball(
        paddle_center,
        paddle_front - BALL_RADIUS,
        0,
        -BALL_INIT_SPEED
    );
    assert(!checkPaddleCollision(&moving_away, &paddle));

    ball_t horizontal_miss = make_ball(
        paddle.pos.x + PADDLE_LEN + BALL_RADIUS + 1,
        paddle_front - BALL_RADIUS,
        0,
        BALL_INIT_SPEED
    );
    assert(!checkPaddleCollision(&horizontal_miss, &paddle));
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

static void test_battle_init(void)
{
    battle_t battle;

    initBattle(&battle, 2, CPU_NORMAL);
    assert(battle.combatant_count == 2);
    assert(!battle.over);
    assert(battle.combatants[PADDLE_LEFT].controller == COMBATANT_PLAYER);
    assert(battle.combatants[PADDLE_LEFT].alive);
    assert(battle.combatants[PADDLE_RIGHT].controller == COMBATANT_CPU);
    assert(battle.combatants[PADDLE_RIGHT].alive);
    assert(battle.combatants[PADDLE_TOP].controller == COMBATANT_INACTIVE);
    assert(!battle.combatants[PADDLE_TOP].alive);
    assert(battle.combatants[PADDLE_BOTTOM].controller == COMBATANT_INACTIVE);
    assert(!battle.combatants[PADDLE_BOTTOM].alive);

    initBattle(&battle, 3, CPU_HARD);
    assert(battle.combatant_count == 3);
    assert(battle.combatants[PADDLE_TOP].controller == COMBATANT_CPU);
    assert(battle.combatants[PADDLE_TOP].alive);
    assert(battle.combatants[PADDLE_BOTTOM].controller == COMBATANT_INACTIVE);
    assert(!battle.combatants[PADDLE_BOTTOM].alive);

    initBattle(&battle, 1, CPU_EASY);
    assert(battle.combatant_count == BATTLE_MIN_COMBATANTS);

    initBattle(&battle, 5, CPU_EASY);
    assert(battle.combatant_count == BATTLE_MAX_COMBATANTS);
    assert(battle.combatants[PADDLE_BOTTOM].controller == COMBATANT_CPU);
    assert(battle.combatants[PADDLE_BOTTOM].alive);
}

static void test_battle_elimination(void)
{
    battle_t battle;
    initBattle(&battle, 3, CPU_NORMAL);

    ball_t ball = make_ball(246, 200, BALL_INIT_SPEED, 0);

    updateBattle(&battle, &ball);

    assert(!battle.combatants[PADDLE_RIGHT].alive);
    assert(battle.combatants[PADDLE_RIGHT].flip_timer == FLIPPER_FLASH_FRAMES);
    assert(battle.combatants[PADDLE_RIGHT].flip_tangent == 200);
    assert(ball.in_play);
    assert(!battle.over);
    assert(ball.vx == -BALL_INIT_SPEED);
    assert(ball.vy == 0);
    assert(ball.x == (BATTLE_RIGHT - BALL_RADIUS) << FIXED_SHIFT);
}

static void test_battle_dead_wall_guaranteed_bounce(void)
{
    battle_t battle;
    initBattle(&battle, 3, CPU_NORMAL);
    battle.combatants[PADDLE_RIGHT].alive = false;

    ball_t ball = make_ball(246, 120, BALL_INIT_SPEED, 0);

    updateBattle(&battle, &ball);

    assert(ball.in_play);
    assert(ball.vx == -BALL_INIT_SPEED);
    assert(ball.x == (BATTLE_RIGHT - BALL_RADIUS) << FIXED_SHIFT);
}

static void test_battle_dead_wall_flat_bounce(void)
{
    battle_t battle;

    initBattle(&battle, 3, CPU_NORMAL);
    battle.combatants[PADDLE_RIGHT].alive = false;
    ball_t near_top = make_ball(246, 40, BALL_INIT_SPEED, -BALL_INIT_SPEED);
    updateBattle(&battle, &near_top);
    assert(near_top.vx == -BALL_INIT_SPEED);
    assert(near_top.vy == -BALL_INIT_SPEED);
    assert(near_top.x == (BATTLE_RIGHT - BALL_RADIUS) << FIXED_SHIFT);
    assert(!battle.over);

    initBattle(&battle, 3, CPU_NORMAL);
    battle.combatants[PADDLE_RIGHT].alive = false;
    ball_t near_bottom = make_ball(246, 200, BALL_INIT_SPEED, BALL_INIT_SPEED);
    updateBattle(&battle, &near_bottom);
    assert(near_bottom.vx == -BALL_INIT_SPEED);
    assert(near_bottom.vy == BALL_INIT_SPEED);
    assert(near_bottom.x == (BATTLE_RIGHT - BALL_RADIUS) << FIXED_SHIFT);
    assert(!battle.over);
}

static void test_battle_flipper_wedge_geometry(void)
{
    flipper_wedge_t right_wedge = battleFlipperWedge(PADDLE_RIGHT, 180);
    assert(right_wedge.pivot_x > BATTLE_RIGHT);
    assert(right_wedge.pivot_y == (PLAYFIELD_TOP + PLAYFIELD_BOTTOM) / 2);
    assert(right_wedge.tip_a_x == BATTLE_RIGHT - FLIPPER_REACH);
    assert(right_wedge.tip_b_x == BATTLE_RIGHT - FLIPPER_REACH);
    /* the tip sits well inside the arena, not on the boundary line itself -
     * a clip-region xmax is exclusive, so a tip drawn exactly on the wall
     * would have no visible area once clipped */
    assert(right_wedge.tip_a_x < BATTLE_RIGHT);
    assert(right_wedge.tip_a_y == 180 - FLIPPER_LEN / 2);
    assert(right_wedge.tip_b_y == 180 + FLIPPER_LEN / 2);
    /* the tip sits away from the pivot's tangent coordinate - that offset
     * is what makes a hit off-center read as an angled flipper */
    assert(right_wedge.tip_a_y != right_wedge.pivot_y);

    flipper_wedge_t left_wedge = battleFlipperWedge(PADDLE_LEFT, 150);
    assert(left_wedge.pivot_x < BATTLE_LEFT);
    assert(left_wedge.tip_a_x == BATTLE_LEFT + FLIPPER_REACH);
    assert(left_wedge.tip_b_x == BATTLE_LEFT + FLIPPER_REACH);

    flipper_wedge_t top_wedge = battleFlipperWedge(PADDLE_TOP, 200);
    assert(top_wedge.pivot_y < BATTLE_TOP);
    assert(top_wedge.tip_a_y == BATTLE_TOP + FLIPPER_REACH);
    assert(top_wedge.tip_b_y == BATTLE_TOP + FLIPPER_REACH);

    flipper_wedge_t bottom_wedge = battleFlipperWedge(PADDLE_BOTTOM, 200);
    assert(bottom_wedge.pivot_y > BATTLE_BOTTOM);
    assert(bottom_wedge.tip_a_y == BATTLE_BOTTOM - FLIPPER_REACH);

    /* clamped so it never extends past the wall's own span, e.g. near a corner */
    flipper_wedge_t clamped = battleFlipperWedge(PADDLE_RIGHT, PLAYFIELD_TOP);
    assert(clamped.tip_a_y >= PLAYFIELD_TOP);
}

static void test_battle_winner(void)
{
    battle_t battle;
    initBattle(&battle, 4, CPU_NORMAL);

    battle.combatants[PADDLE_RIGHT].alive = false;
    battle.combatants[PADDLE_TOP].alive = false;
    battle.combatants[PADDLE_BOTTOM].alive = false;

    assert(battleWinner(&battle) == PADDLE_LEFT);

    battle.combatants[PADDLE_LEFT].alive = false;
    battle.combatants[PADDLE_TOP].alive = true;

    assert(battleWinner(&battle) == PADDLE_TOP);
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
    test_paddle_orientation();
    test_battle_paddle_initialization();
    test_battle_paddle_horizontal_movement();
    test_compute_deflection();
    test_cpu_difficulties();
    test_cpu_horizontal_tracking();
    test_left_paddle_collision();
    test_right_paddle_collision();
    test_top_paddle_collision();
    test_bottom_paddle_collision();
    test_match_scoring();
    test_rally_scoring();
    test_battle_init();
    test_battle_elimination();
    test_battle_dead_wall_guaranteed_bounce();
    test_battle_dead_wall_flat_bounce();
    test_battle_flipper_wedge_geometry();
    test_battle_winner();
    test_save_format();
    return 0;
}
