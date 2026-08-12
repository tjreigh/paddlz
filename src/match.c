#include "match.h"

void initMatch(match_t* match)
{
    match->player_score = 0;
    match->cpu_score = 0;
    match->next_serve_direction = BALL_RIGHT;
    match->over = false;
}

void recordPoint(match_t* match, ball_update_t result)
{
    if (result == BALL_OUT_LEFT) {
        match->cpu_score++;
        match->next_serve_direction = BALL_LEFT;
    } else if (result == BALL_OUT_RIGHT) {
        match->player_score++;
        match->next_serve_direction = BALL_RIGHT;
    }

    match->over = match->player_score >= WINNING_SCORE
        || match->cpu_score >= WINNING_SCORE;
}
