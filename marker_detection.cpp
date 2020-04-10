// Implement the functions in marker_detection.h
#include "marker_detection.h"
#include "parameters.h"

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

// Give out a list of 4x4 transformation matrices (rotation + translation)
void detectMarkersAndEstimatePose(
	const cv::Mat& input_image,
	std::vector<cv::Mat_<float>>& output_marker_poses) {
	if (!output_marker_poses.empty()) {
		output_marker_poses.clear();
	}

	// A list of Marker corners in 2D
	std::vector<std::vector<cv::Point2f>> marker_corners;
	std::vector<int> marker_ids;
	// Detect markers in the image, and store their conrners and ids
	cv::aruco::detectMarkers(input_image, marker_dictionary,
		marker_corners, marker_ids);

	// Image in grayscale used for cv::cornerSubPix
	cv::Mat gray_image;
	cv::cvtColor(input_image, gray_image, cv::COLOR_RGB2GRAY);

	size_t num_of_detected_markers = marker_ids.size();
	// For each marker, estimate their pose by using solvePnP
	for (size_t i = 0; i < num_of_detected_markers; i++) {
		cv::Vec3d rotation_vector, translation_vector;

		cv::Mat mat_intrinsic_parameters(3, 3, CV_32F,
			const_cast<float*>(intrinsic_parameters));
		cv::Mat mat_distortion_coefficients(1, 5, CV_32F,
			const_cast<float*>(distortion_coefficients));

		// Refine the four corners of each detected marker
		cv::cornerSubPix(gray_image, marker_corners[i],
			cv::Size(5, 5), cv::Size(-1, -1),
			cv::TermCriteria(
				cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 80, 0.01));

		// Estimate pose of a marker
		cv::solvePnP(canonical_marker_corners_3d, marker_corners[i],
			mat_intrinsic_parameters, mat_distortion_coefficients,
			rotation_vector, translation_vector);

		// Transform a rotation vector to a rotation matrix
		cv::Mat_<double> rotation_matrix;
		cv::Rodrigues(rotation_vector, rotation_matrix);

		cv::Mat_<double> marker_pose(4, 4);
		// Since solvePnP finds camera location, w.r.t marker pose,
		// to get marker pose w.r.t the camera,
		// transformation matrix must be inverted.
		for (uchar j = 0; j < 3; j++) {
			for (uchar k = 0; k < 3; k++) {
				marker_pose(j, k) = rotation_matrix(k, j);
			}
			marker_pose(3, j) = -translation_vector(j);
			marker_pose(j, 3) = 0.0f;
		}
		marker_pose(3, 3) = 1.0f;
		
		output_marker_poses.push_back(marker_pose);
	}
}