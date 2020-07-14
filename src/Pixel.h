//
// Created by SuNNjek on 10.07.20.
//

#pragma once

#include <stdint.h>

typedef union {
    uint8_t c[4];
    uint32_t val;
    struct { uint8_t b, g, r, a; } bgra;
} PixelRGB32;

inline PixelRGB32 operator -(const PixelRGB32 a, PixelRGB32 b)
{
    PixelRGB32 d(a);
    d.bgra.b -= b.bgra.b;
    d.bgra.g -= b.bgra.g;
    d.bgra.r -= b.bgra.r;
    d.bgra.a -= b.bgra.a;

    return d;
}

inline PixelRGB32 operator *(const PixelRGB32 a, uint8_t w)
{
    PixelRGB32 d;
    d.bgra.b = (int)a.bgra.b * w / 256;
    d.bgra.g = (int)a.bgra.g * w / 256;
    d.bgra.r = (int)a.bgra.r * w / 256;
    d.bgra.a = (int)a.bgra.a * w / 256;

    return d;
}

static uint8_t interp(uint8_t a, uint8_t b, uint8_t w)
{
    return b + (a - b) * w / 256;
}

static PixelRGB32 interp(PixelRGB32 a, PixelRGB32 b, uint8_t w)
{
    PixelRGB32 dst;
    dst.bgra.b = interp(a.bgra.b, b.bgra.b, w);
    dst.bgra.g = interp(a.bgra.g, b.bgra.g, w);
    dst.bgra.r = interp(a.bgra.r, b.bgra.r, w);
    dst.bgra.a = interp(a.bgra.a, b.bgra.a, w);

    return dst;
}
