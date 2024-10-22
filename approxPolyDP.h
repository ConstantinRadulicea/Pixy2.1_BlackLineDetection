#ifndef __APPROXPOLYDP_H__
#define __APPROXPOLYDP_H__
#include "geometry2D.h"
#include <math.h>
#include <vector>

// Recursive function implementing the Ramer–Douglas–Peucker algorithm
void ramerDouglasPeucker(std::vector<Point2D_int>* points, float epsilon, std::vector<Point2D_int>* output);

// Wrapper function to simplify a contour
std::vector<Point2D_int> approxPolyDP(std::vector<Point2D_int>& contour, float epsilon);



#endif // !__APPROXPOLYDP_H__
