#pragma once

#ifndef PARAMETERS
#define PARAMETERS

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#define MARKER_SIZE 0.026

// The intrinsic parameters of my PC's internal camera (3x3 matrix)
static const float intrinsic_parameters[9] = {
	9.4721585489646418e+02f, 0.0f, 6.5256929713596503e+02f,
	0.0f, 9.4984601436177400e+02f, 3.6638013113209280e+02f,
	0.0f, 0.0f, 1.0f
};

// The distortion coefficients of my PC's internal camera (1x5 vector)
static const float distortion_coefficients[5] = {
	5.7631435223608550e-02f, -4.3536148894869725e-01f,
	-6.3402722908834971e-03f, 1.1848583491114485e-03f, 6.3847490014951036e-01f
};

// The canonical coordinates of 4 corners of a marker in 3D space
/*
static const std::vector<cv::Point3f> canonical_marker_corners_3d = {
	cv::Point3f(-MARKER_SIZE / 2.0f, -MARKER_SIZE / 2.0f, 0),
	cv::Point3f(+MARKER_SIZE / 2.0f, -MARKER_SIZE / 2.0f, 0),
	cv::Point3f(+MARKER_SIZE / 2.0f, +MARKER_SIZE / 2.0f, 0),
	cv::Point3f(-MARKER_SIZE / 2.0f, +MARKER_SIZE / 2.0f, 0)
};
*/
static const std::vector<cv::Point3f> canonical_marker_corners_3d = {
	cv::Point3f(-0.5f, -0.5f, 0), cv::Point3f(+0.5f, -0.5f, 0),
	cv::Point3f(+0.5f, +0.5f, 0), cv::Point3f(-0.5f, +0.5f, 0)
};


// The marker dictionary used for marker detection
// This dictionary contains 250 markers of size 6x6
static const cv::Ptr<cv::aruco::Dictionary> marker_dictionary =
	cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

#endif // !PARAMETERS
