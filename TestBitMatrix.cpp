#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "thinning.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>


#define IMG_PATH "img1.png"


BitMatrix imgToBitMatrix(const char img_path[], float black_treshold) {
    BitMatrix bitmatrix_img;
    // Read the image from file
    cv::Mat image = cv::imread(img_path);

    // Check if the image was loaded
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image!" << std::endl;
        return bitmatrix_img;
    }

    bitmatrix_img.init(image.rows, image.cols);
    
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            // Access the pixel value (BGR format for colored images)
            cv::Vec3b& color = image.at<cv::Vec3b>(row, col);  // For 3-channel color images

            // You can access individual channels: Blue, Green, Red
            uchar blue = color[0];
            uchar green = color[1];
            uchar red = color[2];

            HSVcolor hsv = rgb2hsv(RGBcolor{ red, green, blue });
            float luminosity = hsv.V;

            if (luminosity <= black_treshold) {
                bitmatrix_img.setBit(row, col);
            }
            else {
                bitmatrix_img.unsetBit(row, col);
            }
        }
    }
    return bitmatrix_img;
}

cv::Mat bitMatrixToMat(BitMatrix &bit_matrix) {
    cv::Mat image(bit_matrix.getRows(), bit_matrix.getColumns(), CV_8UC1);
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            

            if (bit_matrix.getBit(row, col) == true) {
                image.at<uchar>(row, col) = 255;
            }
            else {
                image.at<uchar>(row, col) = 0;
            }
        }
    }
    return image;
}

std::vector<cv::Point> bitMatrixToArray(BitMatrix& bit_matrix) {
    std::vector<cv::Point> arr;

    for (int row = 0; row < bit_matrix.getRows(); ++row) {
        for (int col = 0; col < bit_matrix.getColumns(); ++col) {


            if (bit_matrix.getBit(row, col) == true) {
                cv::Point point;
                point.x = col;
                point.y = row;
                arr.push_back(point);
            }
 
        }
    }
    return arr;
}



// Direction vectors for 8-connected neighbors
int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

// Utility function to check if a point is valid
bool isValid(int x, int y, const cv::Mat& skeleton, const cv::Mat& visited) {
    return (x >= 0 && x < skeleton.cols && y >= 0 && y < skeleton.rows &&
        skeleton.at<uchar>(y, x) == 255 && visited.at<uchar>(y, x) == 0);
}

// BFS to find the farthest point from a given start point
std::pair<cv::Point, int> bfs(const cv::Point& start, const cv::Mat& skeleton) {
    cv::Mat visited = cv::Mat::zeros(skeleton.size(), CV_8UC1);  // Mark visited points
    std::queue<std::pair<cv::Point, int>> q;
    q.push({ start, 0 });
    visited.at<uchar>(start.y, start.x) = 1;

    cv::Point farthest = start;
    int maxDist = 0;

    while (!q.empty()) {
        auto current = q.front();
        cv::Point p = current.first;
        int dist = current.second;
        q.pop();

        // Update the farthest point found
        if (dist > maxDist) {
            maxDist = dist;
            farthest = p;
        }

        // Explore 8-connected neighbors
        for (int i = 0; i < 8; ++i) {
            int newX = p.x + dx[i];
            int newY = p.y + dy[i];

            if (isValid(newX, newY, skeleton, visited)) {
                visited.at<uchar>(newY, newX) = 1;
                q.push({ cv::Point(newX, newY), dist + 1 });
            }
        }
    }

    return { farthest, maxDist };
}

// Find the longest path in the skeleton
std::vector<cv::Point> findLongestPath(const cv::Mat& skeleton) {
    // Start BFS from any skeleton point
    cv::Point start;
    bool foundStart = false;

    // Find any pixel in the skeleton to start the BFS
    for (int y = 0; y < skeleton.rows && !foundStart; y++) {
        for (int x = 0; x < skeleton.cols && !foundStart; x++) {
            if (skeleton.at<uchar>(y, x) == 255) {
                start = cv::Point(x, y);
                foundStart = true;
            }
        }
    }

    // First BFS to find the farthest point from 'start'
    auto farthestFromStart = bfs(start, skeleton);

    // Second BFS from the farthest point found
    auto longestPathResult = bfs(farthestFromStart.first, skeleton);

    // To store the points of the longest path, run BFS again and record the path
    std::queue<std::pair<cv::Point, std::vector<cv::Point>>> q;
    cv::Mat visited = cv::Mat::zeros(skeleton.size(), CV_8UC1);
    q.push({ longestPathResult.first, {longestPathResult.first} });
    visited.at<uchar>(longestPathResult.first.y, longestPathResult.first.x) = 1;

    std::vector<cv::Point> longestPath;

    while (!q.empty()) {
        auto current = q.front();
        cv::Point p = current.first;
        std::vector<cv::Point> path = current.second;
        q.pop();

        if (path.size() > longestPath.size()) {
            longestPath = path;
        }

        for (int i = 0; i < 8; ++i) {
            int newX = p.x + dx[i];
            int newY = p.y + dy[i];

            if (isValid(newX, newY, skeleton, visited)) {
                visited.at<uchar>(newY, newX) = 1;
                std::vector<cv::Point> newPath = path;
                newPath.push_back(cv::Point(newX, newY));
                q.push({ cv::Point(newX, newY), newPath });
            }
        }
    }

    return longestPath;
}



void TestBitMatrix() {
    BitMatrix bitmatrix_img, temp_bitmatrix, temp_skeleton_bitmatrix;
    cv::Mat image, skeleton;
    bitmatrix_img = imgToBitMatrix(IMG_PATH, 0.25);
    image = bitMatrixToMat(bitmatrix_img);
    cv::imshow("image", image);

    temp_bitmatrix = bitmatrix_img.floodFillOnes(0, 0);
    cv::imshow("temp_bitmatrix", bitMatrixToMat(temp_bitmatrix));

    temp_bitmatrix = bitmatrix_img.floodFillOnes(0, 0);
    thinning(temp_bitmatrix, temp_skeleton_bitmatrix);
    cv::imshow("temp_skeleton_bitmatrix", bitMatrixToMat(temp_skeleton_bitmatrix));


    
    skeleton = bitMatrixToMat(temp_skeleton_bitmatrix);

    /*
    // Find the longest path in the skeleton
    std::vector<cv::Point> longestPath = findLongestPath(skeleton);


    */

    std::vector<Point2D> longestPath_Point2D = temp_skeleton_bitmatrix.findLongestPath(&temp_skeleton_bitmatrix);
    std::vector<cv::Point>longestPath;
    for (size_t i = 0; i < longestPath_Point2D.size(); i++) {
        longestPath.push_back(cv::Point(longestPath_Point2D[i].x, longestPath_Point2D[i].y));
    }

    // Visualize the longest path
    cv::Mat pathImage = cv::Mat::zeros(skeleton.size(), CV_8UC3);
    for (size_t i = 0; i < longestPath.size() - 1; ++i) {
        cv::line(pathImage, longestPath[i], longestPath[i + 1], cv::Scalar(0, 255, 0), 1);
    }
    cv::imshow("Longhest path", pathImage);

    std::vector<cv::Point> approxCurve;
    double epsilon = 1;  // Tolerance value for approximation
    cv::approxPolyDP(longestPath, approxCurve, epsilon, false);  // Simplify the first contour


    cv::Mat result = cv::Mat::zeros(skeleton.size(), CV_8UC3);  // Create a blank canvas

    // Draw the simplified skeleton using the approximate curve
    for (size_t i = 0; i < approxCurve.size() - 1; ++i) {
        cv::line(result, approxCurve[i], approxCurve[i + 1], cv::Scalar(0, 255, 0), 1);
    }

    // Display the result
    cv::imshow("Simplified Skeleton", result);
    
    cv::waitKey(0);  // Wait for a key press before closing the window
}

void main() {
	TestBitMatrix();
}
