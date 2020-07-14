//
// Created by SuNNjek on 12.07.20.
//

#pragma once

#include <avisynth.h>

#include "ImageRotate.h"

class ImageRotateRGB32 : public ImageRotate<CallbackFn, AllocatePolicyStdNew, AlignmentPolicyBmp, PixelRGB32> {};
class ImageRotatePlanar : public ImageRotate<CallbackFn, AllocatePolicyStdNew, AlignmentPolicyPlanar, BYTE> {};

class RotateFilter : public GenericVideoFilter
{
public:
    RotateFilter(PClip child, double angle, unsigned int backcolor, int startframe, int endframe, double endangle, int newwidth, int newheight, int shearmode, IScriptEnvironment* env);
    ~RotateFilter() { }

    PVideoFrame GetFrame(int n, IScriptEnvironment* env) override;

    static AVSValue CreateRotate(AVSValue args, void* user_data, IScriptEnvironment* env);
    static AVSValue CreateHShear(AVSValue args, void* user_data, IScriptEnvironment* env);
    static AVSValue CreateVShear(AVSValue args, void* user_data, IScriptEnvironment* env);

private:
    double startangle; // degrees
    unsigned int backcolor; // color
    int startframe;
    int endframe;
    double endangle;
    int newwidth;
    int newheight;
    int shearmode;

    PixelRGB32 clrBack; // Background color
    int oldwidth;
    int oldheight;
};

