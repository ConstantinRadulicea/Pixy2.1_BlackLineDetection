#ifndef __APPROXPOLYDP_H__
#define __APPROXPOLYDP_H__
#include "geometry2D.h"
#include <math.h>
#include <vector>

// Recursive function implementing the Ramer–Douglas–Peucker algorithm
void ramerDouglasPeucker(std::vector<Point2D>* points, float epsilon, std::vector<Point2D>* output);

// Wrapper function to simplify a contour
std::vector<Point2D> approxPolyDP(std::vector<Point2D>& contour, float epsilon);



#endif // !__APPROXPOLYDP_H__
