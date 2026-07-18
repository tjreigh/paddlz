#include <stdbool.h>
#include "collision.h"
#include "ball.h"
#include "paddle.h"

bool checkPaddleCollision(ball_t* ball, paddle_t* paddle)
{
    // Convert ball position to screen coordinates
    int ball_x = ball->x >> FIXED_SHIFT;
    int ball_y = ball->y >> FIXED_SHIFT;

    // Calculate paddle bounds
    int paddle_left = paddle->pos.x;
    int paddle_right = paddle->pos.x + PADDLE_WIDTH;
    int paddle_top = paddle->pos.y;
    int paddle_bottom = paddle->pos.y + PADDLE_LEN;

    // Check if ball is overlapping with paddle
    // Ball's left edge should be at or past paddle's right edge
    bool x_overlap = (ball_x - BALL_SIZE <= paddle_right) && (ball_x + BALL_SIZE >= paddle_left);

    // Ball's vertical position should overlap with paddle
    bool y_overlap = (ball_y + BALL_SIZE >= paddle_top) && (ball_y - BALL_SIZE <= paddle_bottom);

    // Collision detected if both overlap
    if (x_overlap && y_overlap) {
        // Make sure ball is moving toward the paddle (vx < 0 means moving left toward paddle)
        if (ball->vx < 0) {
            // Reverse horizontal velocity
            ball->vx = -ball->vx;

            // Push ball out of paddle to prevent sticking
            ball->x = (paddle_right + BALL_SIZE) << FIXED_SHIFT;

            return true;
        }
    }

    return false;
}
