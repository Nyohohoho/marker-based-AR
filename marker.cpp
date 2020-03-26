#include <climits>

#include "marker.h"

Marker::Marker() : id(-1) {}

bool operator<(const Marker& M1, const Marker& M2) {
    return M1.id < M2.id;
}

cv::Mat Marker::rotate(const cv::Mat& in) {
    cv::Mat out;
    in.copyTo(out);
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            out.at<uchar>(i, j) = in.at<uchar>(in.cols - j - 1, i);
        }
    }
    return out;
}

int Marker::hammDistMarker(const cv::Mat& bits) {
    int ids[4][5] = {
      {1,0,0,0,0},
      {1,0,1,1,1},
      {0,1,0,0,1},
      {0,1,1,1,0}
    };

    int distance = 0;

    for (int y = 0; y < 5; y++) {
        // Hamming distance to each possible word
        int min_sum = INT_MAX; 

        for (int p = 0; p < 4; p++) {
            int sum = 0;

            // Count
            for (int x = 0; x < 5; x++) {
                sum += bits.at<uchar>(y, x) == ids[p][x] ? 0 : 1;
            }

            if (min_sum > sum) {
                min_sum = sum;
            }
        }

        distance += min_sum;
    }

    return distance;
}

int Marker::mat2id(const cv::Mat& bits) {
    int val = 0;
    for (int y = 0; y < 5; y++) {
        val <<= 1;
        if (bits.at<uchar>(y, 1)) {
            val |= 1;
        }
        val <<= 1;
        if (bits.at<uchar>(y, 3)) {
            val |= 1;
        }
    }
    return val;
}

int Marker::getMarkerId(const cv::Mat& marker_image, int& n_rotations) {
    assert(marker_image.rows == marker_image.cols);
    assert(marker_image.type() == CV_8UC1);

    cv::Mat binarized_marker_image;;

    // Binarize image
    cv::threshold(marker_image, binarized_marker_image,
        125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Markers are divided in 7x7 regions,
    // of which the inner 5x5 belongs to marker info
    // the external border should be entirely black

    int cell_size = marker_image.rows / 7;

    for (int y = 0; y < 7; y++) {
        int inc = 6;

        // For first and last row, check the whole border
        if (y == 0 || y == 6) {
            inc = 1;
        }

        for (int x = 0; x < 7; x += inc) {
            int cellX = x * cell_size;
            int cellY = y * cell_size;
            cv::Mat cell = binarized_marker_image(
                cv::Rect(cellX, cellY, cell_size, cell_size));

            int non_zero = cv::countNonZero(cell);

            // Can not be a marker because the border element is not black
            if (non_zero > (cell_size * cell_size) / 2) {
                return -1;
            }
        }
    }

    cv::Mat bitMatrix = cv::Mat::zeros(5, 5, CV_8UC1);

    // Get information
    // For each inner square, determine if it is  black or white  
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int cellX = (x + 1) * cell_size;
            int cellY = (y + 1) * cell_size;
            cv::Mat cell = binarized_marker_image(
                cv::Rect(cellX, cellY, cell_size, cell_size));

            int non_zero = cv::countNonZero(cell);
            if (non_zero > (cell_size * cell_size) / 2)
                bitMatrix.at<uchar>(y, x) = 1;
        }
    }

    // Check all possible rotations
    cv::Mat rotations[4];
    int distances[4];

    rotations[0] = bitMatrix;
    distances[0] = hammDistMarker(rotations[0]);

    std::pair<int, int> min_distance(distances[0], 0);

    for (int i = 1; i < 4; i++) {
        // Get the hamming distance to the nearest possible word
        rotations[i] = rotate(rotations[i - 1]);
        distances[i] = hammDistMarker(rotations[i]);

        if (distances[i] < min_distance.first) {
            min_distance.first = distances[i];
            min_distance.second = i;
        }
    }

    n_rotations = min_distance.second;
    if (min_distance.first == 0) {
        return mat2id(rotations[min_distance.second]);
    }

    return -1;
}