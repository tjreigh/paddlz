#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>

#include "ball.h"
#include "paddle.h"

bool checkPaddleCollision(ball_t* ball, const paddle_t* paddle);

#endif
