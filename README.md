# paddlz

A pong-like game for the TI-84+CE

## Disclaimer

A wise old man (me in 2020) once said:
> This is my first time writing C. I am terrible. This code is a dumpster fire.
Please never replicate anythig that I have done. Please. Dear God.
Someone please take away my computer priveleges

So it's not my first time writing C anymore because I've attempted a computer science degree between now and when I started this project. I know a lot more than I did. I don't know a lot because I am *not* a C developer by any means, but I thought this would be a fun learning experience.

## Playing

- Choose `CPU Match` or `High Score` from the main menu with the up and down arrows. When `CPU Match` is selected, use left and right to choose a difficulty. Press `2nd` to start.
- Press `2nd` to serve the ball.
- Use the up and down arrows to move the paddle.
- Press `Clear` to return to the menu. Press it again from the menu to quit.

In `CPU Match`, beat the CPU paddle in a first-to-five match. After a point, the serve travels toward the player who conceded it; the serve overlay clearly shows the direction. After the match, press `2nd` to start a new game.

- `Easy` moves slowly and reacts after the ball crosses midfield.
- `Normal` tracks the ball for its full incoming flight.
- `Hard` predicts where the ball will arrive after bouncing off the walls.

In `High Score`, keep returning the ball off the right wall for as long as possible. Missing resets the current score. The best score is saved in the archived `PADDLZHS` AppVar and restored the next time the program starts.

## Building

Install the [CE C/C++ Toolchain](https://ce-programming.github.io/toolchain/) and run:

```sh
make
```

The calculator program is written to `bin/PONG.8xp`.

Run the host-side gameplay tests without a calculator or emulator:

```sh
make unit-test
```
