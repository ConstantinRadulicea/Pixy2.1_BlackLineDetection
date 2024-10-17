#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "approxPolyDP.h"

//#define IMG_PATH "img1.png"
//#define IMG_PATH "img/black.png"
//#define IMG_PATH "img/20241002_194857.jpg" // intersection 1

//#define IMG_PATH "img/20241002_194755.jpg" // straight with start lines
//#define IMG_PATH "img/20241002_194910.jpg" // intersection shiny
#define IMG_PATH "img/20241002_194812.jpg" // curve 1
//#define IMG_PATH "img/20241002_194947.jpg" // curve 2
//#define IMG_PATH "img/20241002_194842.jpg" // curve 3


BitMatrix imgToBitMatrix(const char* _img_path, float black_treshold) {
    BitMatrix bitmatrix_img;
    // Read the image from file
    std::string fp = std::string(_img_path);
    //std::cout << "File path: " << fp << std::endl;
    cv::Mat image = cv::imread(fp);


    // Check if the image was loaded
    if (image.empty()) {
        std::cerr << "Error: Could not open or find the image!" << std::endl;
        return bitmatrix_img;
    }


    cv::Mat dst;
    //cv::Size newSize(320, 200);
    int width = 64;
    cv::Size newSize((int)(width * (float)(320.0/200.0)), width);
    // Resize the image
    //cv::resize(image, dst, newSize, 0.0, 0.0, cv::INTER_AREA);
    cv::resize(image, dst, newSize);
    image = dst;


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
    const char file_path[] = IMG_PATH;
    bitmatrix_img = imgToBitMatrix(file_path, 0.25);
    image = bitMatrixToMat(bitmatrix_img);
    cv::imshow("image", image);

    BitMatrixPosition temp_bitmatrixpos = bitmatrix_img.getFirstSetPixel();

    temp_bitmatrix = bitmatrix_img.floodFillOnes(temp_bitmatrixpos.row, temp_bitmatrixpos.column);
    cv::imshow("temp_bitmatrix", bitMatrixToMat(temp_bitmatrix));

    BitMatrixSkeleton(&temp_bitmatrix, &temp_skeleton_bitmatrix);
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
    std::vector<Point2D> approxCurve;

    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    for (;;)
    {
        pixelPosition = image->getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        image->floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);

        if (body.countNonZero() < 8) {
            continue;
        }
        BitMatrixSkeleton(&body);
        body.findLongestPath(&longestPath);

        
        ramerDouglasPeucker(&longestPath, vector_approximation_epsilon, &approxCurve);
        if (approxCurve.size() > 1) {
            vectors.push_back(approxCurve);
        }
        
        approxCurve.clear();
    }

    // End time
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    std::chrono::duration<double> duration = end - start;
    // Output the result in seconds
    std::cout << "Function execution time: " << duration.count() << " seconds" << std::endl;

    return vectors;
}


std::vector<std::vector<Point2D>> gggg2(BitMatrix* image, float vector_approximation_epsilon) {
    std::vector<std::vector<Point2D>> vectors;
    BitMatrixPosition pixelPosition;
    BitMatrix body(image->getRows(), image->getColumns());
    BitMatrix body_skeleton(image->getRows(), image->getColumns());
    std::vector<Point2D> longestPath;
    std::vector<Point2D> approxCurve;
    body_skeleton = *image;
    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    auto clock_start = clock();
    //BitMatrixSkeleton(&body_skeleton);
    BitMatrixSkeleton2(&body_skeleton);
    for (;;)
    {
        pixelPosition = body_skeleton.getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        body_skeleton.floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);
        //std::cout << "body_skeleton: " << body_skeleton.countNonZero() << " body: " << body.countNonZero() << std::endl;


        if (body.countNonZero() < 1) {
            continue;
        }
        body.findLongestPath(&longestPath);

        
        ramerDouglasPeucker(&longestPath, vector_approximation_epsilon, &approxCurve);
        if (approxCurve.size() > 1) {
            vectors.push_back(approxCurve);
        }
        approxCurve.clear();
        
    }

    // End time
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    std::chrono::duration<double> duration = end - start;
    // Output the result in seconds
    std::cout << "Function execution time: " << duration.count() << " seconds" << " clock_cycles: " << clock() - clock_start << std::endl;

    return vectors;
}


void TestVectors() {
    std::vector<std::vector<Point2D>> vectors;
    char file_path[] = IMG_PATH;
    BitMatrix bitmatrix_img = imgToBitMatrix(file_path, 0.3);
    BitMatrix temp_bitmatrix_1 = bitmatrix_img;
    cv::Mat image = bitMatrixToMat(bitmatrix_img);
    std::cout << "Black pixels: " << bitmatrix_img.countNonZero() << std::endl;
    

    //vectors = gggg(&bitmatrix_img, 3.0f);
    vectors = gggg2(&bitmatrix_img, 3.0f);


    std::vector<std::vector<cv::Point>> approxCurve;
    for (size_t i = 0; i < vectors.size(); i++) {
        std::vector<cv::Point> temp;
        approxCurve.push_back(temp);
        for (size_t j = 0; j < vectors[i].size(); j++) {
            approxCurve[i].push_back(cv::Point(vectors[i][j].x, vectors[i][j].y));
        }
    }


    cv::Mat result = cv::Mat::zeros(bitmatrix_img.getRows(), bitmatrix_img.getColumns(), CV_8UC3);  // Create a blank canvas
    cv::RNG rng(time(0));
    for (size_t i = 0; i < approxCurve.size(); i++)
    {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        // Draw the simplified skeleton using the approximate curve
        for (size_t j = 0; j < approxCurve[i].size() - 1; ++j) {
            cv::line(result, approxCurve[i][j], approxCurve[i][j + 1], color, 1);
        }
    }


    // Display the result
    // Create a window
    
    // Resize the window to a specific size (adjust width and height as needed)
    int windowWidth = 800;  // Adjust this value to fit your screen
    int windowHeight = 600; // Adjust this value to fit your screen
    cv::namedWindow("image", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("image", windowWidth, windowHeight);
    cv::imshow("image", image);

    cv::namedWindow("Simplified Skeleton", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("Simplified Skeleton", windowWidth, windowHeight);
    cv::imshow("Simplified Skeleton", result);
    cv::waitKey(0);  // Wait for a key press before closing the window
}

void main() {

    TestVectors();
	//TestBitMatrix();
}
