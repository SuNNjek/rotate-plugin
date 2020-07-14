//
// Created by SuNNjek on 12.07.20.
//

#pragma once

#include <cstdint>
#include <cstring>

void fillInt(unsigned int *start, int count, int pitch, int height, unsigned int value)
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < count; w++)
            start[w] = value;
        start += pitch / 4;
    }
}

void fillByte(BYTE *start, int count, int pitch, int height, BYTE value)
{
    for (int h = 0; h < height; h++)
    {
        memset(start, value, count); // was stupid bug
        start += pitch;
    }
}

inline int PixelClip(int b)
{
    int a = b;
    return (a < 0) ? 0 : ((a > 255) ? 255 : a);
}

static __inline BYTE ScaledPixelClip(int i) {
    return PixelClip((i + 32768) >> 16);
}

inline int RGB2YUV(int rgb)
{
    const int cyb = int(0.114 * 219 / 255 * 65536 + 0.5);
    const int cyg = int(0.587 * 219 / 255 * 65536 + 0.5);
    const int cyr = int(0.299 * 219 / 255 * 65536 + 0.5);

    // y can't overflow
    int y = (cyb * (rgb & 255) + cyg * ((rgb >> 8) & 255) + cyr*((rgb >> 16) & 255) + 0x108000) >> 16;
    int scaled_y = (y - 16) * int(255.0 / 219.0 * 65536 + 0.5);
    int b_y = ((rgb&255) << 16) - scaled_y;
    int u = ScaledPixelClip((b_y >> 10) * int(1 / 2.018 * 1024 + 0.5) + 0x800000);
    int r_y = (rgb & 0xFF0000) - scaled_y;
    int v = ScaledPixelClip((r_y >> 10) * int(1 / 1.596 * 1024 + 0.5) + 0x800000);
    return (y * 256 + u) * 256 + v;
}
