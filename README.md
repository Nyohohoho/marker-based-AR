# Marker-based Augmented Reality

This assignment has two requirements:
* Use ***solvePnP*** to estimate the pose of each marker.
* Use ***morden OpenGL (3 and 4)*** to draw some graphics on the markers.

## Marker
The marker used in my implementation is **ArUco marker** which is introduced by [**Detection of ArUco Markers**](https://docs.opencv.org/4.2.0/d5/dae/tutorial_aruco_detection.html). Some examples are shown below.

<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/markers.png" height="300">
</p>

***ArUco Library*** provides many different sets of markers. Each set is called a dictionary. The examples shown above are all from the dictionary that contains 250 markers with size of 6x6 binary bits. **Marker length** is an important parameter. According to the official document, it is 0.05 meters.

## Marker Detection
In my implementation, marker detection is accomplished by ***ArUco Library***. Inside this library, there is the function ***detectMarkers*** which is able to detect all complete **ArUco markers** in an image and give four corners of each marker in 2D image coordinate system. As a result, the marker corners in 2D are obtained in this stage.

## Pose Estimation

### From OpenCV to OpenGL
The next stage is pose estimation. The function ***solvePnP*** needs four corners of each marker in 2D image coordinate system and their correspondences in 3D world coordinate system. Since the marker corners in 2D are known from marker detection stage, only the marker corners in 3D needs to be found. The simplest solution is to assume that each marker locates in canonical coordinates in 3D world system, where the four corners are at (-0.5, -0.5, 0.0), (-0.5, +0.5, 0.0), (+0.5, -0.5, 0.0), (+0.5, +0.5, 0.0), respectively. Note that markers are also supposed to lie on a plane in world, so it does not matter that the z-coordinate is zero.

Although camera pose of each marker has been obtained by ***solvePnP***, it is still unable to be used to draw graphics. The difference between OpenGL and OpenCV Coordinate Systems must be considered.

<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/gl_and_cv_coordinate_systems.png" height="300">
</p>

As shown in the figure above, the orientations of y-axis and z-axis are different. In order to apply the pose obtained by ***OpenCV*** to ***OpenGL***, y-axis and z-axis should be inverted. What's more, the matrix in ***OpenGL*** uses **column** as priority and the matrix ***OpenCV*** uses **row** as priority. So, the pose should also be transposed.

### Some Problems
However, when I tried to do inversion on the pose estimated by ***solvePnP***, it did not work! Hence, I also tried to use the function ***estimatePoseSingleMarkers*** in ***ArUco Library*** to obtain pose of each marker, and it worked perfectly. So I drew the axes in order to see what was wrong.

<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/solvePnP_coordinate_system.png" height="300">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/estimatePoseSingleMarkers_coordinate_system.png" height="300">
</p>

The figure on the left is the pose estimated by ***solvePnP***, and the figure on the right is the pose estimated by ***estimatePoseSingleMarkers***. Red line is x-axis, green line is y-axis, and blue line is z-axis.

Here, I met the first problem. The rotation matrices of the two poses estimated by these two functions are opposite to each other in the second column and the third column. See the following figure for an example.

<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/matrices.png" width="1200">
</p>

According to the documents of ***OpenCV***, both of these two functions give the camera pose that transforms points from each marker coordinate system to the camera coordinate system. Based on that, I guess they differ from the direction. ***solvePnP*** returns the camera pose that camera looks at marker, while ***estimatePoseSingleMarkers*** returns the marker pose that camera looks at camera. So, in order to render graphics on marker, marker pose is needed.

Moreover, the translation vector should be multiplied by **marker length**. Here is the second problem. I guess the translation vector actually has the form: "tvec/MARKER_LENGTH".

**Although I successfully implemented this program, yet I still don't know well about these two problems. If you know anything, please tell me!**

## Graphics
At the beginning, I was planning to render the bunny in ***ply*** file with specular shading effect. However, ***ply*** file does not contain information about normals. Although normals can be computed from vertices, it is very cubersome to compute in runtime. So, I used meshlab to export a bunny in ***obj*** file where normals information has been included. Therefore, I wanted to render a bunny in ***obj*** file with specular shading effect and a bunny in ***ply*** file with red-blue color. The shading effect is simply created by the modification of [**OpenGL Tutorial 8**](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/).

<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/figures/two_bunnies.png" height="300">
</p>

Also, when doing AR, the frame or image captured by camera should be rendered by ***OpenGL***. I treated the frame as a texture and mapped it to (-1, -1, 0), (-1, +1, 0), (+1, -1, 0), (+1, +1, 0).

## Demonstration
This is the case of **ArUco markers**. The runtime is about 100 ms for rendering on one marker.
<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/screenshots/bunny.png" height="600">
</p>

This is the case of **Chessboard**. The runtime is about 1000 ms for rendering on one marker.
<p align="center">
  <img src="https://github.com/Nyohohoho/marker-based-AR/blob/master/screenshots/bunny_on_chessboard.png" height="600">
</p>