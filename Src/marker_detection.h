#pragma once

#ifndef MARKER_DETECTION
#define MARKER_DETECTION

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

// Give out a list of 4x4 transformation matrices (rotation + translation)
void detectMarkersAndEstimatePose(
	const cv::Mat& input_image,
	std::vector<cv::Mat_<double>>& output_marker_poses);


#endif // !MARKER_DETECTION
