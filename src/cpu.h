#ifndef CPU_H
#define CPU_H

#include "ball.h"
#include "paddle.h"

typedef enum cpu_difficulty_t
{
    CPU_EASY,
    CPU_NORMAL,
    CPU_HARD,
    CPU_DIFFICULTY_COUNT
} cpu_difficulty_t;

const char* cpuDifficultyName(cpu_difficulty_t difficulty);
int predictBallYAtX(const ball_t* ball, int target_x);
void updateCpuPaddle(
    paddle_t* paddle,
    const ball_t* ball,
    cpu_difficulty_t difficulty
);

#endif
