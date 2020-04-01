#pragma once

#ifndef DRAW_GRAPHICS
#define DRAW_GRAPHICS

#include <GL/glut.h>

#include <opencv2/opencv.hpp>

static GLint image_width;
static GLint image_height;
static GLubyte* pixeldata;

// Use my PC's internal camera
static cv::VideoCapture internal_camera;
// Current frame from the internal camera
static cv::Mat current_frame;
// 4x4 Projection matrix of current frame
static float projection_matrix[16];
//
static std::vector<cv::Mat_<double>> all_camera_poses;

// Initialzie my PC's internal camera
void initialzeCamera();

// Build the projection matrix for rendering on current frame
void buildProjectionMatrix();

// Used for display graphics
// Called by OpenGL glutDisplayFunc
void display(void);

// Used for marker detection and camera pose estimation
// Called by OpenGl glutTimerFunc
void timer(int);


#endif // !DRAW_GRAPHICS
