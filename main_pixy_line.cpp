#include "pixy2_libs/host/libpixyusb2/include/libpixyusb2.h"
#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "approxPolyDP.h"
#include "Matrix.h"

#define CAM_RES2_WIDTH 316
#define CAM_RES2_HEIGHT 208

//#define CAM_RES2_WIDTH (316 * 4)
//#define CAM_RES2_HEIGHT (208 * 4)

#define DOWNSCALE_FACTOR 4
#define MIN_DOWNSCALE_FACTOR 1
#define DOWNSCALE_COLOR_TRESHOLD 0.2f
#define MIN_LINE_LENGTH 1
#define VECTOR_APPROXIMATION_EPSILON 4.0f / (float)DOWNSCALE_FACTOR
#define BLACK_TRERSHOLD 0.25f

Pixy2 pixy;


std::vector<std::vector<Point2D_int>> gggg2_longest_path_baiern_raw(uint8_t* baiern_image, float vector_approximation_epsilon, size_t _downscale_factor) {
    static size_t frame_height = CAM_RES2_HEIGHT;
    static size_t frame_width = CAM_RES2_WIDTH;
    static size_t downscale_factor = MIN_DOWNSCALE_FACTOR;
    static size_t scaled_down_frame_height = CAM_RES2_HEIGHT;
    static size_t scaled_down_frame_width = CAM_RES2_WIDTH;

    static BitMatrix image(frame_height, frame_width);
    static BitMatrix body(frame_height, frame_width);
    static BitMatrix temp(frame_height, frame_width);
    static std::vector<Point2D_int>* longestPath;
    static std::vector<Point2D_int> approxCurve;
    static std::vector<std::vector<Point2D_int>> vectors;
    static BitMatrixPosition pixelPosition;

    vectors.clear();

    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    if (_downscale_factor != downscale_factor)
    {
        if (_downscale_factor < MIN_DOWNSCALE_FACTOR) {
            _downscale_factor = MIN_DOWNSCALE_FACTOR;
        }
        downscale_factor = _downscale_factor;
        scaled_down_frame_width = frame_width / downscale_factor;
        scaled_down_frame_height = frame_height / downscale_factor;

        image.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
        body.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
        temp.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
    }

    baiernToBitmatrixDownscale_minPooling(&image, baiern_image, CAM_RES2_HEIGHT, CAM_RES2_WIDTH, downscale_factor, BLACK_TRERSHOLD, DOWNSCALE_COLOR_TRESHOLD);
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

        longestPath = BitMatrix::findLongestPath(&body, &temp);
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




int main44()
{
    int  Result;
    uint8_t* bayerFrame;
    std::vector<std::vector<Point2D_int>> vectors;

    printf("=============================================================\n");
    printf("= PIXY2 Get Raw Frame Example                               =\n");
    printf("=============================================================\n");

    printf("Connecting to Pixy2...");

    // Initialize Pixy2 Connection //
    {
        Result = pixy.init();

        if (Result < 0)
        {
            printf("Error\n");
            printf("pixy.init() returned %d\n", Result);
            return Result;
        }
        Result = pixy.changeProg("Video");

        if (Result < 0)
        {
            printf("Error\n");
            printf("pixy.changeProg() returned %d\n", Result);
            return Result;
        }
        pixy.setLamp(1, 1);

        printf("Success\n");
    }

    // Get Pixy2 Version information //
    {
        Result = pixy.getVersion();

        if (Result < 0)
        {
            printf("pixy.getVersion() returned %d\n", Result);
            return Result;
        }

        pixy.version->print();
    }



    while (1) {

        // need to call stop() befroe calling getRawFrame().
// Note, you can call getRawFrame multiple times after calling stop().
// That is, you don't need to call stop() each time.
        auto start = std::chrono::high_resolution_clock::now();

        pixy.m_link.stop();
        // grab raw frame, BGGR Bayer format, 1 byte per pixel
        pixy.m_link.getRawFrame(&bayerFrame);
        
        // Call resume() to resume the current program, otherwise Pixy will be left
// in "paused" state.  
        pixy.m_link.resume();
        std::cout << "FPS: " << (int)pixy.getFPS() << std::endl;

        vectors = gggg2_longest_path_baiern_raw(bayerFrame, VECTOR_APPROXIMATION_EPSILON, DOWNSCALE_FACTOR);



        // End time
        auto end = std::chrono::high_resolution_clock::now();
        // Calculate the duration
        std::chrono::duration<double> duration = end - start;
        // Output the result in seconds
        std::cout << "Function execution time: " << duration.count() << " seconds" << std::endl << std::endl;



    //    std::vector<std::vector<cv::Point>> approxCurve;
    //    for (size_t i = 0; i < vectors.size(); i++) {
    //        std::vector<cv::Point> temp;
    //        approxCurve.push_back(temp);
    //        for (size_t j = 0; j < vectors[i].size(); j++) {
    //            approxCurve[i].push_back(cv::Point(vectors[i][j].x, vectors[i][j].y));
    //        }
    //    }


    //    cv::Mat result = cv::Mat::zeros(CAM_RES2_HEIGHT / DOWNSCALE_FACTOR, CAM_RES2_WIDTH / DOWNSCALE_FACTOR, CV_8UC3);  // Create a blank canvas
    //    cv::RNG rng(time(0));
    //    for (size_t i = 0; i < approxCurve.size(); i++)
    //    {
    //        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    //        // Draw the simplified skeleton using the approximate curve
    //        for (size_t j = 0; j < approxCurve[i].size() - 1; ++j) {
    //            cv::line(result, approxCurve[i][j], approxCurve[i][j + 1], color, 1);
    //        }
    //    }


    //    // Display the result
    //// Create a window
    //// Resize the window to a specific size (adjust width and height as needed)
    //    int windowWidth = 400;  // Adjust this value to fit your screen
    //    int windowHeight = 320; // Adjust this value to fit your screen

    //    cv::namedWindow("lines", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
    //    cv::resizeWindow("lines", windowWidth, windowHeight);
    //    cv::imshow("lines", result);

        //cv::waitKey(1);  // Wait for a key press before closing the window
    }
}

