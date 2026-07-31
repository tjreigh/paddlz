# paddlz

A pong-like game for the TI-84+CE

## Disclaimer

A wise old man (me 3 years ago) once said:
> This is my first time writing C. I am terrible. This code is a dumpster fire.
Please never replicate anythig that I have done. Please. Dear God.
Someone please take away my computer priveleges

So it's not my first time writing C anymore because I've attempted a computer science degree between now and when I started this project. I know a lot more than I did. I don't know a lot because I am *not* a C developer by any means, but I thought about this project one day, so now I want to work on it until it's "done."

My intention is not for this to be used by people; I am doing this purely for the sake of doing it. I enjoy computers (not enough to do them as my job though) and I enjoy learning, so I will use this as an opportunity to learn more about the deeper workings of the TI-84. Yay disclaimer over

## Playing

- Press `2nd` to serve the ball.
- Use the up and down arrows to move the paddle.
- Keep the rally going to increase your score.
- Press `Clear` to quit.

Missing the ball resets the current score. The best score is kept until the program exits.

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
