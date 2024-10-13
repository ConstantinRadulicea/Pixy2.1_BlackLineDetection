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


    std::vector<std::vector<cv::Point>> contours;
    skeleton = bitMatrixToMat(temp_skeleton_bitmatrix);
    cv::findContours(skeleton, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<cv::Point> approxCurve;
    double epsilon = 5.0;  // Tolerance value for approximation
    cv::approxPolyDP(contours[0], approxCurve, epsilon, false);  // Simplify the first contour


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
