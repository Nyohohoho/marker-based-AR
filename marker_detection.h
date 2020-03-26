#pragma once

#ifndef _MARKER_DETECTION_
#define _MARKER_DETECTION_

#include <vector>

#include <opencv2/opencv.hpp>

#include <GL/glut.h>

#include "geometric_types.h"
#include "marker.h"

class MarkerDetection {
// The following variables are used for storage
private:
	// The image to be detected
	cv::Mat input_image;
	// 4 corners of a standard marker in 2-dimension
	std::vector<cv::Point2f> marker_corners_2d;
	// 4 corners of a standard marker in 3-dimension
	std::vector<cv::Point3f> marker_corners_3d;
	// Intrinsic parameters of my PC's camera (3x3)
	float intrinsic[9];
	// Distortion coefficients of my PC's camera (1x5)
	float distortion_coefficients[5];

// The following variable is used for OpenGL drawing
private:
	// Projection matrix (4x4)
	float projection_matrix[16];

public:
	// (Public) Constructor
	MarkerDetection();

	// (Public) Set the input image
	void setImage(const cv::Mat& input_image);

// The following variables are used for functions in runtime 
private:
	// The input image in grayscale
	cv::Mat grayscale_image;
	// The input image which is binarized
	cv::Mat binarized_image;
	// Save contours in the input image
	std::vector<std::vector<cv::Point>> contours;
	// The detected markers in the input image
	std::vector<Marker> detected_markers;

private:
	// (Private) Convert input image to grayscale
	void prepareImage();
	// (Private) Binarize the input image in grayscale
	void performThreshold();
	// (Private) Find contours that contains more points
	// than specified number in the binarized image
	void findContours(int min_contour_points_allowed);
	// (Private) Compute perimeter of a polygon
	float perimeter(const std::vector<cv::Point2f>& polygon);
	// (Private) Find closed contours that can be approximated with 4 points
	void findMarkerCandidates();
	// (Private) Detect markers in the grayscale image
	void detectMarkers();
	// (Private) Estimate the camera poses of the markers
	void estimatePosition();

public:
	// (Public) Detect markers and estimate their camera poses
	void detectAndEstimate();

	// (Public) Get the height of the image
	const GLint& getImageHeight() const;

	// (Public) Get the width of the image
	const GLint& getImageWidth() const;

	// (Public) Get the data of the image
	const uchar* getImageData() const;

	// (Public) Get the projection matrix
	const float* getProjectionMatrix() const;

	// (Public) Get the number of detected marker
	const std::vector<Marker>& getDetectedMarkers() const;
};

#endif // !_MARKER_DETECTION_
