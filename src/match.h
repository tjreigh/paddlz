#ifndef MATCH_H
#define MATCH_H

#include <stdbool.h>

#include "ball.h"

#define WINNING_SCORE 5

typedef struct match_t
{
    unsigned int player_score;
    unsigned int cpu_score;
    ball_direction_t next_serve_direction;
    bool over;
} match_t;

void initMatch(match_t* match);
void recordPoint(match_t* match, ball_update_t result);

#endif
