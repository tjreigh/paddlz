#!/usr/bin/env python3
"""Generates the battle-mode flipper sprite as C source.

Draws an outlined, tapered pinball flipper into a square canvas. The hinge
is pixel-aligned to the canvas center because gfx_RotatedScaledSprite and
friends rotate around the sprite's center. The black rim keeps the silhouette
crisp at every angle, while the inset face and center pin make the tiny sprite
read as a mechanical paddle instead of a plain capsule.

Run with `python3 scripts/gen_flipper_sprite.py` and paste the printed
array over `flipperSpriteData` in src/main.c. The trailing preview comment
is just for eyeballing the shape - it isn't part of the C output.
"""

import math

SIZE = 24             # must match FLIPPER_SPRITE_SIZE in main.c
BG = 255              # transparent key, matches BG_COLOR
OUTLINE = 0           # rim and hinge pin, matches PADDLE_COLOR
FACE = 160            # inset face, matches BALL_COLOR

# Blade runs from the hinge (sprite center) toward the tip, in the sprite's
# own rest orientation (pointing toward row 0, "up"). battleFlipperAngle()
# in src/battle.c rotates this via a 256-step angle at draw time.
HINGE = (12.0, 12.0)
TIP = (12.0, 2.5)
OUTER_HINGE_RADIUS = 4.5
OUTER_TIP_RADIUS = 2.75
INNER_HINGE_RADIUS = 3.0
INNER_TIP_RADIUS = 1.35
PIN_RADIUS = 1.25


def distance_to_tapered_blade(px, py, hinge_radius, tip_radius):
    """Return (distance, local radius) at the nearest blade-axis point."""
    dx = TIP[0] - HINGE[0]
    dy = TIP[1] - HINGE[1]
    seg_len2 = dx * dx + dy * dy
    t = ((px - HINGE[0]) * dx + (py - HINGE[1]) * dy) / seg_len2
    t = max(0.0, min(1.0, t))
    cx = HINGE[0] + t * dx
    cy = HINGE[1] + t * dy
    radius = hinge_radius + (tip_radius - hinge_radius) * t
    return math.hypot(px - cx, py - cy), radius


def build_grid():
    grid = [[BG for _ in range(SIZE)] for _ in range(SIZE)]

    for row in range(SIZE):
        for col in range(SIZE):
            px, py = col + 0.5, row + 0.5
            outer_dist, outer_radius = distance_to_tapered_blade(
                px, py, OUTER_HINGE_RADIUS, OUTER_TIP_RADIUS
            )
            inner_dist, inner_radius = distance_to_tapered_blade(
                px, py, INNER_HINGE_RADIUS, INNER_TIP_RADIUS
            )
            pin_dist = math.hypot(px - HINGE[0], py - HINGE[1])

            if outer_dist <= outer_radius:
                grid[row][col] = OUTLINE
            if inner_dist <= inner_radius:
                grid[row][col] = FACE
            if pin_dist <= PIN_RADIUS:
                grid[row][col] = OUTLINE

    return grid


def print_c_array(grid):
    bytes_out = [SIZE, SIZE]
    for row in grid:
        bytes_out.extend(row)

    print("static const uint8_t flipperSpriteData[] = {")
    for i in range(0, len(bytes_out), 16):
        chunk = bytes_out[i:i + 16]
        print("\t" + ", ".join(str(b) for b in chunk) + ",")
    print("};")


def print_preview(grid):
    print("\n/* preview:")
    for row in grid:
        pixels = {BG: ".", OUTLINE: "#", FACE: "+"}
        print(" * " + "".join(pixels[v] for v in row))
    print(" * legend: # outline/pin, + face, . transparent")
    print(" */")


if __name__ == "__main__":
    grid = build_grid()
    print_c_array(grid)
    print_preview(grid)
