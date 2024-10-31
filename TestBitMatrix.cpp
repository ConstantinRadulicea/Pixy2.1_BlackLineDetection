#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "approxPolyDP.h"
#include "Matrix.h"

#define DOWNSCALE_FACTOR 4
#define DOWNSCALE_COLOR_TRESHOLD 0.3f
#define MIN_LINE_LENGTH 1
#define VECTOR_APPROXIMATION_EPSILON 4.0f / (float)DOWNSCALE_FACTOR
#define BLACK_TRERSHOLD 0.2f

//#define IMG_PATH "img/img1.png"
//#define IMG_PATH "img/black.png"
//#define IMG_PATH "img/test1.png"
//#define IMG_PATH "img/test2.png"
//#define IMG_PATH "img/test3.png"
#define IMG_PATH "img/20241002_194857.jpg" // intersection 1

//#define IMG_PATH "img/20241002_194755.jpg" // straight with start lines
//#define IMG_PATH "img/20241002_194910.jpg" // intersection shiny
//#define IMG_PATH "img/20241002_194812.jpg" // curve 1
//#define IMG_PATH "img/20241002_194947.jpg" // curve 2
//#define IMG_PATH "img/20241002_194842.jpg" // curve 3


cv::Mat convertToBayerPattern(const cv::Mat& inputImage) {
    if (inputImage.empty()) {
        throw std::invalid_argument("Input image is empty.");
    }

    // Ensure the image is in BGR format
    cv::Mat bgrImage;
    if (inputImage.channels() == 1) {
        cv::cvtColor(inputImage, bgrImage, cv::COLOR_GRAY2BGR);
    }
    else {
        bgrImage = inputImage.clone();
    }

    // Create an empty Bayer pattern image
    cv::Mat bayerImage(bgrImage.size(), CV_8UC1);

    // Assign Bayer pattern (RGGB)
    for (int y = 0; y < bgrImage.rows; ++y) {
        for (int x = 0; x < bgrImage.cols; ++x) {
            // Bayer pattern: RGGB
            if ((y % 2 == 0) && (x % 2 == 0)) {
                // Red pixel (R)
                bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[2];
            }
            else if ((y % 2 == 0) && (x % 2 == 1)) {
                // Green pixel (G1)
                bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[1];
            }
            else if ((y % 2 == 1) && (x % 2 == 0)) {
                // Green pixel (G2)
                bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[1];
            }
            else {
                // Blue pixel (B)
                bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[0];
            }
        }
    }

    return bayerImage;
}



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
    //image = convertToBayerPattern(dst);
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
    std::vector<Point2D_int>* longestPath;
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

void interpolate(uint8_t* frame, uint16_t x, uint16_t y, uint16_t width, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint8_t* pixel = frame + y * width + x;
    if (y & 1)
    {
        if (x & 1)
        {
            *r = *pixel;
            *g = (*(pixel - 1) + *(pixel + 1) + *(pixel + width) + *(pixel - width)) >> 2;
            *b = (*(pixel - width - 1) + *(pixel - width + 1) + *(pixel + width - 1) + *(pixel + width + 1)) >> 2;
        }
        else
        {
            *r = (*(pixel - 1) + *(pixel + 1)) >> 1;
            *g = *pixel;
            *b = (*(pixel - width) + *(pixel + width)) >> 1;
        }
    }
    else
    {
        if (x & 1)
        {
            *r = (*(pixel - width) + *(pixel + width)) >> 1;
            *g = *pixel;
            *b = (*(pixel - 1) + *(pixel + 1)) >> 1;
        }
        else
        {
            *r = (*(pixel - width - 1) + *(pixel - width + 1) + *(pixel + width - 1) + *(pixel + width + 1)) >> 2;
            *g = (*(pixel - 1) + *(pixel + 1) + *(pixel + width) + *(pixel - width)) >> 2;
            *b = *pixel;
        }
    }
}


void baiernToBitmatrix(uint8_t* frame, BitMatrix* _matrix, uint16_t height, uint16_t width, float black_treshold) {
    RGBcolor pixel;
    HSVcolor hsv;
    uint8_t luminosity;
    _matrix->clear();
    for (uint16_t row = 0; row < height; row++)
    {
        for (uint16_t col = 0; col < (width); col++) {

            luminosity = frame[(row * width) + col];

            if (luminosity <= (black_treshold * 255)) {
                _matrix->setBit(row, col);
            }
            else {
                _matrix->unsetBit(row, col);
            }
        }
    }

}


static void baiernToBitmatrixDownscale(BitMatrix* _dst, uint8_t* _src, uint16_t height, uint16_t width, size_t downscale_rate, float min_treshold) {
    if (downscale_rate == 1) {
        baiernToBitmatrix(_src, _dst, height, width, min_treshold);
        return;
    }

    size_t src_last_col, src_last_row;
    size_t n_settedbits;
    //size_t downscale_rate = 2;
    _dst->clear();
    src_last_col = width - downscale_rate;
    src_last_row = height - downscale_rate;

    for (size_t row = 0; row < src_last_row; row += downscale_rate)
    {
        for (size_t col = 0; col < src_last_col; col += downscale_rate)
        {
            n_settedbits = 0;
            for (size_t i = 0; i < downscale_rate; i++)
            {
                for (size_t j = 0; j < downscale_rate; j++)
                {
                    n_settedbits += _src[(width * (row + i)) + (col + j)];
                }
            }
            if (n_settedbits <= (size_t)(((downscale_rate * downscale_rate) * (min_treshold * 255)))) {
                _dst->setBit(row / downscale_rate, col / downscale_rate);
            }
        }
    }
}


std::vector<std::vector<Point2D_int>> gggg2_longest_path_baiern(Matrix<uint8_t>* baiern_image, float vector_approximation_epsilon) {
    BitMatrix image(baiern_image->getRows() / DOWNSCALE_FACTOR, baiern_image->getCols() / DOWNSCALE_FACTOR);
    BitMatrix body(image.getRows(), image.getColumns());
    BitMatrix temp(image.getRows(), image.getColumns());
    std::vector<Point2D_int>* longestPath;
    std::vector<Point2D_int> approxCurve;
    std::vector<std::vector<Point2D_int>> vectors;
    BitMatrixPosition pixelPosition;

    // Start time
    auto start = std::chrono::high_resolution_clock::now();
    //baiernToBitmatrix((uint8_t*)(baiern_image->data()), &image, image.getRows(), image.getColumns(), BLACK_TRERSHOLD);
    baiernToBitmatrixDownscale(&image, (uint8_t*)(baiern_image->data()), baiern_image->getRows(), baiern_image->getCols(), DOWNSCALE_FACTOR, BLACK_TRERSHOLD);
    BitMatrixSkeletonZS(&image, &temp);
    for (;;)
    {
        pixelPosition = image.getFirstSetPixel();
        if (!(pixelPosition.valid)) {
            break;
        }
        image.floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);

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




    cv::Mat dst;
    //cv::Size newSize(320, 200);
    int width = 208;
    int height = (int)(width * (float)(316.0 / 208.0));
    cv::Size newSize(height, width);
    // Resize the image
    cv::resize(original_img, dst, newSize, 0.0, 0.0, cv::INTER_LANCZOS4);

    cv::Mat baiern_img = convertToBayerPattern(dst);
    Matrix<uint8_t> temp_baiern_matrix = matToMatrix<uint8_t>(baiern_img);
    vectors = gggg2_longest_path_baiern(&temp_baiern_matrix, VECTOR_APPROXIMATION_EPSILON);
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




