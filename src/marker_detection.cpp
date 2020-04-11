// Implement the functions in marker_detection.h
#include "marker_detection.h"
#include "parameters.h"

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

// The length of marker is 0.05 meters
#define MARKER_LENGTH 0.05f

// Give out a list of 4x4 transformation matrices (rotation + translation)
void detectMarkersAndEstimatePose(
	const cv::Mat& input_image,
	std::vector<cv::Mat>& output_marker_poses) {
	if (!output_marker_poses.empty()) {
		output_marker_poses.clear();
	}

	// A list of Marker corners in 2D
	std::vector<std::vector<cv::Point2f>> marker_corners;
	std::vector<int> marker_ids;
	// Detect markers in the image, and store their conrners and ids
	cv::aruco::detectMarkers(input_image, marker_dictionary,
		marker_corners, marker_ids);

	cv::Mat mat_intrinsic_parameters(3, 3, CV_32F,
		const_cast<float*>(intrinsic_parameters));
	cv::Mat mat_distortion_coefficients(1, 5, CV_32F,
		const_cast<float*>(distortion_coefficients));

	size_t num_of_detected_markers = marker_ids.size();
	// For each marker, estimate their pose by using solvePnP
	for (size_t i = 0; i < num_of_detected_markers; i++) {
		cv::Vec3d rotation_vector, translation_vector;

		// Estimate pose of a marker
		cv::solvePnP(canonical_marker_corners_3d, marker_corners[i],
			mat_intrinsic_parameters, mat_distortion_coefficients,
			rotation_vector, translation_vector);

		// Transform a rotation vector to a rotation matrix
		cv::Mat rotation_matrix;
		cv::Rodrigues(rotation_vector, rotation_matrix);
		// Use marker length to standardize translation
		translation_vector = MARKER_LENGTH * translation_vector;

		// Store the poses
		cv::Mat marker_pose = cv::Mat::zeros(4, 4, CV_32F);
		for (uchar row = 0; row < 3; row++) {
			for (uchar column = 0; column < 3; column++) {
				// Since "solvePnP" gives the rotation that is used in OpenGL,
				// then directly store it
				marker_pose.at<float>(row, column) =
					static_cast<float>(
						rotation_matrix.at<double>(row, column));
			}
			// Invert the y-axis and z-axis in order to use in OpenGL
			marker_pose.at<float>(row, 3) =
				row == 0 ?
				static_cast<float>(translation_vector(row)) :
				-static_cast<float>(translation_vector(row));
		}
		marker_pose.at<float>(3, 3) = 1.0f;

		// Column is the priority in OpenGL, so transpose it
		cv::transpose(marker_pose, marker_pose);

		output_marker_poses.push_back(marker_pose);
	}
}

// This function has the same functionality as the previous one
// but it is implemented without "solvePnP"
// So, it is only used for testing
void detectArucoMarkers(
	const cv::Mat& input_image,
	std::vector<cv::Mat>& output_marker_poses) {
	if (!output_marker_poses.empty()) {
		output_marker_poses.clear();
	}

	// A list of Marker corners in 2D
	std::vector<std::vector<cv::Point2f>> marker_corners;
	std::vector<int> marker_ids;
	// Detect markers in the image, and store their conrners and ids
	cv::aruco::detectMarkers(input_image, marker_dictionary,
		marker_corners, marker_ids);

	cv::Mat mat_intrinsic_parameters(3, 3, CV_32F,
		const_cast<float*>(intrinsic_parameters));
	cv::Mat mat_distortion_coefficients(1, 5, CV_32F,
		const_cast<float*>(distortion_coefficients));

	// If any marker is detected, estimate pose
	std::vector<cv::Vec3d> rvecs, tvecs;
	if (!marker_ids.empty()) {
		cv::aruco::estimatePoseSingleMarkers(marker_corners, 0.05f,
			mat_intrinsic_parameters, mat_distortion_coefficients,
			rvecs, tvecs);
	}

	size_t num_of_detected_markers = marker_ids.size();
	// For each marker, estimate their pose by using solvePnP
	for (size_t i = 0; i < num_of_detected_markers; i++) {
		cv::Vec3d rotation_vector = rvecs[i];
		cv::Vec3d translation_vector = tvecs[i];

		// Transform a rotation vector to a rotation matrix
		cv::Mat rotation_matrix;
		cv::Rodrigues(rotation_vector, rotation_matrix);

		// Store the poses
		cv::Mat marker_pose = cv::Mat::zeros(4, 4, CV_32F);
		for (uchar row = 0; row < 3; row++) {
			for (uchar column = 0; column < 3; column++) {
				marker_pose.at<float>(row, column) =
					static_cast<float>(
						rotation_matrix.at<double>(row, column));
			}
			marker_pose.at<float>(row, 3) =
				static_cast<float>(translation_vector(row));
		}
		marker_pose.at<float>(3, 3) = 1.0f;

		// Convert the poses for the use of OpenGL
		cv::Mat cv2gl = cv::Mat::zeros(4, 4, CV_32F);
		cv2gl.at<float>(0, 0) = 1.0f;
		// Invert the y-axis
		cv2gl.at<float>(1, 1) = -1.0f;
		// Invert the z-axis
		cv2gl.at<float>(2, 2) = -1.0f;
		cv2gl.at<float>(3, 3) = 1.0f;
		marker_pose = cv2gl * marker_pose;
		// Column is the priority in OpenGL, so transpose it
		cv::transpose(marker_pose, marker_pose);

		output_marker_poses.push_back(marker_pose);
	}
}
