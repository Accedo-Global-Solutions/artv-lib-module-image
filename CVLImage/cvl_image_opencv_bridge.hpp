#ifndef CVL_IMAGE_OPENCV_BRIDGE_HPP
#define CVL_IMAGE_OPENCV_BRIDGE_HPP

#include "cvl_image.h"

#include <opencv2/core/core.hpp>



/** Return point constructed from OpenCV point. */
static inline CVLPoint cvl_point_from_opencv(const cv::Point ocv_point) {
    return CVLPoint({ocv_point.x, ocv_point.y});
}



/** Return OpenCV point constructed from point. */
static inline cv::Point cvl_point_to_opencv(const CVLPoint point) {
    return cv::Point(point.x, point.y);
}



/** Return rect constructed from OpenCV rect. */
static inline CVLRect cvl_rect_from_opencv(const cv::Rect ocv_rect) {
    return CVLRect({ocv_rect.x, ocv_rect.y, ocv_rect.width, ocv_rect.height});
}



/** Return OpenCV rect constructed from rect. */
static inline cv::Rect cvl_rect_to_opencv(const CVLRect rect) {
    return cv::Rect(rect.x, rect.y, rect.width, rect.height);
}



/**
 * Wrap OpenCV mat by image buffer.
 * @note Returned image buffer and OpenCV mat will share the same data.
 */
static inline CVLImageBuffer cvl_image_from_opencv(const cv::Mat &mat) {
    return CVLImageBuffer({
        mat.data,
        (CVLImagePixelCount)mat.rows,
        (CVLImagePixelCount)mat.cols,
        (CVLImageBytesCount)mat.step
    });
}



/**
 * Wrap image buffer by OpenCV mat of specified pixel type.
 * @note Returned OpenCV mat and image buffer will share the same data.
 */
static inline cv::Mat cvl_image_to_opencv(const CVLImageBuffer * const image, const int mat_type) {
    return cv::Mat((int)image->height, (int)image->width, mat_type, image->data, image->rowBytes);
}



#endif //CVL_IMAGE_OPENCV_BRIDGE_HPP

