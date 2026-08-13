#include <stdbool.h>
#include "collision.h"
#include "ball.h"
#include "paddle.h"

int computeDeflection(int offset, int max_offset)
{
    if (offset < -max_offset) {
        offset = -max_offset;
    } else if (offset > max_offset) {
        offset = max_offset;
    }

    return (offset * BALL_MAX_DEFLECT_SPEED) / max_offset;
}

static bool checkVerticalPaddleCollision(ball_t* ball, const paddle_t* paddle, int ball_x, int ball_y)
{
    // Calculate paddle bounds
    int paddle_left = paddle->pos.x;
    int paddle_right = paddle_left + PADDLE_WIDTH;
    int paddle_top = paddle->pos.y;
    int paddle_bottom = paddle->pos.y + PADDLE_LEN;

    bool x_overlap;
    if (paddle->side == PADDLE_LEFT) {
        x_overlap = ball->vx < 0
            && ball_x >= paddle_right
            && ball_x - BALL_RADIUS <= paddle_right;
    } else {
        x_overlap = ball->vx > 0
            && ball_x <= paddle_left
            && ball_x + BALL_RADIUS >= paddle_left;
    }

    // Ball's vertical position should overlap with paddle
    bool y_overlap = (ball_y + BALL_RADIUS >= paddle_top)
        && (ball_y - BALL_RADIUS <= paddle_bottom);

    // Collision detected if both overlap
    if (x_overlap && y_overlap) {
        int paddle_center = paddle_top + PADDLE_LEN / 2;
        int max_offset = PADDLE_LEN / 2;

        ball->vx = -ball->vx;
        ball->vy = computeDeflection(ball_y - paddle_center, max_offset);
        ball->x = paddle->side == PADDLE_LEFT
            ? (paddle_right + BALL_RADIUS) << FIXED_SHIFT
            : (paddle_left - BALL_RADIUS) << FIXED_SHIFT;

        return true;
    }

    return false;
}

static bool checkHorizontalPaddleCollision(ball_t* ball, const paddle_t* paddle, int ball_x, int ball_y)
{
    // Calculate paddle bounds
    int paddle_top = paddle->pos.y;
    int paddle_bottom = paddle_top + PADDLE_WIDTH;
    int paddle_left = paddle->pos.x;
    int paddle_right = paddle_left + PADDLE_LEN;

    bool y_overlap;
    if (paddle->side == PADDLE_TOP) {
        y_overlap = ball->vy < 0
            && ball_y >= paddle_bottom
            && ball_y - BALL_RADIUS <= paddle_bottom;
    } else {
        y_overlap = ball->vy > 0
            && ball_y <= paddle_top
            && ball_y + BALL_RADIUS >= paddle_top;
    }

    // Ball's horizontal position should overlap with paddle
    bool x_overlap = (ball_x + BALL_RADIUS >= paddle_left)
        && (ball_x - BALL_RADIUS <= paddle_right);

    // Collision detected if both overlap
    if (x_overlap && y_overlap) {
        int paddle_center = paddle_left + PADDLE_LEN / 2;
        int max_offset = PADDLE_LEN / 2;

        ball->vy = -ball->vy;
        ball->vx = computeDeflection(ball_x - paddle_center, max_offset);
        ball->y = paddle->side == PADDLE_TOP
            ? (paddle_bottom + BALL_RADIUS) << FIXED_SHIFT
            : (paddle_top - BALL_RADIUS) << FIXED_SHIFT;

        return true;
    }

    return false;
}

bool checkPaddleCollision(ball_t* ball, const paddle_t* paddle)
{
    // Convert ball position to screen coordinates
    int ball_x = ball->x >> FIXED_SHIFT;
    int ball_y = ball->y >> FIXED_SHIFT;

    return paddleIsVertical(paddle->side)
        ? checkVerticalPaddleCollision(ball, paddle, ball_x, ball_y)
        : checkHorizontalPaddleCollision(ball, paddle, ball_x, ball_y);
}
