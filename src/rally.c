#include "rally.h"

void initRally(rally_t* rally)
{
    rally->score = 0;
    rally->high_score = 0;
}

void resetRallyScore(rally_t* rally)
{
    rally->score = 0;
}

void recordRallyHit(rally_t* rally)
{
    rally->score++;
    if (rally->score > rally->high_score) {
        rally->high_score = rally->score;
    }
}
