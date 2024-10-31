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
#define BLACK_TRERSHOLD 0.3f

//#define IMG_PATH "img/img1.png"
//#define IMG_PATH "img/black.png"
//#define IMG_PATH "img/test1.png"
//#define IMG_PATH "img/test2.png"
//#define IMG_PATH "img/test3.png"
//#define IMG_PATH "img/20241002_194857.jpg" // intersection 1

//#define IMG_PATH "img/20241002_194755.jpg" // straight with start lines
//#define IMG_PATH "img/20241002_194910.jpg" // intersection shiny
//#define IMG_PATH "img/20241002_194812.jpg" // curve 1
//#define IMG_PATH "img/20241002_194947.jpg" // curve 2
#define IMG_PATH "img/20241002_194842.jpg" // curve 3


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
            if (n_settedbits <= (size_t)(((downscale_rate * downscale_rate) * (min_treshold * 255.0f)))) {
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



cv::Mat TestFunction(Matrix<uint8_t>* baiern_image) {
    BitMatrix image(baiern_image->getRows() / DOWNSCALE_FACTOR, baiern_image->getCols() / DOWNSCALE_FACTOR);
    BitMatrix temp(image.getRows(), image.getColumns());
    cv::Mat res;
    int windowWidth = 400;  // Adjust this value to fit your screen
    int windowHeight = 320; // Adjust this value to fit your screen

    baiernToBitmatrixDownscale(&image, (uint8_t*)(baiern_image->data()), baiern_image->getRows(), baiern_image->getCols(), DOWNSCALE_FACTOR, BLACK_TRERSHOLD);
    res = bitMatrixToMat(image);
    cv::namedWindow("biern_downcaled", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("biern_downcaled", windowWidth, windowHeight);
    cv::imshow("biern_downcaled", res);
    
    
    BitMatrixSkeletonZS(&image, &temp);
    res = bitMatrixToMat(image);
    cv::namedWindow("skeleton", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("skeleton", windowWidth, windowHeight);
    cv::imshow("skeleton", res);

    return res;
}


void TestVectors() {
    std::vector<std::vector<Point2D_int>> vectors;
    cv::Mat original_img = cv::imread(IMG_PATH);


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


    std::vector<std::vector<cv::Point>> approxCurve;
    for (size_t i = 0; i < vectors.size(); i++) {
        std::vector<cv::Point> temp;
        approxCurve.push_back(temp);
        for (size_t j = 0; j < vectors[i].size(); j++) {
            approxCurve[i].push_back(cv::Point(vectors[i][j].x, vectors[i][j].y));
        }
    }


    cv::Mat result = cv::Mat::zeros(baiern_img.rows / DOWNSCALE_FACTOR, baiern_img.cols / DOWNSCALE_FACTOR, CV_8UC3);  // Create a blank canvas
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
    

    cv::namedWindow("baiern", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("baiern", windowWidth, windowHeight);
    cv::imshow("baiern", baiern_img);

    //cv::namedWindow("treshold", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    //cv::resizeWindow("treshold", windowWidth, windowHeight);
    //cv::imshow("treshold", image);

    cv::namedWindow("lines", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    cv::resizeWindow("lines", windowWidth, windowHeight);
    cv::imshow("lines", result);


    TestFunction(&temp_baiern_matrix);
    cv::waitKey(0);  // Wait for a key press before closing the window
}






void main() {
    TestVectors();
	//TestBitMatrix();
}




