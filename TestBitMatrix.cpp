#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "approxPolyDP.h"


#define IMG_PATH "img/20241002_194857.jpg"


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




void TestBitMatrix() {
    BitMatrix bitmatrix_img, temp_bitmatrix, temp_skeleton_bitmatrix;
    cv::Mat image, skeleton;
    bitmatrix_img = imgToBitMatrix(IMG_PATH, 0.25);
    image = bitMatrixToMat(bitmatrix_img);
    cv::imshow("image", image);

    BitMatrixPosition temp_bitmatrixpos = bitmatrix_img.getFirstSetPixel();

    temp_bitmatrix = bitmatrix_img.floodFillOnes(temp_bitmatrixpos.row, temp_bitmatrixpos.column);
    cv::imshow("temp_bitmatrix", bitMatrixToMat(temp_bitmatrix));

    BitMatrixSkeleton(temp_bitmatrix, temp_skeleton_bitmatrix);
    cv::imshow("temp_skeleton_bitmatrix", bitMatrixToMat(temp_skeleton_bitmatrix));


    
    skeleton = bitMatrixToMat(temp_skeleton_bitmatrix);

    /*
    // Find the longest path in the skeleton
    std::vector<cv::Point> longestPath = findLongestPath(skeleton);


    */

    std::vector<Point2D> longestPath_Point2D = temp_skeleton_bitmatrix.findLongestPath();

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
    //double epsilon = 1;  // Tolerance value for approximation
    //cv::approxPolyDP(longestPath, approxCurve, epsilon, false);  // Simplify the first contour
    
    
    std::vector<Point2D> approxCurve_Point2D;
    double epsilon = 1;  // Tolerance value for approximation
    approxCurve_Point2D = approxPolyDP(longestPath_Point2D, epsilon);

    for (size_t i = 0; i < approxCurve_Point2D.size(); i++) {
        approxCurve.push_back(cv::Point(approxCurve_Point2D[i].x, approxCurve_Point2D[i].y));
    }


    cv::Mat result = cv::Mat::zeros(skeleton.size(), CV_8UC3);  // Create a blank canvas

    // Draw the simplified skeleton using the approximate curve
    for (size_t i = 0; i < approxCurve.size() - 1; ++i) {
        cv::line(result, approxCurve[i], approxCurve[i + 1], cv::Scalar(0, 255, 0), 1);
    }

    // Display the result
    cv::imshow("Simplified Skeleton", result);
    
    cv::waitKey(0);  // Wait for a key press before closing the window
}



std::vector<std::vector<Point2D>> gggg(BitMatrix* image, float vector_approximation_epsilon) {
    std::vector<std::vector<Point2D>> vectors;
    BitMatrixPosition pixelPosition;
    BitMatrix body(image->getRows(), image->getColumns());
    BitMatrix body_skeleton(image->getRows(), image->getColumns());
    std::vector<Point2D> longestPath;


    for (;;)
    {
        pixelPosition = image->getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        image->floodFillOnes(pixelPosition.row, pixelPosition.column, &body);
        BitMatrixSkeleton(body, body_skeleton);
        BitMatrix::AandNotB(image, &body);
        if (body.countNonZero() < 100)
        {
            continue;
        }
        body_skeleton.findLongestPath(&longestPath);

        std::vector<Point2D> approxCurve;
        ramerDouglasPeucker(&longestPath, vector_approximation_epsilon, &approxCurve);
        vectors.push_back(approxCurve);
    }
    
    return vectors;
}


void TestVectors() {
    std::vector<std::vector<Point2D>> vectors;
    BitMatrix bitmatrix_img = imgToBitMatrix(IMG_PATH, 0.25);
    cv::Mat image = bitMatrixToMat(bitmatrix_img);
    cv::imshow("image", image);

    vectors = gggg(&bitmatrix_img, 1.0f);

    std::vector<std::vector<cv::Point>> approxCurve;
    for (size_t i = 0; i < vectors.size(); i++) {
        std::vector<cv::Point> temp;
        approxCurve.push_back(temp);
        for (size_t j = 0; j < vectors[i].size(); j++) {
            approxCurve[i].push_back(cv::Point(vectors[i][j].x, vectors[i][j].y));
        }
    }


    cv::Mat result = cv::Mat::zeros(bitmatrix_img.getRows(), bitmatrix_img.getColumns(), CV_8UC3);  // Create a blank canvas

    for (size_t i = 0; i < approxCurve.size(); i++)
    {
        // Draw the simplified skeleton using the approximate curve
        for (size_t j = 0; j < approxCurve[i].size() - 1; ++j) {
            cv::line(result, approxCurve[i][j], approxCurve[i][j + 1], cv::Scalar(0, 255, 0), 1);
        }
    }


    // Display the result
    cv::imshow("Simplified Skeleton", result);
    cv::waitKey(0);  // Wait for a key press before closing the window
}

void main() {
    TestVectors();
	//TestBitMatrix();
}
