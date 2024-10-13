#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "thinning.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


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
    cv::Mat image;
    bitmatrix_img = imgToBitMatrix(IMG_PATH, 0.25);
    image = bitMatrixToMat(bitmatrix_img);
    cv::imshow("image", image);

    temp_bitmatrix = bitmatrix_img.floodFillOnes(0, 0);
    cv::imshow("temp_bitmatrix", bitMatrixToMat(temp_bitmatrix));

    temp_bitmatrix = bitmatrix_img.floodFillOnes(0, 0);
    thinning(temp_bitmatrix, temp_skeleton_bitmatrix);
    cv::imshow("temp_skeleton_bitmatrix", bitMatrixToMat(temp_skeleton_bitmatrix));
    
    cv::waitKey(0);  // Wait for a key press before closing the window
}

void main() {
	TestBitMatrix();
}
