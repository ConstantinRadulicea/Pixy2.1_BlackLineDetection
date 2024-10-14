#ifndef __APPROXPOLYDP_H__
#define __APPROXPOLYDP_H__
#include "geometry2D.h"
#include <math.h>
#include <vector>


// Helper function to compute the perpendicular distance of a point from a line
static double perpendicularDistance(Point2D& point, Point2D& lineStart, Point2D& lineEnd) {
    double dx = lineEnd.x - lineStart.x;
    double dy = lineEnd.y - lineStart.y;
    
    if (dx == 0 && dy == 0) {
        // The line is a single point
        return sqrtf(powf(point.x - lineStart.x, 2) + powf(point.y - lineStart.y, 2));
    }
   
    // Calculate the perpendicular distance using the formula
    return fabs(dy * point.x - dx * point.y + lineEnd.x * lineStart.y - lineEnd.y * lineStart.x) /
        sqrtf(dx * dx + dy * dy);
}

// Recursive function implementing the Ramer–Douglas–Peucker algorithm
static void ramerDouglasPeucker(std::vector<Point2D>& points, double epsilon, std::vector<Point2D>& output) {
    if (points.size() < 2) {
        return; // Not enough points to simplify
    }

    // Find the point with the maximum distance from the line formed by the first and last points
    double maxDistance = 0;
    int index = -1;

    for (size_t i = 1; i < points.size() - 1; ++i) {
        double dist = perpendicularDistance(points[i], points[0], points[points.size() - 1]);
        if (dist > maxDistance) {
            maxDistance = dist;
            index = i;
        }
    }

    // If the max distance is greater than epsilon, recursively simplify the curve
    if (maxDistance > epsilon) {
        std::vector<Point2D> leftSubList(points.begin(), points.begin() + index + 1);
        std::vector<Point2D> rightSubList(points.begin() + index, points.end());

        std::vector<Point2D> leftResult, rightResult;
        ramerDouglasPeucker(leftSubList, epsilon, leftResult);
        ramerDouglasPeucker(rightSubList, epsilon, rightResult);

        // Combine the results from the left and right sub-lists
        output.insert(output.end(), leftResult.begin(), leftResult.end() - 1);
        output.insert(output.end(), rightResult.begin(), rightResult.end());
    }
    else {
        // If no point is farther than epsilon, the line is good enough
        output.push_back(points[0]);
        output.push_back(points[points.size() - 1]);
    }
}

// Wrapper function to simplify a contour
static std::vector<Point2D> approxPolyDP(std::vector<Point2D>& contour, double epsilon) {
    std::vector<Point2D> simplifiedContour;
    ramerDouglasPeucker(contour, epsilon, simplifiedContour);
    return simplifiedContour;
}



#endif // !__APPROXPOLYDP_H__
