#include <stdbool.h>
#include "collision.h"
#include "ball.h"
#include "paddle.h"

bool checkPaddleCollision(ball_t* ball, const paddle_t* paddle)
{
    // Convert ball position to screen coordinates
    int ball_x = ball->x >> FIXED_SHIFT;
    int ball_y = ball->y >> FIXED_SHIFT;

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
        int impact_offset = ball_y - paddle_center;
        int max_offset = PADDLE_LEN / 2;

        if (impact_offset < -max_offset) {
            impact_offset = -max_offset;
        } else if (impact_offset > max_offset) {
            impact_offset = max_offset;
        }

        ball->vx = -ball->vx;
        ball->vy = (impact_offset * BALL_MAX_VERTICAL_SPEED) / max_offset;
        ball->x = paddle->side == PADDLE_LEFT
            ? (paddle_right + BALL_RADIUS) << FIXED_SHIFT
            : (paddle_left - BALL_RADIUS) << FIXED_SHIFT;

        return true;
    }

    return false;
}
