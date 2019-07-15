#ifndef CVL_IMAGE_UTILS_H
#define CVL_IMAGE_UTILS_H


#include "cvl_image.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



/** Return point with specified coordinates. */
static inline CVLPoint cvl_point_make(const int x, const int y) {
#ifdef _MSC_VER
    CVLPoint p;
    p.x = x;
    p.y = y;
    return p;
#else
    return (CVLPoint){x, y};
#endif
}



/** Return point with zero coordinates. */
static inline CVLPoint cvl_point_make_zero() {
    return cvl_point_make(0, 0);
}



static inline CVLPoint cvl_point_invert(const CVLPoint point) {
    return cvl_point_make(-point.x, -point.y);
}



/** Return rect with zero coordinates and size. */
static inline CVLRect cvl_rect_make(const int x, const int y, const int width, const int height) {
#ifdef _MSC_VER
    CVLRect r;
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
#else
    return (CVLRect){x, y, width, height};
#endif
}



/** Return rect with zero coordinates and size. */
static inline CVLRect cvl_rect_make_empty() {
    return cvl_rect_make(0, 0, 0, 0);
}



/** Return rect around specified point with specified radius for x and y axes. */
static inline CVLRect cvl_rect_make_around_center(const CVLPoint center,
                                                  const int rad_x,
                                                  const int rad_y)
{
#ifdef _MSC_VER
    CVLRect r;
    r.x = center.x - rad_x;
    r.y = center.y - rad_y;
    r.width = 1 + rad_x * 2;
    r.height = 1 + rad_y * 2;
    return r;
#else
    return (CVLRect){center.x - rad_x, center.y - rad_y, 1 + rad_x * 2, 1 + rad_y * 2};
#endif
}



/** Convert pointer to 4 integers (x, y, width, height) to CVLRect structure. */
static inline CVLRect cvl_rect_make_from_values(const int * const values) {
#ifdef _MSC_VER
    CVLRect r;
    r.x = values[0];
    r.y = values[1];
    r.width = values[2];
    r.height = values[3];
    return values ? r : cvl_rect_make_empty();
#else
    return values ? (CVLRect){values[0], values[1], values[2], values[3]} : cvl_rect_make_empty();
#endif
}



/** Return rect moved with specified offset. */
static inline CVLRect cvl_rect_move(const CVLRect rect, const CVLPoint offset) {
    return cvl_rect_make(rect.x + offset.x, rect.y + offset.y,
                         rect.width,        rect.height);
}



/** Return center of rect. */
static inline CVLPoint cvl_rect_center(const CVLRect rect) {
    return cvl_point_make(rect.x + rect.width  / 2,
                          rect.y + rect.height / 2);
}



/** Return origin of rect. */
static inline CVLPoint cvl_rect_origin(const CVLRect rect) {
    return cvl_point_make(rect.x, rect.y);
}



/** Check whether rect is empty. */
static inline bool cvl_rect_is_empty(const CVLRect rect) {
    return rect.width == 0 || rect.height == 0;
}



/** Check whether rectangle is not empty and completely inside an image. */
static inline bool cvl_rect_is_good_roi(const CVLRect rect,
                                        const CVLImageBuffer * const image)
{
    if (rect.width <= 0 || rect.height <= 0) {
        return false;
    }
    
    if (rect.x < 0 || rect.y < 0) {
        return false;
    }
    
    if (rect.x + rect.width > (int)image->width || rect.y + rect.height > (int)image->height) {
        return false;
    }
    
    return true;
}



/**
 * Calculate bounding rectangle around rectangles @a a and @a b.
 *
 * Both rectangles must not be empty (positive width and height) in order for
 * this routine to work correctly.
 */
static inline CVLRect cvl_rect_union(const CVLRect a, const CVLRect b) {
    const int r_x1 = (a.x < b.x) ? (a.x) : (b.x);
    const int r_y1 = (a.y < b.y) ? (a.y) : (b.y);
    const int r_x2 = (a.x + a.width ) > (b.x + b.width ) ? (a.x + a.width ) : (b.x + b.width );
    const int r_y2 = (a.y + a.height) > (b.y + b.height) ? (a.y + a.height) : (b.y + b.height);
    
#ifdef _MSC_VER
    CVLRect r;
    r.x = r_x1;
    r.y = r_y1;
    r.width = r_x2 - r_x1;
    r.height = r_y2 - r_y1;
    return r;
#else
    return (CVLRect){r_x1, r_y1, r_x2 - r_x1, r_y2 - r_y1};
#endif
}



/**
 * Create image by given height, width and pixel size.
 * This function does memory allocation for image data.
 *
 * @see cvl_image_release
 */
static inline CVLImageBuffer cvl_image_create(const CVLImagePixelCount height,
                                              const CVLImagePixelCount width,
                                              const CVLImageBytesCount pixel_size)
{
    assert(height > 0 && width > 0 && pixel_size > 0);
    void * const data = malloc(width * height * pixel_size);
#ifdef _MSC_VER
    CVLImageBuffer image;
    image.data = data;
    image.height = height;
    image.width = width;
    image.rowBytes = width * pixel_size;
    return image;
#else
    return (CVLImageBuffer){data, height, width, width * pixel_size};
#endif
}



/** Return empty image. */
static inline CVLImageBuffer cvl_image_make_empty() {
#ifdef _MSC_VER
    CVLImageBuffer image;
    image.data = NULL;
    image.height = 0;
    image.width = 0;
    image.rowBytes = 0;
    return image;
#else
    return (CVLImageBuffer){NULL, 0, 0, 0};
#endif
}



/** Release image memory. */
static inline void cvl_image_release(CVLImageBuffer * const image) {
    if (!image->data) {
        return;
    }
    free(image->data);
    image->data     = NULL;
    image->height   = 0   ;
    image->width    = 0   ;
    image->rowBytes = 0   ;
}



/** Check whether image is continuous. */
static inline bool cvl_image_is_continuous(const CVLImageBuffer * const image,
                                           const CVLImageBytesCount pixel_size)
{
    return image->width * pixel_size == image->rowBytes;
}



/** Check that image is not empty and has good format. */
static inline bool cvl_image_is_good(const CVLImageBuffer * const image,
                                     const CVLImageBytesCount pixel_size)
{
    return
    image         &&
    image->data   &&
    image->height &&
    image->width  &&
    image->rowBytes >= image->width * pixel_size;
}



/** Return subimage of image at specified roi. */
static inline CVLImageBuffer cvl_image_subimage(const CVLImageBuffer * const image,
                                                const CVLRect roi,
                                                const CVLImageBytesCount  pixel_size)
{
    
    assert(cvl_image_is_good(image, pixel_size));
    assert(cvl_rect_is_good_roi(roi, image));
#ifdef _MSC_VER
    CVLImageBuffer subimage;
    subimage.data = (CVLPixel_8 *)image->data + image->rowBytes * roi.y + pixel_size * roi.x;
    subimage.height = (CVLImagePixelCount)roi.height;
    subimage.width = (CVLImagePixelCount)roi.width;
    subimage.rowBytes = image->rowBytes;
    return subimage;
#else
    return (CVLImageBuffer) {
        (CVLPixel_8 *)image->data + image->rowBytes * roi.y + pixel_size * roi.x,
        (CVLImagePixelCount)roi.height,
        (CVLImagePixelCount)roi.width ,
        image->rowBytes
    };
#endif
}



/**
 * Check if image memory layout is compatible with passed image properties
 * and reallocate image if it is incompatible.
 *
 * Image considered compatible only if it has the same height, width and
 * rowBytes = width * pixel_size.
 * @param image Image which memory layout should be checked.
 * @param height Destination layout height.
 * @param width Destination layout width.
 * @param pixel_size Destination layout pixel memory size. This size should take into account all
 * image channels.
 */
static inline void cvl_image_reuse(CVLImageBuffer * image,
                                   const CVLImagePixelCount height,
                                   const CVLImagePixelCount width,
                                   const CVLImageBytesCount pixel_size)
{
    assert(height > 0 && width > 0 && pixel_size > 0);
    if (cvl_image_is_good(image, pixel_size) && (image->height == height) &&
        (image->width == width) && (image->rowBytes == width * pixel_size))
        return;
    cvl_image_release(image);
    *image = cvl_image_create(height, width, pixel_size);
}



/** Copy image data. */
static inline void cvl_image_copy(const CVLImageBuffer * const source_image,
                                  CVLImageBuffer * const dest_image,
                                  const CVLImageBytesCount pixel_size)
{
    assert(cvl_image_is_good(source_image, pixel_size));
    assert(cvl_image_is_good(dest_image,   pixel_size));
    assert(source_image->width == dest_image->width && source_image->height == dest_image->height);

    if (cvl_image_is_continuous(source_image, pixel_size) && cvl_image_is_continuous(dest_image, pixel_size)) {
        memcpy(dest_image->data, source_image->data, source_image->rowBytes * source_image->height);
    }
    else {
        for (CVLImagePixelCount y = 0; y < source_image->height; ++y)
            memcpy(CVL_GET_LINE(CVLPixel_8, dest_image, y),
                   CVL_GET_LINE(const CVLPixel_8, source_image, y),
                   source_image->width * pixel_size);
    }
}


    
/** Fill image with zeroes. */
static inline void cvl_image_clear(const CVLImageBuffer * const image,
                                   const CVLImageBytesCount pixel_size)
{
    if (cvl_image_is_continuous(image, pixel_size)) {
        memset(image->data, 0, image->height * image->rowBytes);
    }
    else {
        for (CVLImagePixelCount y = 0; y < image->height; ++y) {
            memset(CVL_GET_LINE(CVLPixel_8, image, y), 0, image->width * pixel_size);
        }
    }
}

#ifdef __cplusplus
}  //extern "C" {
#endif

#endif //CVL_IMAGE_UTILS_H

