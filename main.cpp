#include "parameters.h"
#include "draw_graphics.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <GL/glut.h>

int main(int argc, char** argv) {
    initialzeCamera();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	// Set window as 1280x720
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Marker Based Augmented Reality");
	glutDisplayFunc(&display);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();

	/*
	cv::VideoCapture inputVideo;
    inputVideo.open(0);
    cv::Mat mat_intrinsic_parameters(3, 3, CV_32F,
        const_cast<float*>(intrinsic_parameters));
    cv::Mat mat_distortion_coefficients(1, 5, CV_32F,
        const_cast<float*>(distortion_coefficients));
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    while (inputVideo.grab()) {
        cv::Mat image, imageCopy;
        inputVideo.retrieve(image);
        image.copyTo(imageCopy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);
        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            cv::Vec3d rvecs, tvecs;
            // cv::aruco::estimatePoseSingleMarkers(corners, 0.05, mat_intrinsic_parameters, mat_distortion_coefficients, rvecs, tvecs);
            // draw axis for each marker
            for (int i = 0; i < ids.size(); i++) {
                cv::solvePnP(canonical_marker_corners_3d, corners[i], mat_intrinsic_parameters, mat_distortion_coefficients, rvecs, tvecs);
                cv::aruco::drawAxis(imageCopy, mat_intrinsic_parameters, mat_distortion_coefficients, rvecs, tvecs, 0.5);
            }
        }
        cv::imshow("out", imageCopy);
        char key = (char)cv::waitKey(30);
        if (key == 27)
            break;
    }
	*/
	
	return 0;
}