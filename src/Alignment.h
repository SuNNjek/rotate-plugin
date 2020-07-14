//
// Created by SuNNjek on 10.07.20.
//

#pragma once

class AlignmentPolicyUnaligned
{
public:
    static unsigned aligned_width(unsigned width) { return sizeof(PixelRGB32) * width; }
protected:
    ~AlignmentPolicyUnaligned() { } // to prohibit destruction by client
};

class AlignmentPolicyBmp
{
public:
    static unsigned aligned_width(unsigned width) { return (sizeof(PixelRGB32) * width + 3) & (~3); }
protected:
    ~AlignmentPolicyBmp() { } // to prohibit destruction by client
};

class AlignmentPolicyPlanar
{
public:
    static unsigned aligned_width(unsigned width) { return (sizeof(BYTE) * width + 7) & (~7); }
protected:
    ~AlignmentPolicyPlanar() { } // to prohibit destruction by client
};
