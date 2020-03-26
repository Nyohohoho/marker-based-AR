#include <cmath>
#include <climits>
#include <utility>
#include <vector>

#include "marker_detection.h"

// The size of a standard marker is 100x100
const uint MARKER_SIZE = 100;

// (Public) Default constructor 
MarkerDetection::MarkerDetection() {
	/*------------Initialize the coordinates of 4 corners in 2D--------------*/
	this->marker_corners_2d.push_back(cv::Point2f(0, 0));
	this->marker_corners_2d.push_back(cv::Point2f(MARKER_SIZE - 1, 0));
	this->marker_corners_2d.push_back(
		cv::Point2f(MARKER_SIZE - 1, MARKER_SIZE - 1));
	this->marker_corners_2d.push_back(cv::Point2f(0, MARKER_SIZE - 1));
	/*----------------------------------END----------------------------------*/

	/*------------Initialize the coordinates of 4 corners in 3D--------------*/
	this->marker_corners_3d.push_back(cv::Point3f(-0.5f, -0.5f, 0));
	this->marker_corners_3d.push_back(cv::Point3f(+0.5f, -0.5f, 0));
	this->marker_corners_3d.push_back(cv::Point3f(+0.5f, +0.5f, 0));
	this->marker_corners_3d.push_back(cv::Point3f(-0.5f, +0.5f, 0));
	/*----------------------------------END----------------------------------*/

	/*------Initialize intrinsic parameters and distortion coefficients------*/
	this->intrinsic[0] = 9.4721585489646418e+02f;
	this->intrinsic[1] = 0.0f;
	this->intrinsic[2] = 6.5256929713596503e+02f;
	this->intrinsic[3] = 0.0f;
	this->intrinsic[4] = 9.4984601436177400e+02f;
	this->intrinsic[5] = 3.6638013113209280e+02f;
	this->intrinsic[6] = 0.0f;
	this->intrinsic[7] = 0.0f;
	this->intrinsic[8] = 1.0f;

	this->distortion_coefficients[0] = 5.7631435223608550e-02f;
	this->distortion_coefficients[1] = -4.3536148894869725e-01;
	this->distortion_coefficients[2] = -6.3402722908834971e-03;
	this->distortion_coefficients[3] = 1.1848583491114485e-03;
	this->distortion_coefficients[4] = 6.3847490014951036e-01;
	/*----------------------------------END----------------------------------*/

	/*-------------------Initialize the projection matrix--------------------*/
	for (int i = 0; i < 16; i++) {
		this->projection_matrix[i] = 0.0f;
	}
	/*----------------------------------END----------------------------------*/
}

// (Public) Set the input image and build projection matrix
void MarkerDetection::setImage(const cv::Mat& input_image) {
	if (!this->input_image.empty()) {
		this->input_image.release();
	}
	input_image.copyTo(this->input_image);

	/*----------------------Set the projection matrix------------------------*/
	// Near clipping distance
	float clipping_near = 0.01f;
	// Far clipping distance
	float clipping_far = 100.0f;

	// (Camera parameters) Focal length in x axis
	float focal_length_x = 9.4721585489646418e+02f;
	// (Camera parameters) Focal length in y axis
	float focal_length_y = 9.4984601436177400e+02f;
	// (Camera parameters) Principle point in x axis
	float principle_point_x = 6.5256929713596503e+02f;
	// (Camera parameters) Principle point in y axis
	float principle_point_y = 3.6638013113209280e+02f;

	projection_matrix[0] =
		-2.0f * focal_length_x / this->input_image.cols;
	projection_matrix[1] = 0.0f;
	projection_matrix[2] = 0.0f;
	projection_matrix[3] = 0.0f;

	projection_matrix[4] = 0.0f;
	projection_matrix[5] =
		2.0f * focal_length_y / this->input_image.rows;
	projection_matrix[6] = 0.0f;
	projection_matrix[7] = 0.0f;

	projection_matrix[8] =
		2.0f * principle_point_x / this->input_image.cols - 1.0;
	projection_matrix[9] =
		2.0f * principle_point_y / this->input_image.rows - 1.0;
	projection_matrix[10] =
		-(clipping_far + clipping_near) / (clipping_far - clipping_near);
	projection_matrix[11] = -1.0f;

	projection_matrix[12] = 0.0f;
	projection_matrix[13] = 0.0f;
	projection_matrix[14] =
		-2.0f * clipping_far * clipping_near / (clipping_far - clipping_near);
	projection_matrix[15] = 0.0f;
	/*----------------------------------END----------------------------------*/
}

// (Private) Convert input image to grayscale
void MarkerDetection::prepareImage() {
	cv::cvtColor(input_image, grayscale_image, cv::COLOR_BGRA2GRAY);
}

// (Private) Binarize the input image in grayscale
void MarkerDetection::performThreshold() {
	cv::adaptiveThreshold(grayscale_image, binarized_image,
		255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 7);
}

// (Private) Find contours that contains more points
// than specified number in the binarized image
void MarkerDetection::findContours(int min_contour_points_allowed) {
	std::vector<std::vector<cv::Point>> all_contours;
	// Find all contours
	cv::findContours(binarized_image, all_contours,
		cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	// Search all contours to find suitable contours
	for (size_t i = 0; i < all_contours.size(); i++) {
		int num_of_contour_point = all_contours[i].size();
		// If the number of contour point is larger than specified number
		if (num_of_contour_point > min_contour_points_allowed) {
			contours.push_back(all_contours[i]);
		}
	}
}

// (Private) Compute perimeter of a polygon
float MarkerDetection::perimeter(const std::vector<cv::Point2f>& polygon) {
	float sum = 0;
	float dx, dy;

	// Compute the sum of distance between adjacent points
	for (size_t i = 0; i < polygon.size(); i++) {
		// The adjacent point of i
		size_t j = (i + 1) % polygon.size();

		// (xi - xj, yi - yj)
		cv::Point2f one_side = polygon[i] - polygon[j];
		// (xi - xj)^2 + (yi - yj)^2
		float square_distance = one_side.dot(one_side);
		// Add square root of the square distance
		sum += std::sqrt(square_distance);
	}

	return sum;
}

// (Private) Find closed contours that can be approximated with 4 points
void MarkerDetection::findMarkerCandidates() {
	std::vector<Marker> possible_markers;
	float min_contour_length_allowed = 100.0f;

	// Store the polygon which is represented as a set of points
	// As output of cv::approxPolyDP
	std::vector<cv::Point2f> approximated_curve;

	// For each contour, analyze if it is a paralelepiped likely to be marker
	for (size_t i = 0; i < contours.size(); i++) {
		// Approximate to a polygon
		cv::approxPolyDP(contours[i],
			approximated_curve,
			// Maximum distance between the original curve and its approximation
			double(contours[i].size()) * 0.05,
			// Closed approximated curve
			true);

		// Only keep the polygon that contains 4 vertices
		if (approximated_curve.size() != 4) {
			continue;
		}

		// Only keep the polygon that is convex
		if (!cv::isContourConvex(approximated_curve)) {
			continue;
		}

		float min_distance = FLT_MAX;
		// Ensure that the distace between adjacent points is large enough
		// so that the quadrilateral is large enough
		for (size_t i = 0; i < 4; i++) {
			size_t j = (i + 1) % 4;

			// (xi - xj, yi - yj)
			cv::Point2f one_side =
				approximated_curve[i] - approximated_curve[j];
			// (xi - xj)^2 + (yi - yj)^2
			float square_distance = one_side.dot(one_side);
			// Find the min distance between adjacent points
			min_distance = std::min(min_distance, square_distance);
		}
		// If the min distance is not very small,
		// save the quadrilateral in possible markers vector
		if (min_distance > min_contour_length_allowed) {
			Marker marker_reference;
			for (int i = 0; i < 4; i++) {
				marker_reference.points.push_back(approximated_curve[i]);
			}
			possible_markers.push_back(marker_reference);
		}
	}

	// Save the points in counterclockwise order
	for (size_t i = 0; i < possible_markers.size(); i++) {
		Marker& marker = possible_markers[i];

		// Trace the two vectors spanning the quadrilateral of a possible marker
		// v1 is the vector by first and second points
		cv::Point v1 = marker.points[1] - marker.points[0];
		// v2 is the vector by first and third points
		cv::Point v2 = marker.points[2] - marker.points[0];

		// Compute determinant of the possible marker
		float determinant = (v1.x * v2.y) - (v1.y * v2.x);
		// If the points are saved in clockwise order
		// the determinant is negative
		if (determinant < 0.0) {
			// Swap the position of first and third points,
			// so that the points are saved in counterclockwise order
			swap(marker.points[1], marker.points[3]);
		}
	}

	// Remove these possible markers whose corners are too close to each other
	std::vector<std::pair<int, int>> too_near_candidates;
	// Compute the average distance of each corner
	// to the nearest corner of the other marker candidate
	for (size_t i = 0; i < possible_markers.size(); i++) {
		const Marker& marker_reference_1 = possible_markers[i];

		for (size_t j = i + 1; j < possible_markers.size(); j++) {
			const Marker& marker_reference_2 = possible_markers[j];

			float square_distance = 0;
			for (int k = 0; k < 4; k++) {
				cv::Point2f one_side =
					marker_reference_1.points[k] - marker_reference_2.points[k];
				square_distance += one_side.dot(one_side);
			}
			// 1/4 * sum((xi - xj)^2 + (yi - yj)^2)
			float distance_between_two_quadrilaterals = square_distance / 4;
			// If the distance is less than 100
			if (distance_between_two_quadrilaterals < 100.0f) {
				too_near_candidates.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	// Mask for removing some possible markers
	std::vector<bool> removal_mask(possible_markers.size(), false);
	// Compare perimeters of two quadrilaterals in a pair of too near candidate
	for (size_t i = 0; i < too_near_candidates.size(); i++) {
		float perimeter1 =
			perimeter(possible_markers[too_near_candidates[i].first].points);
		float perimeter2 =
			perimeter(possible_markers[too_near_candidates[i].second].points);

		size_t removal_index;
		// Remove the candidate that has smaller perimeter
		if (perimeter1 > perimeter2) {
			removal_index = too_near_candidates[i].second;
		} else {
			removal_index = too_near_candidates[i].first;
		}
		removal_mask[removal_index] = true;
	}

	// Return detected candidates
	for (size_t i = 0; i < possible_markers.size(); i++) {
		if (!removal_mask[i]) {
			detected_markers.push_back(possible_markers[i]);
		}
	}
}

// (Private) Detect markers in the grayscale image
void MarkerDetection::detectMarkers() {
	cv::Mat canonical_marker;
	std::vector<Marker> good_markers;

	// Identify the markers
	for (size_t i = 0; i < detected_markers.size(); i++) {
		Marker& marker = detected_markers[i];

		// Find the perspective transfomation
		// that brings current marker to rectangular form
		cv::Mat perspective_transform
			= cv::getPerspectiveTransform(marker.points, marker_corners_2d);

		// Transform image to get a canonical marker image
		cv::warpPerspective(grayscale_image, canonical_marker,
			perspective_transform, cv::Size(MARKER_SIZE, MARKER_SIZE));

		int nRotations;
		int id = Marker::getMarkerId(canonical_marker, nRotations);
		//cout<<"ID:"<<id<<endl;
		if (id != -1) {
			marker.id = id;
			// Sort the points so that they are always in the same order
			// no matter the camera orientation
			// Rotates the order of the elements in the range [first,last),
			// in such a way that the element pointed by middle
			// becomes the new first element.
			std::rotate(marker.points.begin(),
				marker.points.begin() + 4 - nRotations, marker.points.end());

			good_markers.push_back(marker);
		}
	}

	// Refine using subpixel accuracy the corners if more than 1 marker detected
	if (good_markers.size() > 0) {
		// 4 points for each corner
		std::vector<cv::Point2f> precise_corners(4 * good_markers.size());

		for (size_t i = 0; i < good_markers.size(); i++) {
			for (int j = 0; j < 4; j++) {
				// i indicates the i(th) marker
				// j indicates the j(th) point of the marker
				precise_corners[i * 4 + j] = good_markers[i].points[j];
			}
		}

		// Refines the corners
		cv::cornerSubPix(grayscale_image, precise_corners,
			cv::Size(5, 5), cv::Size(-1, -1),
			cv::TermCriteria(
				cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01));

		// Copy back
		for (size_t i = 0; i < good_markers.size(); i++) {
			for (int j = 0; j < 4; j++) {
				// i indicates the i(th) marker
				// j indicates the j(th) point of the marker
				good_markers[i].points[j] = precise_corners[i * 4 + j];
			}
		}
	}

	detected_markers = good_markers;
}

// (Private) Estimate the camera poses of the markers
void MarkerDetection::estimatePosition() {
	for (size_t i = 0; i < detected_markers.size(); i++) {
		Marker& marker_reference = detected_markers[i];

		// Mat_<float> = CV_32F
		cv::Mat_<float> Rvec, Tvec;

		//solvePnP(m_markerCorners3d,m.points,camMatrix,distCoeff,raux,taux);
		cv::Mat mat_intrinsic(3, 3, CV_32F,
			const_cast<float*>(intrinsic));
		cv::Mat mat_distortion_coefficients(1, 5, CV_32F,
			const_cast<float*>(distortion_coefficients));
		cv::solvePnP(marker_corners_3d, marker_reference.points,
			mat_intrinsic, mat_distortion_coefficients, Rvec, Tvec);

		cv::Mat_<float> rotation_matrix(3, 3);
		// Get rotation matrix
		cv::Rodrigues(Rvec, rotation_matrix);

		// Copy to transformation matrix
		for (int col = 0; col < 3; col++) {
			for (int row = 0; row < 3; row++) {
				// Copy rotation component
				marker_reference.transformation.r().mat[row][col]
					= rotation_matrix(row, col);
			}
			// Copy translation component
			marker_reference.transformation.t().data[col] = Tvec(col);
		}

		// Since solvePnP finds camera location, w.r.t marker pose
		// to get marker pose w.r.t the camera, it should be inverted
		marker_reference.transformation =
			marker_reference.transformation.getInverted();
	}
}

// (Public) Detect markers and estimate their camera poses
void MarkerDetection::detectAndEstimate() {
	prepareImage();
	performThreshold();
	contours.clear();
	findContours(200);
	detected_markers.clear();
	findMarkerCandidates();
	detectMarkers();
	// If more than 1 marker detected
	if (this->detected_markers.size() > 0) {
		estimatePosition();
	}
}

// (Public) Get the height of the image
const GLint& MarkerDetection::getImageHeight() const {
	return this->input_image.rows;
}

// (Public) Get the width of the image
const GLint& MarkerDetection::getImageWidth() const {
	return this->input_image.cols;
}

// (Public) Get the data of the image
const uchar* MarkerDetection::getImageData() const {
	return this->input_image.data;
}

// (Public) Get the projection matrix
const float* MarkerDetection::getProjectionMatrix() const {
	return this->projection_matrix;
}

const std::vector<Marker>& MarkerDetection::getDetectedMarkers() const {
	return this->detected_markers;
}

