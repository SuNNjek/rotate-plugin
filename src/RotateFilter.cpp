//
// Created by SuNNjek on 12.07.20.
//

#include <avisynth.h>

#include "Helper.h"
#include "RotateFilter.h"

RotateFilter::RotateFilter(PClip child, double angle, unsigned int backcolor, int startframe, int endframe,
                           double endangle, int newwidth, int newheight, int shearmode, IScriptEnvironment *env) : GenericVideoFilter(child)
{
    oldwidth = vi.width;
    oldheight = vi.height;

    if (newwidth > 0)
    {
        this->newwidth = newwidth;
        vi.width = this->newwidth;
    }
    else
    {
        this->newwidth = vi.width;
    }

    if (newheight > 0)
    {
        this->newheight = newheight;
        vi.height = this->newheight;
    }
    else
    {
        this->newheight = vi.height;
    }


    if (!vi.IsRGB32() && !vi.IsYV12())
        env->ThrowError("Rotate: image format must be RGB32 or YV12!");

    this->shearmode = shearmode;

    if (!vi.IsRGB32())
    {
        this->startangle = -angle; // negative: frame top is bottom
        this->endangle = -endangle; // negative: frame top is bottom
        this->backcolor = RGB2YUV(backcolor);
    }
    else // YV12
    {
        this->startangle = angle;
        this->backcolor = backcolor;
        this->endangle = endangle;
    }

    this->startframe = startframe;
    this->endframe = (endframe < 0) ? (vi.num_frames - 1) : endframe;

    this->clrBack.val = backcolor;
}

PVideoFrame RotateFilter::GetFrame(int n, IScriptEnvironment *env)
{
    double angle = (endframe == startframe) ? startangle : startangle + (n-startframe)*(endangle - startangle)/(endframe - startframe);

    PVideoFrame src = child->GetFrame(n, env);

    // Request frame 'n' from the child (source) clip.
    if ((n<startframe || n>endframe) && oldwidth==newwidth && oldheight==newheight) // do not rotate frames out of range
        return src;

    PVideoFrame dst = env->NewVideoFrame(vi);

    if (vi.IsRGB32())
    {
        const unsigned char* srcp = src->GetReadPtr();
        unsigned char* dstp = dst->GetWritePtr();

        const int dst_pitch = dst->GetPitch();
        const int dst_width = dst->GetRowSize();
        const int dst_height = dst->GetHeight();

        const int src_pitch = src->GetPitch();
        const int src_width = src->GetRowSize();
        const int src_height = src->GetHeight();

        // This code deals with RGB32 colourspace where each pixel is represented by
        // 4 bytes, Blue, Green and Red and "spare" byte that could/should be used for alpha
        // keying but usually isn't.
        ImageRotateRGB32::Img srcimg(src_width / 4, src_height);
        BYTE *srcimgp = reinterpret_cast<BYTE *>(srcimg.GetPtr());

        // copy source to img
        env->BitBlt(srcimgp, srcimg.GetStride(), srcp, src_pitch, src_width, src_height);

        // Do not use ProgressAnbAbortCallBack *cb = 0
        ImageRotateRGB32::Img dstimg;
        if (shearmode == 1)
            dstimg = ImageRotateRGB32::AllocAndHShear(srcimg, clrBack, angle, 0);
        else if (shearmode == 2)
            dstimg = ImageRotateRGB32::AllocAndVShear(srcimg, clrBack, angle, 0);
        else //(shearmode == 0)
            dstimg = ImageRotateRGB32::AllocAndRotate(srcimg, clrBack, angle, 0);

        srcimg.Free();

        BYTE *dstimgp = reinterpret_cast<BYTE *>(dstimg.GetPtr());

        // copy dest img to dst frame
        int hdif = (int)dstimg.GetHeight() - dst_height; // height difference
        int hhdif = hdif / 2; // half height difference
        int hoffset = abs((int)dstimg.GetStride() * hhdif) & (0xFFFFFFFC); // multiple 4
        int hoffsetdst = abs((int)dst_pitch * hhdif) & (0xFFFFFFFC); // multiple 4

        int wdif = (int)dstimg.GetWidth() * 4 - dst_width;
        int woffset = abs(wdif / 2) & (0xFFFFFFFC); // multiple 4

        if (wdif >= 0 && hdif >= 0)
        {
            env->BitBlt(dstp, dst_pitch, dstimgp + hoffset + woffset, dstimg.GetStride(), dst_width, dst_height);
        }
        else if (wdif < 0 && hdif >= 0)
        {
            fillInt(reinterpret_cast<unsigned int *>(dstp), woffset / 4, dst_pitch, dst_height, backcolor);
            env->BitBlt(dstp + woffset, dst_pitch, dstimgp + hoffset, dstimg.GetStride(), dstimg.GetWidth() * 4, dst_height);
            fillInt(reinterpret_cast<unsigned int *>(dstp + woffset + dstimg.GetWidth() * 4), woffset / 4, dst_pitch, dst_height, backcolor);
        }
        else if (wdif >= 0 && hdif < 0)
        {
            fillInt(reinterpret_cast<unsigned int *>(dstp), dst_width / 4, dst_pitch, -hhdif, backcolor);
            env->BitBlt(dstp + hoffsetdst, dst_pitch, dstimgp + woffset, dstimg.GetStride(), dst_width, dstimg.GetHeight());
            fillInt(reinterpret_cast<unsigned int *>(dstp+hoffsetdst+dstimg.GetHeight()*dst_pitch), dst_width / 4, dst_pitch, -hhdif, backcolor);
        }
        else // if (wdif < 0 && hdif < 0)
        {
            fillInt(reinterpret_cast<unsigned int *>(dstp), dst_width / 4, dst_pitch, -hhdif, backcolor);
            fillInt(reinterpret_cast<unsigned int *>(dstp + hoffsetdst), woffset / 4, dst_pitch, dst_height, backcolor);
            env->BitBlt(dstp + hoffsetdst + woffset, dst_pitch, dstimgp, dstimg.GetStride(), dstimg.GetWidth() * 4, dst_height);
            fillInt(reinterpret_cast<unsigned int *>(dstp + hoffsetdst + woffset + dstimg.GetWidth() * 4), woffset / 4, dst_pitch, dst_height, backcolor);
            fillInt(reinterpret_cast<unsigned int *>(dstp + hoffset + dstimg.GetHeight() * dst_pitch), dst_width / 4, dst_pitch, -hhdif, backcolor);
        }

        dstimg.Free();
    }
    else // Planar
    {
        for (int i = 0; i < 3; i++)
        {
            int plane;
            if (i == 0) plane = PLANAR_Y;
            else if (i == 1) plane = PLANAR_U;
            else if (i == 2) plane = PLANAR_V;

            const unsigned char* srcp = src->GetReadPtr(plane);
            unsigned char* dstp = dst->GetWritePtr(plane);

            const int dst_pitch = dst->GetPitch(plane);
            const int dst_width = dst->GetRowSize(plane);
            const int dst_height = dst->GetHeight(plane);
            const int src_pitch = src->GetPitch(plane);
            const int src_width = src->GetRowSize(plane);
            const int src_height = src->GetHeight(plane);

            ImageRotatePlanar::Img srcimg (src_width, src_height);

            BYTE *srcimgp = reinterpret_cast<BYTE *>(srcimg.GetPtr());

            // copy source to img
            env->BitBlt(srcimgp, srcimg.GetStride(), srcp, src_pitch, src_width, src_height);

            BYTE color = clrBack.c[2 - i]; // specific plane color
            // Do not use ProgressAnbAbortCallBack *cb = 0

            ImageRotatePlanar::Img dstimg;
            if (shearmode==1)
                dstimg = ImageRotatePlanar::AllocAndHShear(srcimg, color, angle, 0);
            else if (shearmode==2)
                dstimg = ImageRotatePlanar::AllocAndVShear(srcimg, color, angle, 0);
            else //(shearmode == 0)
                dstimg = ImageRotatePlanar::AllocAndRotate(srcimg, color, angle, 0);

            srcimg.Free();

            BYTE *dstimgp = reinterpret_cast<BYTE *>(dstimg.GetPtr());

            // copy rotated dest img to dst frame and fill empty space
            int hdif = (int)dstimg.GetHeight() - dst_height; // height difference
            int hhdif = hdif/2; // half height difference ???
            int hoffset = (int)dstimg.GetStride() * abs(hhdif);
            int hoffsetdst = (int)dst_pitch * abs(hhdif);

            int wdif = (int)dstimg.GetWidth() - dst_width;
            int woffset = abs(wdif / 2);

            if (wdif >= 0 && hdif >= 0)
            {
                env->BitBlt(dstp, dst_pitch, dstimgp + hoffset + woffset, dstimg.GetStride(), dst_width, dst_height);
            }
            else if (wdif < 0 && hdif >= 0)
            {
                fillByte( dstp, woffset, dst_pitch, dst_height, color);
                env->BitBlt(dstp + woffset, dst_pitch, dstimgp + hoffset, dstimg.GetStride(), dstimg.GetWidth(), dst_height);
                fillByte(dstp + woffset + dstimg.GetWidth(), woffset, dst_pitch, dst_height, color);
            }
            else if (wdif >= 0 && hdif < 0)
            {
                fillByte( dstp, dst_width, dst_pitch, -hhdif, color);
                env->BitBlt(dstp + hoffsetdst, dst_pitch, dstimgp + woffset, dstimg.GetStride(), dst_width, dstimg.GetHeight());
                fillByte(dstp + hoffsetdst + dstimg.GetHeight() * dst_pitch, dst_width, dst_pitch, -hhdif, color);
            }
            else // if (wdif < 0 && hdif < 0)
            {
                fillByte( dstp, dst_width, dst_pitch, -hhdif, color);
                fillByte( dstp + hoffsetdst, woffset, dst_pitch, dst_height, color);
                env->BitBlt(dstp + hoffsetdst + woffset, dst_pitch, dstimgp, dstimg.GetStride(), dstimg.GetWidth(), dst_height);
                fillByte(dstp + hoffsetdst + woffset + dstimg.GetWidth(), woffset, dst_pitch, dst_height, color);
                fillByte( dstp + hoffset + dstimg.GetHeight() * dst_pitch, dst_width, dst_pitch, -hhdif, color);
            }

            dstimg.Free();
        }
    }

    // As we now are finished processing the image, we return the destination image.
    return dst;
}

AVSValue RotateFilter::CreateRotate(AVSValue args, void *user_data, IScriptEnvironment *env)
{
    double angle = args[1].AsFloat(0);
    return new RotateFilter(args[0].AsClip(), // the 0th parameter is the source clip
                            angle, // rotation angle in degrees.
                            args[2].AsInt(0), // background color (integer or hex or global color constant like color_gray).
                            args[3].AsInt(0), // start frame
                            args[4].AsInt(-1), // end frame (-1 as latest)
                            args[5].AsFloat(float(angle)), // end rotation angle in degrees.
                            args[6].AsInt(0), // destination width. 0 - same as source
                            args[7].AsInt(0), // destination height.
                            0, // rotaion or shear mode
                            env);
}

AVSValue RotateFilter::CreateHShear(AVSValue args, void *user_data, IScriptEnvironment *env)
{
    double angle = args[1].AsFloat(0);
    return new RotateFilter(args[0].AsClip(), // the 0th parameter is the source clip
                            angle, // rotation angle in degrees.
                            args[2].AsInt(0), // background color (integer or hex or global color constant like color_gray).
                            args[3].AsInt(0), // start frame
                            args[4].AsInt(-1), // end frame (-1 as latest)
                            args[5].AsFloat(float(angle)), // end rotation angle in degrees.
                            args[6].AsInt(0), // destination width. 0 - same as source
                            args[7].AsInt(0), // destination height.
                            1, // rotaion or shear mode
                            env);
}

AVSValue RotateFilter::CreateVShear(AVSValue args, void *user_data, IScriptEnvironment *env)
{
    double angle = args[1].AsFloat(0);
    return new RotateFilter(args[0].AsClip(), // the 0th parameter is the source clip
                            angle, // rotation angle in degrees.
                            args[2].AsInt(0), // background color (integer or hex or global color constant like color_gray).
                            args[3].AsInt(0), // start frame
                            args[4].AsInt(-1), // end frame (-1 as latest)
                            args[5].AsFloat(float(angle)), // end rotation angle in degrees.
                            args[6].AsInt(0), // destination width. 0 - same as source
                            args[7].AsInt(0), // destination height.
                            2, // rotaion or shear mode
                            env);
}
