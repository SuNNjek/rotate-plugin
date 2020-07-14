//
// Created by SuNNjek on 10.07.20.
//

#pragma once

#include "Pixel.h"
#include "Alloc.h"
#include "Alignment.h"

template<
    class AllocatorPolicy = AllocatePolicyStdNew,
    class AlignmentPolicy = AlignmentPolicyBmp,
    class PixelType = PixelRGB32
>
class Image
{
public:
    Image() : width(0), height(0), widthPad(0), ptr(0) { }
    Image(const Image &src) : width(src.width), height(src.height), widthPad(src.widthPad), ptr(src.ptr) { }
    Image(int width, int height) { Allocate(width, height); }

    const unsigned GetWidth() const { return width; }
    const unsigned GetHeight() const { return height; }
    const unsigned GetStride() const { return widthPad; }

    PixelType *GetPtr() { return ptr; }
    const PixelType *GetPtr() const { return ptr; }

    PixelType &RGBValue(int x, int y) { return *((PixelType *)((char *)ptr + widthPad * y) + x); }
    const PixelType &RGBValue(int x, int y) const { return *((PixelType *)((char *)ptr + widthPad * y) + x); }

    void PrevLine (PixelType *& p) const { p = (PixelType *) ((char *)p - widthPad); }
    void NextLine (PixelType *& p) const { p = (PixelType *) ((char *)p + widthPad); }

    void Free()
    {
        AllocatorPolicy::mfree(ptr);
        ptr = 0;
    }

private:
    PixelType *ptr;
    unsigned width, height, widthPad;

    void Allocate(int width, int height)
    {
        this->width = width;
        this->height = height;
        this->widthPad = AlignmentPolicy::aligned_width(width);
        this->ptr = (PixelType*)AllocatorPolicy::allocate(widthPad * height);
    }
};
