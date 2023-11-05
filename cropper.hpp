#ifndef CROPPER_HPP
#define CROPPER_HPP
#include <stdio.h>
#include <opencv2/opencv.hpp>

namespace Cropper
{
    void crop(const cv::Mat &, cv::Mat &);
    bool is_angle_right(const cv::Point &, const cv::Point &, const cv::Point &, double tolerance = 5.0);
    void erode(const cv::Mat &, cv::Mat &, int intensity = 3);
    void dilate(const cv::Mat &, cv::Mat &, int intensity = 3);
    std::vector<cv::Point> get_contour(const cv::Mat &);
}

#endif /*CROPPER_HPP*/