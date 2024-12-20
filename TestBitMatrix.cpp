#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "approxPolyDP.h"

#define DOWNSCALE_FACTOR 4
#define DOWNSCALE_COLOR_TRESHOLD 0.3f
#define MIN_LINE_LENGTH 1
#define VECTOR_APPROXIMATION_EPSILON 4.0f / (float)DOWNSCALE_FACTOR

//#define IMG_PATH "img/img1.png"
//#define IMG_PATH "img/black.png"
//#define IMG_PATH "img/test1.png"
//#define IMG_PATH "img/test2.png"
//#define IMG_PATH "img/test3.png"
//#define IMG_PATH "img/20241002_194857.jpg" // intersection 1

#define IMG_PATH "img/20241002_194755.jpg" // straight with start lines
//#define IMG_PATH "img/20241002_194910.jpg" // intersection shiny
//#define IMG_PATH "img/20241002_194812.jpg" // curve 1
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
    int width = 208;
    int height = (int)(width * (float)(316.0 / 208.0));
    cv::Size newSize(height, width);
    // Resize the image
    cv::resize(image, dst, newSize, 0.0, 0.0, cv::INTER_LANCZOS4);
    //cv::resize(image, dst, newSize);
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

    BitMatrix scaled;
    size_t downscale_rate = DOWNSCALE_FACTOR;
    scaled.init(bitmatrix_img.getRows() / downscale_rate, bitmatrix_img.getColumns() / downscale_rate);
    BitMatrix::downscale(&scaled, &bitmatrix_img, downscale_rate, DOWNSCALE_COLOR_TRESHOLD);
    return scaled;

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

    std::vector<Point2D_int> longestPath_Point2D = temp_skeleton_bitmatrix.findLongestPath();

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
    
    
    std::vector<Point2D_int> approxCurve_Point2D;
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


std::vector<std::vector<Point2D_int>> gggg(BitMatrix* image, float vector_approximation_epsilon) {
    std::vector<std::vector<Point2D_int>> vectors;
    BitMatrixPosition pixelPosition;
    BitMatrix body(image->getRows(), image->getColumns());
    BitMatrix body_skeleton(image->getRows(), image->getColumns());
    std::vector<Point2D_int> longestPath;
    std::vector<Point2D_int> approxCurve;

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


std::vector<std::vector<Point2D_int>> gggg2(BitMatrix* image, float vector_approximation_epsilon) {
    BitMatrix body(image->getRows(), image->getColumns());
    BitMatrix temp(image->getRows(), image->getColumns());
    std::vector<Point2D_int> longestPath;
    std::vector<Point2D_int> approxCurve;
    std::vector<std::vector<Point2D_int>> vectors;
    BitMatrixPosition pixelPosition;

    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    BitMatrixSkeletonZS(image, &temp);
    for (;;)
    {
        pixelPosition = image->getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        image->floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);

        if (body.countNonZero() < MIN_LINE_LENGTH) {
            continue;
        }
        body.findLongestPath(&longestPath, &temp);

        ramerDouglasPeucker(&longestPath, vector_approximation_epsilon, &approxCurve);
        if (approxCurve.size() > 0) {
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

std::vector<std::vector<Point2D_int>> gggg2_longest_path(BitMatrix* image, float vector_approximation_epsilon) {
    BitMatrix body(image->getRows(), image->getColumns());
    BitMatrix temp(image->getRows(), image->getColumns());
    std::vector<Point2D_int> *longestPath;
    std::vector<Point2D_int> approxCurve;
    std::vector<std::vector<Point2D_int>> vectors;
    BitMatrixPosition pixelPosition;

    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    BitMatrixSkeletonZS(image, &temp);
    for (;;)
    {
        pixelPosition = image->getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        image->floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);

        if (body.countNonZero() < MIN_LINE_LENGTH) {
            continue;
        }

        longestPath = BitMatrix::findLongestPath2(&body, &temp);
        if (longestPath == NULL) {
            continue;
        }
        ramerDouglasPeucker(longestPath, vector_approximation_epsilon, &approxCurve);
        delete longestPath;
        if (approxCurve.size() > 0) {
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



std::vector<std::vector<Point2D_int>> gggg3(BitMatrix* image, float vector_approximation_epsilon) {
    std::vector<std::vector<Point2D_int>> vectors;
    BitMatrixPosition pixelPosition;
    BitMatrix visited(image->getRows(), image->getColumns());
    BitMatrix body_skeleton(image->getRows(), image->getColumns());
    std::vector<Point2D_int> longestPath;
    std::vector<Point2D_int> approxCurve;
    size_t temp_size_1;
    body_skeleton = *image;
    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    auto clock_start = clock();
    //BitMatrixSkeleton(&body_skeleton);
    //BitMatrixSkeletonZS(&body_skeleton);
    for (;;)
    {
        body_skeleton.findLongestPath(&longestPath, &visited);
        BitMatrix::AandNotB(&body_skeleton, &visited);
        temp_size_1 = longestPath.size();
        if (temp_size_1 <= 0){
            break;
        }
        else if (temp_size_1 < 4) {
            continue;
        }

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

cv::Mat TestFunction(BitMatrix bitmatrix_img) {
    BitMatrixSkeletonZS(&bitmatrix_img);
    cv::Mat res = bitMatrixToMat(bitmatrix_img);

    int windowWidth = 400;  // Adjust this value to fit your screen
    int windowHeight = 320; // Adjust this value to fit your screen

    cv::namedWindow("test", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("test", windowWidth, windowHeight);
    cv::imshow("test", res);
    return res;
}


void TestVectors() {
    std::vector<std::vector<Point2D_int>> vectors;
    cv::Mat original_img = cv::imread(IMG_PATH);
    char file_path[] = IMG_PATH;
    BitMatrix bitmatrix_img = imgToBitMatrix(file_path, 0.3);
    BitMatrix temp_bitmatrix_1 = bitmatrix_img;
    cv::Mat image = bitMatrixToMat(bitmatrix_img);
    std::cout << "Black pixels: " << bitmatrix_img.countNonZero() << std::endl;
    

    //temp_bitmatrix_1 = bitmatrix_img;
    //vectors = gggg(&temp_bitmatrix_1, VECTOR_APPROXIMATION_EPSILON);
    temp_bitmatrix_1 = bitmatrix_img;
    vectors = gggg2(&temp_bitmatrix_1, VECTOR_APPROXIMATION_EPSILON);
    temp_bitmatrix_1 = bitmatrix_img;
    vectors = gggg2_longest_path(&temp_bitmatrix_1, VECTOR_APPROXIMATION_EPSILON);
    //temp_bitmatrix_1 = bitmatrix_img;
    //vectors = gggg3(&temp_bitmatrix_1, 3.0f);


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
    int windowWidth = 400;  // Adjust this value to fit your screen
    int windowHeight = 320; // Adjust this value to fit your screen
    cv::namedWindow("original image", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("original image", windowWidth, windowHeight);
    cv::imshow("original image", original_img);

    cv::namedWindow("treshold", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("treshold", windowWidth, windowHeight);
    cv::imshow("treshold", image);

    cv::namedWindow("lines", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("lines", windowWidth, windowHeight);
    cv::imshow("lines", result);


    TestFunction(bitmatrix_img);
    cv::waitKey(0);  // Wait for a key press before closing the window
}

void main() {

    TestVectors();
	//TestBitMatrix();
}
