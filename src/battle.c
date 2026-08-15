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

static int flipperTangentCenter(paddle_side_t side)
{
    return paddleIsVertical(side)
        ? (PLAYFIELD_TOP + PLAYFIELD_BOTTOM) / 2
        : (BATTLE_LEFT + BATTLE_RIGHT) / 2;
}

static int flipperTangentOffset(paddle_side_t side, int tangent)
{
    int offset = tangent - flipperTangentCenter(side);

    if (offset > FLIPPER_MAX_SWING_OFFSET) {
        return FLIPPER_MAX_SWING_OFFSET;
    }
    if (offset < -FLIPPER_MAX_SWING_OFFSET) {
        return -FLIPPER_MAX_SWING_OFFSET;
    }

    return offset;
}

point_t battleFlipperHinge(paddle_side_t side, int tangent)
{
    int offset = flipperTangentOffset(side, tangent);
    int tip_tangent_offset = FLIPPER_MAX_TIP_TANGENT_OFFSET * offset
        / FLIPPER_MAX_SWING_OFFSET;
    int hinge_tangent = tangent - tip_tangent_offset;
    point_t hinge;

    /* At the fully struck angle, the blade tip is tip_tangent_offset pixels
     * farther from wall center than the pivot. Pulling the pivot back by the
     * same amount puts that tip at the recorded impact tangent. */
    switch (side) {
        case PADDLE_LEFT:
            hinge.x = BATTLE_LEFT + FLIPPER_HINGE_INSET;
            hinge.y = hinge_tangent;
            break;
        case PADDLE_RIGHT:
            hinge.x = BATTLE_RIGHT - FLIPPER_HINGE_INSET;
            hinge.y = hinge_tangent;
            break;
        case PADDLE_TOP:
            hinge.x = hinge_tangent;
            hinge.y = BATTLE_TOP + FLIPPER_HINGE_INSET;
            break;
        case PADDLE_BOTTOM:
        default:
            hinge.x = hinge_tangent;
            hinge.y = BATTLE_BOTTOM - FLIPPER_HINGE_INSET;
            break;
    }

    return hinge;
}

/* Rest orientation of the sprite art (blade pointing toward row 0, "up")
 * mapped to a 256-step angle, clockwise from up, per wall - each wall's
 * base angle points its blade away from the wall, into the arena. */
static int flipperRestAngle(paddle_side_t side)
{
    switch (side) {
        case PADDLE_LEFT:
            return 64;
        case PADDLE_RIGHT:
            return 192;
        case PADDLE_TOP:
            return 128;
        case PADDLE_BOTTOM:
        default:
            return 0;
    }
}

/* Sign of the angle delta that swings the tip toward increasing tangent
 * (larger y for vertical walls, larger x for horizontal walls), given
 * each wall's rest angle above. */
static int flipperSwingSign(paddle_side_t side)
{
    switch (side) {
        case PADDLE_LEFT:
        case PADDLE_BOTTOM:
            return 1;
        case PADDLE_RIGHT:
        case PADDLE_TOP:
        default:
            return -1;
    }
}

int battleFlipperAngle(paddle_side_t side, int tangent, int flip_timer)
{
    if (flip_timer > FLIPPER_FLASH_FRAMES) {
        flip_timer = FLIPPER_FLASH_FRAMES;
    } else if (flip_timer < 0) {
        flip_timer = 0;
    }

    int offset = flipperTangentOffset(side, tangent);

    /* flip_timer == FLIPPER_FLASH_FRAMES right when the ball lands - swing
     * starts fully struck (aimed at the real contact point) and eases back
     * to the wall's rest angle as the timer counts down to 0. */
    int swing = flipperSwingSign(side) * FLIPPER_MAX_SWING_ANGLE * offset
        / FLIPPER_MAX_SWING_OFFSET * flip_timer / FLIPPER_FLASH_FRAMES;

    int angle = (flipperRestAngle(side) + swing) % 256;
    if (angle < 0) {
        angle += 256;
    }

    return angle;
}
