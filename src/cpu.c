#include "cpu.h"
#include "draw.h"

#define CPU_EASY_SPEED 1
#define CPU_NORMAL_SPEED 2
#define CPU_HARD_SPEED 2

const char* cpuDifficultyName(cpu_difficulty_t difficulty)
{
    switch (difficulty) {
        case CPU_EASY:
            return "EASY";
        case CPU_HARD:
            return "HARD";
        case CPU_NORMAL:
        default:
            return "NORMAL";
    }
}

int predictBallYAtX(const ball_t* ball, int target_x)
{
    int current_y = ball->y >> FIXED_SHIFT;
    if (ball->vx <= 0) {
        return current_y;
    }

    int distance = (target_x << FIXED_SHIFT) - ball->x;
    if (distance <= 0) {
        return current_y;
    }

    int steps = (distance + ball->vx - 1) / ball->vx;
    int predicted_y = ball->y + steps * ball->vy;
    int min_y = (PLAYFIELD_TOP + BALL_RADIUS) << FIXED_SHIFT;
    int max_y = (PLAYFIELD_BOTTOM - BALL_RADIUS) << FIXED_SHIFT;
    int span = max_y - min_y;
    int period = span * 2;
    int offset = (predicted_y - min_y) % period;

    if (offset < 0) {
        offset += period;
    }
    if (offset > span) {
        offset = period - offset;
    }

    return (min_y + offset) >> FIXED_SHIFT;
}

static bool ballApproaching(paddle_side_t side, const ball_t* ball)
{
    switch (side) {
        case PADDLE_LEFT:
            return ball->vx < 0;
        case PADDLE_RIGHT:
            return ball->vx > 0;
        case PADDLE_TOP:
            return ball->vy < 0;
        case PADDLE_BOTTOM:
        default:
            return ball->vy > 0;
    }
}

static bool ballOnNearSide(paddle_side_t side, const ball_t* ball)
{
    int ball_x = ball->x >> FIXED_SHIFT;
    int ball_y = ball->y >> FIXED_SHIFT;

    switch (side) {
        case PADDLE_LEFT:
            return ball_x < SCREEN_WIDTH / 2;
        case PADDLE_RIGHT:
            return ball_x > SCREEN_WIDTH / 2;
        case PADDLE_TOP:
            return ball_y < SCREEN_HEIGHT / 2;
        case PADDLE_BOTTOM:
        default:
            return ball_y > SCREEN_HEIGHT / 2;
    }
}

void updateCpuPaddle(
    paddle_t* paddle,
    const ball_t* ball,
    cpu_difficulty_t difficulty
)
{
    bool vertical = paddleIsVertical(paddle->side);
    int target = vertical
        ? (PLAYFIELD_TOP + PLAYFIELD_BOTTOM) / 2
        : (BATTLE_LEFT + BATTLE_RIGHT) / 2;
    int speed = CPU_NORMAL_SPEED;
    int ball_tangent = vertical ? (ball->y >> FIXED_SHIFT) : (ball->x >> FIXED_SHIFT);

    if (ball->in_play && ballApproaching(paddle->side, ball)) {
        if (difficulty == CPU_EASY) {
            speed = CPU_EASY_SPEED;
            if (ballOnNearSide(paddle->side, ball)) {
                target = ball_tangent;
            }
        } else if (difficulty == CPU_HARD && vertical) {
            speed = CPU_HARD_SPEED;
            int target_x = paddle->side == PADDLE_RIGHT
                ? paddle->pos.x - BALL_RADIUS
                : paddle->pos.x + PADDLE_WIDTH + BALL_RADIUS;
            target = predictBallYAtX(ball, target_x);
        } else {
            if (difficulty == CPU_HARD) {
                speed = CPU_HARD_SPEED;
            }
            target = ball_tangent;
        }
    }

    movePaddleToward(paddle, target, speed);
}
