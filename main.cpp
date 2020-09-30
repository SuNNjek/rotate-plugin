//
// Created by SuNNjek on 10.07.20.
//

#include <avisynth.h>

#include "src/RotateFilter.h"

const AVS_Linkage *AVS_linkage = 0;

// This is the entry point that gets called by AviSynth to load the plugin
extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors)
{
    AVS_linkage = vectors;

    // Add functions here like so:
    env->AddFunction("Rotate", "c[angle]f[color]i[start]i[end]i[endangle]f[width]i[height]i[aspect]f", RotateFilter::CreateRotate, 0);
    env->AddFunction("HShear", "c[angle]f[color]i[start]i[end]i[endangle]f[width]i[height]i[aspect]f", RotateFilter::CreateHShear, 0);
    env->AddFunction("VShear", "c[angle]f[color]i[start]i[end]i[endangle]f[width]i[height]i[aspect]f", RotateFilter::CreateVShear, 0);

    // Return a description of the plugin
    // For all I know, you can't see this anywhere, but whatever :P
    return "Rotate plugin";
}
