#ifndef BATTLE_H
#define BATTLE_H

#include <stdbool.h>

#include "ball.h"
#include "cpu.h"
#include "paddle.h"

#define BATTLE_MIN_COMBATANTS 2
#define BATTLE_MAX_COMBATANTS 4
#define FLIPPER_FLASH_FRAMES 8
#define FLIPPER_HINGE_INSET 6
#define FLIPPER_MAX_SWING_ANGLE 26
#define FLIPPER_MAX_SWING_OFFSET 70
/* The blade's contact point is about 10 px from the hinge. At the maximum
 * 26/256-turn swing, that puts it 6 px away along the wall's tangent. */
#define FLIPPER_MAX_TIP_TANGENT_OFFSET 6

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

void initBattle(battle_t* battle, int combatant_count, cpu_difficulty_t difficulty);
void updateBattle(battle_t* battle, ball_t* ball);
paddle_side_t battleWinner(const battle_t* battle);

/* A flipper is positioned so its outer contact point, rather than its hinge,
 * lines up with the recorded ball impact. The hinge stays fixed for the
 * animation while the angle eases back to rest via flip_timer. */
point_t battleFlipperHinge(paddle_side_t side, int tangent);
int battleFlipperAngle(paddle_side_t side, int tangent, int flip_timer);

#endif
