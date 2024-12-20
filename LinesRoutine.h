#ifndef __LINESROUTINE_H__
#define __LINESROUTINE_H__

#include <vector>
#include "BitMatrix.h"
#include "Matrix.h"
#include "BitMatrixSkeleton.h"
#include "approxPolyDP.h"


#define CAM_RES2_WIDTH 316
#define CAM_RES2_HEIGHT 208

#define DOWNSCALE_FACTOR 1
#define MIN_DOWNSCALE_FACTOR 1
#define DOWNSCALE_COLOR_TRESHOLD 0.2f
#define MIN_LINE_LENGTH 20
#define VECTOR_APPROXIMATION_EPSILON 1.0f / (float)DOWNSCALE_FACTOR
#define BLACK_TRERSHOLD 0.25f


#define LINESROUTINE_DEBUG

#ifdef LINESROUTINE_DEBUG
#include <iostream>
#include <chrono>
#endif // LINESROUTINE_DEBUG

std::vector<std::vector<Point2D_int>> LinesRoutine(uint8_t* baiern_image, float vector_approximation_epsilon, size_t _downscale_factor);

#endif // !__LINESROUTINE_H__
