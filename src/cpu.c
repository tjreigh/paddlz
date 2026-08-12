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

void updateCpuPaddle(
    paddle_t* paddle,
    const ball_t* ball,
    cpu_difficulty_t difficulty
)
{
    int target_y = (PLAYFIELD_TOP + PLAYFIELD_BOTTOM) / 2;
    int speed = CPU_NORMAL_SPEED;
    int ball_x = ball->x >> FIXED_SHIFT;

    if (ball->in_play && ball->vx > 0) {
        if (difficulty == CPU_EASY) {
            speed = CPU_EASY_SPEED;
            if (ball_x > SCREEN_WIDTH / 2) {
                target_y = ball->y >> FIXED_SHIFT;
            }
        } else if (difficulty == CPU_HARD) {
            speed = CPU_HARD_SPEED;
            target_y = predictBallYAtX(
                ball,
                paddle->pos.x - BALL_RADIUS
            );
        } else {
            target_y = ball->y >> FIXED_SHIFT;
        }
    }

    movePaddleToward(paddle, target_y, speed);
}
