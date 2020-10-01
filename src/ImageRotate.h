//
// Created by SuNNjek on 10.07.20.
//

#pragma once

#if WIN32
    #define _USE_MATH_DEFINES
    #include <cmath>
#else
    #include <math.h>
#endif

#include "Pixel.h"
#include "Alloc.h"
#include "Alignment.h"
#include "Image.h"

typedef bool (*CallbackFn)(double percent_complete);

template<
        typename ProgressAndAbortCallBack = CallbackFn,
        class AllocatorPolicy = AllocatePolicyStdNew,
        class AlignmentPolicy = AlignmentPolicyBmp,
        class PixelType = PixelRGB32
>
class ImageRotate
{
public:
    typedef Image<AllocatorPolicy, AlignmentPolicy, PixelType> Img;

    static Img AllocAndRotate(const Img &src, PixelType clrBack, double angle, double aspect, ProgressAndAbortCallBack *cb = 0)
    {
        if (!src.GetPtr())
            return src;

        Img mid_image(src);

        // mod by Fizick: change intervals from 0...360 to -180..180
        // to prevent any extra rotation for angles -45...45
        // Bring angle to range of (-INF .. 180.0)
        while (angle >= 180.0)
            angle -= 360.0;

        // Bring angle to range of [-180.0 .. 180.0)
        while (angle < -180.0)
            angle += 360.0;

        if (aspect != 1.0)
        {
            if (angle > 90.0)
            {
                // Angle in (90.0 .. 180.0]
                // Rotate image by 180 degrees into temporary image,
                // so it requires only an extra rotation angle
                // of -90.0 .. 0.0 to complete rotation.
                mid_image = Rotate180(src, cb);
                angle -= 180.0;
            }
            else if (angle < -90.0)
            {
                // Angle in [-180.0 .. -90.0)
                // Rotate image by 180 degrees into temporary image,
                // so it requires only an extra rotation angle
                // of 0.0 .. +90.0 to complete rotation.
                mid_image = Rotate180(src, cb);
                angle += 180.0;
            }
        }
        else // aspect == 1, so we may use rotation90
        {
            if (angle > 135.0)
            {
                mid_image = Rotate180(src, cb);
                angle -= 180.0;
            }
            else if (angle > 45.0)
            {
                mid_image = Rotate90(src, cb);
                angle -= 90.0;
            }
            else if (angle < -135.0)
            {
                mid_image = Rotate180(src, cb);
                angle += 180.0;
            }
            else if (angle < -45.0)
            {
                mid_image = Rotate270(src, cb);
                angle += 90.0;
            }
        }

        // check for abort
        if (!mid_image.GetPtr())
            return mid_image;

        // If we got here, angle is in [-90.0 .. +90.0]
        Img dst(Rotate45(mid_image, clrBack, angle, aspect, src.GetPtr() != mid_image.GetPtr(), cb));

        if (src.GetPtr() != mid_image.GetPtr())
        {
            // Middle image was required, free it now.
            mid_image.Free();
        }

        return dst;
    }

    static Img AllocAndHShear(const Img &src, PixelType clrBack, double angle, double aspect, ProgressAndAbortCallBack *cb = 0)
    {
        if (!src.GetPtr())
            return src;

        Img mid_image(src);

        // mod by Fizick: change intervals from 0...360 to -180..180
        // to prevent any extra rotation for angles -45...45
        // Bring angle to range of (-INF .. 180.0)
        while (angle >= 180.0)
            angle -= 360.0;

        // Bring angle to range of [-180.0 .. 180.0)
        while (angle < -180.0)
            angle += 360.0;

        if (angle > 90.0)
        {
            // Rotate image by 180 degrees into temporary image,
            // so it requires only an extra rotation angle
            // of -90.0 .. +0.0 to complete rotation.
            mid_image = Rotate180(src, cb);
            angle -= 180.0;
        }
        else if (angle < -90.0)
        {
            // Rotate image by -180 degrees into temporary image,
            // so it requires only an extra rotation angle
            // of 0.0 .. 90.0 to complete rotation.
            mid_image = Rotate180(src, cb);
            angle += 180.0;
        }

        // check for abort
        if (!mid_image.GetPtr())
            return mid_image;

        // If we got here, angle is in (-90.0 .. +90.0]
        Img dst(HShearUpTo90 (mid_image, clrBack, angle, aspect, src.GetPtr() != mid_image.GetPtr(), cb));

        if (src.GetPtr() != mid_image.GetPtr())
        {
            // Middle image was required, free it now.
            mid_image.Free();
        }

        return dst;
    }

    static Img AllocAndVShear(const Img &src, PixelType clrBack, double angle, double aspect, ProgressAndAbortCallBack *cb = 0)
    {
        if (!src.GetPtr())
            return src;

        Img mid_image(src);

        // mod by Fizick: change intervals from 0...360 to -180..180
        // to prevent any extra rotation for angles -45...45
        // Bring angle to range of (-INF .. 180.0)
        while (angle >= 180.0)
            angle -= 360.0;

        // Bring angle to range of [-180.0 .. 180.0)
        while (angle < -180.0)
            angle += 360.0;

        if (angle > 90.0)
        {
            // Rotate image by 180 degrees into temporary image,
            // so it requires only an extra rotation angle
            // of -90.0 .. +0.0 to complete rotation.
            mid_image = Rotate180(src, cb);
            angle -= 180.0;
        }
        else if (angle < -90.0)
        {
            // Rotate image by -180 degrees into temporary image,
            // so it requires only an extra rotation angle
            // of 0.0 .. 90.0 to complete rotation.
            mid_image = Rotate180(src, cb);
            angle += 180.0;
        }

        // check for abort
        if (!mid_image.GetPtr())
            return mid_image;

        // If we got here, angle is in (-90.0 .. +90.0]
        Img dst(VShearUpTo90(mid_image, clrBack, angle, aspect, src.GetPtr() != mid_image.GetPtr(), cb));

        if (src.GetPtr() != mid_image.GetPtr())
        {
            // Middle image was required, free it now.
            mid_image.Free();
        }

        return dst;
    }

private:
    static Img Rotate45(const Img &src, PixelType clrBack, double dAngle, double dAspect, bool bMidImage, ProgressAndAbortCallBack *cb = 0)
    {
        double dRadAngle = dAngle * M_PI / double(180); // Angle in radians
        double dSinE = sin(dRadAngle);
        double dTan = tan(dRadAngle / 2.0);

        // Calc first shear (horizontal) destination image dimensions
        int dst1_width_delta = (int(double(src.GetHeight()) * fabs(dTan) / dAspect + 0.5) + 1) & 0xFFFFFFFE; // delta  must be even! - Fizick
        Img dst1 (src.GetWidth() + dst1_width_delta, src.GetHeight()); // Fizick
        if (!dst1.GetPtr())
            return dst1;

        // Perform 1st shear (horizontal)
        for (unsigned u = 0; u < dst1.GetHeight(); u++)
        {
            double dShear = dst1_width_delta / 2 + ((int)u - ((int)dst1.GetHeight() - 1) / 2.0) * dTan / dAspect; // Fizick
            int iShear = (int)floor(dShear);
            HorizSkew(src, dst1, u, iShear, uint8_t(255 * (dShear - double(iShear)) + 1), clrBack);

            // Report progress
            if (cb && !((*cb) (
                    bMidImage ? 50.0 + (50.0 / 3) * u / dst1.GetHeight() :
                    (100.0 / 3) * u / dst1.GetHeight()
            )))
            {
                dst1.Free();
                return dst1;
            }
        }

        // Perform 2nd shear  (vertical)
        int ivertex = dst1.GetWidth() / 2 - int((src.GetHeight() - 1) * fabs(dTan) / dAspect + 0.5); // Fizick
        int dst2_height_delta = int(2 * ivertex * fabs(dSinE) * dAspect + 1) & 0xFFFFFFE; // Fizick
        int dst2_height_delta2 = int(dst1.GetWidth() * fabs(dSinE) * dAspect - dst1.GetHeight()) & 0xFFFFFFFE;
        if (dst2_height_delta2 > dst2_height_delta)
            dst2_height_delta = dst2_height_delta2; // use max

        Img dst2 (dst1.GetWidth(), src.GetHeight() + dst2_height_delta); // Fizick
        if (!dst2.GetPtr())
        {
            dst1.Free();
            return dst2;
        }

        double dOffset = (dst2_height_delta + (dst2.GetWidth() - 1) * dSinE * dAspect) / 2.0; // Fizick
        // Variable skew offset
        for (unsigned u = 0; u < dst2.GetWidth(); u++, dOffset -= (dSinE * dAspect))
        {
            int iShear = int (floor(dOffset));
            VertSkew(dst1, dst2, u, iShear, uint8_t(255 * (dOffset - double(iShear)) + 1), clrBack);

            // Report progress
            if (cb && !((*cb) (
                    bMidImage ? 66.0 + (50.0 / 3) * u / dst2.GetHeight() :
                    33.0 + (100.0 / 3) * u / dst2.GetHeight()
            )))
            {
                dst1.Free();
                dst2.Free();
                return dst2;
            }
        }

        // Free result of 1st shear
        dst1.Free();

        int dst3_width_delta = (int(double(src.GetHeight()) * fabs(dSinE) / dAspect + double(src.GetWidth()) * cos(dRadAngle)) + 1 - dst2.GetWidth()) & 0xFFFFFFFE; // Fizick
        // Perform 3rd shear (horizontal)
        Img dst3(dst2.GetWidth() + dst3_width_delta, dst2.GetHeight()); // Fizick
        if (!dst3.GetPtr())
        {
            dst2.Free();
            return dst3;
        }

        dOffset = dst3_width_delta / 2 + (((int)dst3.GetHeight() - 1) / 2.0) * (-dTan) / dAspect; // Fizick
        for (unsigned u = 0; u < dst3.GetHeight(); u++, dOffset += (dTan / dAspect))
        {
            int iShear = int(floor(dOffset));
            HorizSkew(dst2, dst3, u, iShear, uint8_t(255 * (dOffset - double (iShear)) + 1), clrBack);

            if (cb && !((*cb) (
                    bMidImage ? 83.0 + (50.0 / 3) * u / dst3.GetHeight() :
                    66.0 + (100.0 / 3) * u / dst3.GetHeight()
            )))
            {
                dst2.Free();
                dst3.Free();
                return dst3;
            }
        }

        // Free result of 2nd shear
        dst2.Free();

        return dst3;
    }

    static Img Rotate90(const Img &src, ProgressAndAbortCallBack *cb = 0)
    {
        Img dst(src.GetHeight(), src.GetWidth());
        if (!dst.GetPtr())
            return dst;

        for (unsigned y = 0; y < src.GetHeight(); y++)
        {
            const PixelType *pSrc = &src.RGBValue(0, y);
            PixelType *pDst = &dst.RGBValue(y, dst.GetHeight() - 1);

            for (unsigned x = 0; x < src.GetWidth(); x++)
            {
                *pDst = *pSrc;
                pSrc++;
                dst.PrevLine(pDst);
            }

            // Report progress
            if (cb && !((*cb) (50.0 * y / src.GetHeight())))
            {
                dst.Free();
                break;
            }
        }

        return dst;
    }

    static Img Rotate180(const Img &src, ProgressAndAbortCallBack *cb = 0)
    {
        Img dst(src.GetWidth(), src.GetHeight());
        if (!dst.GetPtr())
            return dst;

        for (unsigned y = 0; y < src.GetHeight(); y++)
        {
            const PixelType *pSrc = &src.RGBValue(0, y);
            PixelType *pDst = &dst.RGBValue(dst.GetWidth() - 1, dst.GetHeight() - y - 1);
            for (unsigned x = 0; x < src.GetWidth(); x++)
            {
                *pDst = *pSrc;
                pSrc++;
                pDst--;
            }

            // Report progress
            if (cb && !((*cb) (50.0 * y / src.GetHeight())))
            {
                dst.Free();
                break;
            }
        }

        return dst;
    }

    static Img Rotate270(const Img &src, ProgressAndAbortCallBack *cb = 0)
    {
        Img dst(src.GetHeight(), src.GetWidth());
        if (!dst.GetPtr())
            return dst;

        for (unsigned y = 0; y < src.GetHeight(); y++)
        {
            const PixelType *pSrc = &src.RGBValue(0, y);
            PixelType *pDst = &dst.RGBValue(dst.GetWidth() - y - 1, 0);
            for (unsigned x = 0; x < src.GetWidth(); x++)
            {
                *pDst = *pSrc;
                pSrc++;
                dst.NextLine(pDst);
            }

            // Report progress
            if (cb && !((*cb) (50.0 * y / src.GetHeight())))
            {
                dst.Free();
                break;
            }
        }

        return dst;
    }

    static void HorizSkew(const Img &src, Img &dst, unsigned uRow, int iOffset, uint8_t weight, PixelType clrBack)
    {
        PixelType *p = &dst.RGBValue(0, uRow);

        for (int i = 0u; i < iOffset; i++)
            *p++ = clrBack;

        PixelType pxlOldLeft;
        pxlOldLeft = clrBack;

        for (unsigned i = 0; i < src.GetWidth(); i++) {
            // Loop through row pixels
            PixelType pxlSrc = src.RGBValue(i, uRow);
            // Calculate weights
            PixelType pxlLeft = interp(pxlSrc, clrBack, weight);

            // Check boundries
            if ((i + iOffset >= 0) && (i + iOffset < dst.GetWidth())) {
                // Update left over on source
                dst.RGBValue(i + iOffset, uRow) = pxlSrc - (pxlLeft - pxlOldLeft);
            }

            // Save leftover for next pixel in scan
            pxlOldLeft = pxlLeft;
        }

        // Go to rightmost point of skew
        int i = src.GetWidth() + iOffset;

        // If still in image bounds, put leftovers there
        if (i < (int)dst.GetWidth())
            dst.RGBValue(i, uRow) = pxlOldLeft;

        p = &dst.RGBValue(i, uRow);
        while (++i < (int)dst.GetWidth())
        {   // Clear to the right of the skewed line with background
            *++p = clrBack;
        }
    }

    static void VertSkew(const Img &src, Img &dst, unsigned uCol, int iOffset, uint8_t weight, PixelType clrBack)
    {
        PixelType *p = &dst.RGBValue(uCol, 0);

        for (int i = 0; i < iOffset; i++)
        {
            // Fill gap above skew with background
            *p = clrBack;
            dst.NextLine(p);
        }

        PixelType pxlOldLeft;
        pxlOldLeft = clrBack;

        for (int i = 0; i < (int) src.GetHeight(); i++) {
            // Loop through column pixels
            PixelType pxlSrc = src.RGBValue(uCol, i);
            // Calculate weights
            PixelType pxlLeft = interp(pxlSrc, clrBack, weight);

            // Update left over on source
            // Check boundries
            int iYPos = i + iOffset;
            if ((iYPos >= 0) && (iYPos < (int) dst.GetHeight()))
                dst.RGBValue(uCol, iYPos) = pxlSrc - (pxlLeft - pxlOldLeft);

            // Save leftover for next pixel in scan
            pxlOldLeft = pxlLeft;
        }

        // Go to bottom point of skew
        int i = src.GetHeight() + iOffset;

        // If still in image bounds, put leftovers there
        if (i < (int)dst.GetHeight())
            dst.RGBValue(uCol, i) = pxlOldLeft;

        // Clear below skewed line with background
        p = &dst.RGBValue(uCol, i);
        while (++i < (int)dst.GetHeight())
        {
            dst.NextLine(p);
            *p = clrBack;
        }
    }

    static Img VShearUpTo90(const Img &src, PixelType clrBack, double dAngle, double dAspect, bool bMidImage, ProgressAndAbortCallBack *cb = 0)
    {
        double dRadAngle = dAngle * M_PI / double(180); // Angle in radians
        double dSin = sin(dRadAngle);
        double dCos = cos(dRadAngle);

        if (fabs(dCos) < 0.01)
        { // singular
            Img dst2 (0, src.GetHeight() * 2); // 2 is reserve
            return dst2;
        }

        double dTan = dSin/dCos;

        // Calc shear (vertical) destination image dimensions
        int dst2_height_delta = (int(double(src.GetWidth()) * fabs(dTan) * dAspect + 0.5) + 1) & 0xFFFFFFFE;
        Img dst2 (src.GetWidth(), src.GetHeight() + dst2_height_delta); // Fizick
        if (!dst2.GetPtr())
        {
            dst2.Free();
            return dst2;
        }

        double dOffset = (dst2_height_delta + (dst2.GetWidth() - 1) * dTan * dAspect) / 2.0;
        // Variable skew offset
        for (int u = 0; u < (int)dst2.GetWidth(); u++, dOffset -= (dTan * dAspect))
        {
            int iShear = int(floor(dOffset));
            VertSkew(src, dst2, u, iShear, uint8_t(255 * (dOffset - double(iShear)) + 1), clrBack);

            // Report progress
            if (cb && !((*cb)(
                    bMidImage ? 66.0 + (50.0 / 3) * u / dst2.GetHeight() :
                    33.0 + (100.0 / 3) * u / dst2.GetHeight()
            )))
            {
                dst2.Free();
                return dst2;
            }
        }

        return dst2;
    }

    static Img HShearUpTo90(const Img &src, PixelType clrBack, double dAngle, double dAspect, bool bMidImage, ProgressAndAbortCallBack *cb = 0)
    {
        double dRadAngle = dAngle * M_PI / double(180); // Angle in radians
        double dSin = sin(dRadAngle);
        double dCos = cos(dRadAngle);

        if (fabs(dCos) < 0.01)
        { // singular
            Img dst1 (src.GetWidth() * 2, 0); // 2 is reserve
            return dst1;
        }

        double dTan = dSin/dCos;

        // Calc first shear (horizontal) destination image dimensions
        int dst1_width_delta = (int(double(src.GetHeight()) * fabs(dTan) / dAspect + 0.5) + 1) & 0xFFFFFFFE;
        Img dst1 (src.GetWidth() + dst1_width_delta, src.GetHeight());
        if (!dst1.GetPtr())
            return dst1;

        // Perform 1st shear (horizontal)
        for (unsigned u = 0; u < dst1.GetHeight(); u++)
        {
            double dShear = dst1_width_delta / 2 + ((int)u - ((int)dst1.GetHeight() - 1) / 2.0) * dTan / dAspect;
            int iShear = (int)floor(dShear);
            HorizSkew(src, dst1, u, iShear, uint8_t(255 * (dShear - double(iShear)) + 1), clrBack);

            // Report progress
            if (cb && !((*cb) (
                    bMidImage ? 50.0 + (50.0 / 3) * u / dst1.GetHeight() :
                    (100.0 / 3) * u / dst1.GetHeight()
            )))
            {
                dst1.Free();
                return dst1;
            }
        }

        return dst1;
    }
};
