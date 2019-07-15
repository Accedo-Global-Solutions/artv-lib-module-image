#ifndef CVL_IMAGE_H
#define CVL_IMAGE_H

#include <stdlib.h>
#include <stdint.h>



#if defined(__APPLE__) && !CVL_FORCE_NON_APPLE

#include <Accelerate/Accelerate.h>

typedef vImagePixelCount CVLImagePixelCount;
typedef size_t CVLImageBytesCount;
typedef vImage_Buffer CVLImageBuffer;
typedef Pixel_8 CVLPixel_8;
typedef Pixel_F CVLPixel_F;
typedef Pixel_8888 CVLPixel_8888;
typedef Pixel_FFFF CVLPixel_FFFF;

#else

#ifndef CVLImagePixelCount
typedef unsigned long CVLImagePixelCount;
#endif

#ifndef CVLImageBytesCount
typedef size_t CVLImageBytesCount;
#endif

/** CVL image buffer structure. */
typedef struct {
    void              *data;     ///< Pointer to the top left pixel of the buffer.
    CVLImagePixelCount height;   ///< The height (in pixels) of the buffer.
    CVLImagePixelCount width;    ///< The width (in pixels) of the buffer.
    CVLImageBytesCount rowBytes; ///< The number of bytes in a pixel row, including any unused space between one row and the next.
} CVLImageBuffer;

/** 8 bit planar pixel value. */
typedef uint8_t CVLPixel_8;

/** Floating point planar pixel value. */
typedef float CVLPixel_F;

/** Interleaved 4 channels (8 bit/channel) pixel value. */
typedef uint8_t CVLPixel_8888[4];

/** Interleaved 4 channels (32 bit/channel, floating point) pixel value. */
typedef float CVLPixel_FFFF[4];

#endif



/** Floating point (double precision) planar pixel value. */
typedef double CVLPixel_D;

/** Interleaved 4 channels (64 bit/channel, floating point) pixel value. */
typedef double CVLPixel_DDDD[4];



/** Integer based point. */
typedef struct {
    int x, y;
} CVLPoint;



/** Integer based recangle. */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} CVLRect;



/** Size of CVLImageBuffer. **/
#define CVLImageBuffer_sz (sizeof(CVLImageBuffer))

/** Size of Pixel_8. */
#define CVLPixel_8_sz (sizeof(CVLPixel_8))

/** Size of Pixel_F. */
#define CVLPixel_F_sz (sizeof(CVLPixel_F))

/** Size of Pixel_D. */
#define CVLPixel_D_sz (sizeof(CVLPixel_D))

/** Size of Pixel_8888. */
#define CVLPixel_8888_sz (sizeof(CVLPixel_8888))

/** Size of Pixel_FFFF. */
#define CVLPixel_FFFF_sz (sizeof(CVLPixel_FFFF))

/** Size of Pixel_DDDD. */
#define CVLPixel_DDDD_sz (sizeof(CVLPixel_DDDD))

/** Get pointer to y line of image with specified type of pixel. */
#define CVL_GET_LINE(TYPE, IMAGE, Y) \
((TYPE *)((CVLPixel_8 *)(IMAGE)->data + (Y) * (IMAGE)->rowBytes))

/** Get pointer to (x, y) pixel of image with specified type of pixel. */
#define CVL_GET_PIXEL(TYPE, IMAGE, Y, X) \
(((TYPE *)((CVLPixel_8 *)(IMAGE)->data + (Y) * (IMAGE)->rowBytes))[X])



/**
 * Assign one CVLPixel_8888 to another.
 *
 * Both arguments @a to and @a from must be of Pixel_8* type (pointers to first
 * channels of pixels to be copied). sizeof(int) must be equal to 4 bytes.
 */
#define CVL_ASSIGN_8888(to, from) \
(*(int*)(to) = *(int const*)(from))

/**
 * Check equality of two CVLPixel_8888 pixels.
 *
 * Both arguments @a a and @a b must be of Pixel_8* type (pointers to first
 * channels of pixels). sizeof(int) must be equal to 4 bytes.
 */
#define CVL_EQUAL_8888(a, b) \
(*(int const*)(a) == *(int const*)(b))



/** Clamp value. */
#define CVL_CLAMP(val, minval, maxval) \
(val) < (minval) ? (minval) : ((val) > (maxval) ? (maxval) : (val))

/** Unused. */
#define CVL_UNUSED(x) ((void)x)



#endif //CVL_IMAGE_H

