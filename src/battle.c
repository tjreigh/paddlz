#include "battle.h"

#include "collision.h"
#include "draw.h"

static bool ballReachedWallBoundary(const ball_t* ball, paddle_side_t side)
{
    int x = ball->x >> FIXED_SHIFT;
    int y = ball->y >> FIXED_SHIFT;

    switch (side) {
        case PADDLE_LEFT:
            return ball->vx < 0 && x - BALL_RADIUS <= BATTLE_LEFT;
        case PADDLE_RIGHT:
            return ball->vx > 0 && x + BALL_RADIUS >= BATTLE_RIGHT;
        case PADDLE_TOP:
            return ball->vy < 0 && y - BALL_RADIUS <= BATTLE_TOP;
        case PADDLE_BOTTOM:
        default:
            return ball->vy > 0 && y + BALL_RADIUS >= BATTLE_BOTTOM;
    }
}

static void recordFlipperHit(combatant_t* combatant, const ball_t* ball)
{
    combatant->flip_tangent = paddleIsVertical(combatant->side)
        ? (ball->y >> FIXED_SHIFT)
        : (ball->x >> FIXED_SHIFT);
    combatant->flip_timer = FLIPPER_FLASH_FRAMES;
}

static void reflectAtWall(ball_t* ball, paddle_side_t side)
{
    switch (side) {
        case PADDLE_LEFT:
            ball->vx = -ball->vx;
            ball->x = (BATTLE_LEFT + BALL_RADIUS) << FIXED_SHIFT;
            break;
        case PADDLE_RIGHT:
            ball->vx = -ball->vx;
            ball->x = (BATTLE_RIGHT - BALL_RADIUS) << FIXED_SHIFT;
            break;
        case PADDLE_TOP:
            ball->vy = -ball->vy;
            ball->y = (BATTLE_TOP + BALL_RADIUS) << FIXED_SHIFT;
            break;
        case PADDLE_BOTTOM:
        default:
            ball->vy = -ball->vy;
            ball->y = (BATTLE_BOTTOM - BALL_RADIUS) << FIXED_SHIFT;
            break;
    }
}

void initBattle(battle_t* battle, int combatant_count, cpu_difficulty_t difficulty)
{
    if (combatant_count < BATTLE_MIN_COMBATANTS) {
        combatant_count = BATTLE_MIN_COMBATANTS;
    } else if (combatant_count > BATTLE_MAX_COMBATANTS) {
        combatant_count = BATTLE_MAX_COMBATANTS;
    }

    battle->combatant_count = combatant_count;
    battle->cpu_difficulty = difficulty;
    battle->over = false;

    for (int i = 0; i < BATTLE_MAX_COMBATANTS; i++) {
        paddle_side_t side = (paddle_side_t)i;
        combatant_t* combatant = &battle->combatants[i];

        initBattlePaddle(&combatant->paddle, side);
        combatant->side = side;
        combatant->flip_timer = 0;
        combatant->flip_tangent = 0;

        if (i == 0) {
            combatant->controller = COMBATANT_PLAYER;
            combatant->alive = true;
        } else if (i < combatant_count) {
            combatant->controller = COMBATANT_CPU;
            combatant->alive = true;
        } else {
            combatant->controller = COMBATANT_INACTIVE;
            combatant->alive = false;
        }
    }
}

void updateBattle(battle_t* battle, ball_t* ball)
{
    if (battle->over || !ball->in_play) {
        return;
    }

    advanceBall(ball);

    for (int i = 0; i < BATTLE_MAX_COMBATANTS; i++) {
        if (battle->combatants[i].flip_timer > 0) {
            battle->combatants[i].flip_timer--;
        }
    }

    for (int i = 0; i < BATTLE_MAX_COMBATANTS; i++) {
        combatant_t* combatant = &battle->combatants[i];

        if (combatant->alive) {
            if (combatant->controller == COMBATANT_CPU) {
                updateCpuPaddle(&combatant->paddle, ball, battle->cpu_difficulty);
            }

            if (!checkPaddleCollision(ball, &combatant->paddle)
                && ballReachedWallBoundary(ball, combatant->side)) {
                combatant->alive = false;
                recordFlipperHit(combatant, ball);
                reflectAtWall(ball, combatant->side);
                break;
            }
        } else if (ballReachedWallBoundary(ball, combatant->side)) {
            recordFlipperHit(combatant, ball);
            reflectAtWall(ball, combatant->side);
            break;
        }
    }

    int alive_count = 0;
    for (int i = 0; i < BATTLE_MAX_COMBATANTS; i++) {
        if (battle->combatants[i].alive) {
            alive_count++;
        }
    }
    battle->over = alive_count <= 1;
}

paddle_side_t battleWinner(const battle_t* battle)
{
    for (int i = 0; i < BATTLE_MAX_COMBATANTS; i++) {
        if (battle->combatants[i].alive) {
            return (paddle_side_t)i;
        }
    }

    return PADDLE_LEFT;
}

flipper_wedge_t battleFlipperWedge(paddle_side_t side, int tangent)
{
    bool vertical = paddleIsVertical(side);
    int half = FLIPPER_LEN / 2;
    int min_tangent = (vertical ? PLAYFIELD_TOP : BATTLE_LEFT) + half;
    int max_tangent = (vertical ? PLAYFIELD_BOTTOM : BATTLE_RIGHT) - half;

    if (tangent < min_tangent) {
        tangent = min_tangent;
    } else if (tangent > max_tangent) {
        tangent = max_tangent;
    }

    /* The pivot is fixed at the wall's own midpoint, off-board - the tip
     * swings out to wherever the ball actually landed, so a hit away from
     * the wall's center reads as an angled flipper, not a straight poke. */
    int pivot_tangent = vertical
        ? (PLAYFIELD_TOP + PLAYFIELD_BOTTOM) / 2
        : (BATTLE_LEFT + BATTLE_RIGHT) / 2;

    flipper_wedge_t wedge;

    switch (side) {
        case PADDLE_LEFT:
            wedge.pivot_x = BATTLE_LEFT - FLIPPER_PIVOT_SETBACK;
            wedge.pivot_y = pivot_tangent;
            wedge.tip_a_x = BATTLE_LEFT + FLIPPER_REACH;
            wedge.tip_a_y = tangent - half;
            wedge.tip_b_x = BATTLE_LEFT + FLIPPER_REACH;
            wedge.tip_b_y = tangent + half;
            break;
        case PADDLE_RIGHT:
            wedge.pivot_x = BATTLE_RIGHT + FLIPPER_PIVOT_SETBACK;
            wedge.pivot_y = pivot_tangent;
            wedge.tip_a_x = BATTLE_RIGHT - FLIPPER_REACH;
            wedge.tip_a_y = tangent - half;
            wedge.tip_b_x = BATTLE_RIGHT - FLIPPER_REACH;
            wedge.tip_b_y = tangent + half;
            break;
        case PADDLE_TOP:
            wedge.pivot_x = pivot_tangent;
            wedge.pivot_y = BATTLE_TOP - FLIPPER_PIVOT_SETBACK;
            wedge.tip_a_x = tangent - half;
            wedge.tip_a_y = BATTLE_TOP + FLIPPER_REACH;
            wedge.tip_b_x = tangent + half;
            wedge.tip_b_y = BATTLE_TOP + FLIPPER_REACH;
            break;
        case PADDLE_BOTTOM:
        default:
            wedge.pivot_x = pivot_tangent;
            wedge.pivot_y = BATTLE_BOTTOM + FLIPPER_PIVOT_SETBACK;
            wedge.tip_a_x = tangent - half;
            wedge.tip_a_y = BATTLE_BOTTOM - FLIPPER_REACH;
            wedge.tip_b_x = tangent + half;
            wedge.tip_b_y = BATTLE_BOTTOM - FLIPPER_REACH;
            break;
    }

    return wedge;
}
