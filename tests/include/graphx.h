#ifndef TEST_GRAPHX_H
#define TEST_GRAPHX_H

/* Host-side no-op replacements for drawing calls used by the logic modules. */
static inline void gfx_SetColor(int color)
{
    (void)color;
}

static inline void gfx_FillCircle(int x, int y, int radius)
{
    (void)x;
    (void)y;
    (void)radius;
}

static inline void gfx_FillRectangle_NoClip(int x, int y, int width, int height)
{
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

#endif
