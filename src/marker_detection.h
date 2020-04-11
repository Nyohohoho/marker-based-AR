#pragma once

#ifndef MARKER_DETECTION
#define MARKER_DETECTION

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

// Give out a list of 4x4 transformation matrices (rotation + translation)
void detectMarkersAndEstimatePose(
	const cv::Mat& input_image,
	std::vector<cv::Mat>& output_marker_poses);

// This function has the same functionality as the previous one
// but it is implemented without "solvePnP"
// So, it is only used for testing
void detectArucoMarkers(
	const cv::Mat& input_image,
	std::vector<cv::Mat>& output_marker_poses);


#endif // !MARKER_DETECTION
