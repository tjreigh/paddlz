#ifndef RALLY_H
#define RALLY_H

typedef struct rally_t
{
    unsigned int score;
    unsigned int high_score;
} rally_t;

void initRally(rally_t* rally);
void resetRallyScore(rally_t* rally);
void recordRallyHit(rally_t* rally);

#endif
