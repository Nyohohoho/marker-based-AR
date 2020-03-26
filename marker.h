#pragma once

#ifndef _MARKER_
#define _MARKER_

#include <iostream>

#include <opencv2/opencv.hpp>

#include "geometric_types.h"

struct Marker {
    // ID of the marker
    int id;
    // Camera pose of the marker
    Transformation transformation;
    // Save points of the marker
    std::vector<cv::Point2f> points;

    Marker();

    // (Public) Compare two markers' ids
    friend bool operator<(const Marker& M1, const Marker& M2);

    static cv::Mat rotate(const cv::Mat& in);
    static int hammDistMarker(const cv::Mat& bits);
    static int mat2id(const cv::Mat& bits);
    static int getMarkerId(const cv::Mat& marker_image, int& n_rotations);
};

#endif // !_MARKER_
