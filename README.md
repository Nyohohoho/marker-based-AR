# Marker-based Augmented Reality

This assignment requires me to use *solvePnP* to estimate the camera pose, and then use OpenGL to draw some graphics on each marker. I mainly refer to the book ***Mastering OpenCV with Practical Computer Vision Projects*** in order to finsh this assignment. The camera used in my implement is my PC's internal camera, which has been calibrated.

Following the [*tutorial of OpenCV*](https://docs.opencv.org/trunk/d5/dae/tutorial_aruco_detection.html), I use the markers shown in the image below. They are some examples in a marker dictionary that contains 250 markers with 6x6 bit binary pattern.

![marker example](https://docs.opencv.org/trunk/singlemarkerssource.png)

## Marker Detection and Pose Estimation

**Marker detection** is achieved by using the function *detectMarker* in ArUco library. This function need an image and a marker dictionary as input, and then detect markers in the image by referring to the dictionary. Then, it gives a list of marker corners in 2D (imgae coordinates) and a list of marker id. The most important thing is the list of marker corners. Since each marker contains 4 corners, if there are N markers in the image, then the list will contain 4N corners. These corners will be 

**Pose estimation** is achieved by using function *solvePnP* in OpenCV for each marker. This function need to know the 3D coordinates and 2D coordinates of a marker. Here, since a marker can be specified by 4 corners, the 2D coordinates are determined. The only thing left is finding 3D coordinates. The simplest solution is to imagine that they are in canonical 3D coordinates, which are (-0.5, -0.5, 0.0), (-0.5, +0.5, 0.0), (+0.5, -0.5, 0.0), (+0.5, +0.5, 0.0). It does not matter that the z-coordinate is set to zero, because the marker is actually on a paper in the real world (the simplest case).

After obtaining the rotation matrix (which is converted from rotation vector) and translation vector of each marker from *solvePnP*, one more step is necessary. Since *solvePnP* finds camera location, w.r.t marker pose, to get marker pose w.r.t the camera, they must be inverted. In other words, rotation matrix need to be transposed since it is orthogonal, and translation vector should be negated (translation = -translation).

## Demonstration
The demo is shown as a gif. I simply draw a gray cube and three axes on each markers. Unfortunately, it is obvious that the drawn graphics centers offset from the centers of markers, and I have not found out how to solve this problem...
![demo](https://github.com/Nyohohoho/marker-based-AR/blob/master/Screenshots/demo.gif)
