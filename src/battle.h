#ifndef BATTLE_H
#define BATTLE_H

#include <stdbool.h>

#include "ball.h"
#include "cpu.h"
#include "paddle.h"

#define BATTLE_MIN_COMBATANTS 2
#define BATTLE_MAX_COMBATANTS 4
#define FLIPPER_FLASH_FRAMES 6
#define FLIPPER_LEN 28
#define FLIPPER_PIVOT_SETBACK 10
#define FLIPPER_REACH 18

typedef enum combatant_controller_t
{
    COMBATANT_PLAYER,
    COMBATANT_CPU,
    COMBATANT_INACTIVE
} combatant_controller_t;

typedef struct combatant_t
{
    paddle_t paddle;
    paddle_side_t side;
    combatant_controller_t controller;
    bool alive;
    int flip_timer;
    int flip_tangent;
} combatant_t;

typedef struct battle_t
{
    combatant_t combatants[BATTLE_MAX_COMBATANTS];
    int combatant_count;
    cpu_difficulty_t cpu_difficulty;
    bool over;
} battle_t;

/* A flipper is drawn as a wedge from a fixed off-board pivot to a wide
 * contact edge at the point where the ball was actually hit - the pivot
 * end is meant to be clipped away by the arena boundary when drawn. */
typedef struct flipper_wedge_t
{
    int pivot_x;
    int pivot_y;
    int tip_a_x;
    int tip_a_y;
    int tip_b_x;
    int tip_b_y;
} flipper_wedge_t;

void initBattle(battle_t* battle, int combatant_count, cpu_difficulty_t difficulty);
void updateBattle(battle_t* battle, ball_t* ball);
paddle_side_t battleWinner(const battle_t* battle);
flipper_wedge_t battleFlipperWedge(paddle_side_t side, int tangent);

#endif
