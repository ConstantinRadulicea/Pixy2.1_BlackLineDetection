#include "approxPolyDP.h"


// Helper function to compute the perpendicular distance of a point from a line
float perpendicularDistance(Point2D_int* point, Point2D_int* lineStart, Point2D_int* lineEnd) {
    float dx = lineEnd->x - lineStart->x;
    float dy = lineEnd->y - lineStart->y;

    if (dx == 0 && dy == 0) {
        // The line is a single point
        return sqrtf(powf(point->x - lineStart->x, 2) + powf(point->y - lineStart->y, 2));
    }

    // Calculate the perpendicular distance using the formula
    return fabs(dy * point->x - dx * point->y + lineEnd->x * lineStart->y - lineEnd->y * lineStart->x) /
        sqrtf(dx * dx + dy * dy);
}

// Recursive function implementing the Ramer–Douglas–Peucker algorithm
void ramerDouglasPeucker(std::vector<Point2D_int>* points, float epsilon, std::vector<Point2D_int>* output) {
    size_t n_points = points->size();
    Point2D_int temp_point;
    if (n_points < 2) {
        if (n_points == 1) {
            output->push_back(points->at(0));
            return;
        }
        return; // Not enough points to simplify
    }

    // Find the point with the maximum distance from the line formed by the first and last points
    double maxDistance = 0;
    int index = -1;
    temp_point = points->at(0);

    for (size_t i = 1; i < n_points - 1; ++i) {
        double dist = perpendicularDistance(&(points->at(i)), &temp_point, &(points->at(n_points - 1)));
        if (dist > maxDistance) {
            maxDistance = dist;
            index = i;
        }
    }

    // If the max distance is greater than epsilon, recursively simplify the curve
    if (maxDistance > epsilon) {
        std::vector<Point2D_int> leftSubList(points->begin(), points->begin() + index + 1);
        std::vector<Point2D_int> rightSubList(points->begin() + index, points->end());

        std::vector<Point2D_int> leftResult, rightResult;
        ramerDouglasPeucker(&leftSubList, epsilon, &leftResult);
        ramerDouglasPeucker(&rightSubList, epsilon, &rightResult);

        // Combine the results from the left and right sub-lists
        output->insert(output->end(), leftResult.begin(), leftResult.end() - 1);
        output->insert(output->end(), rightResult.begin(), rightResult.end());
    }
    else {
        // If no point is farther than epsilon, the line is good enough
        output->push_back(points->at(0));
        output->push_back(points->at(points->size() - 1));
    }
}

// Wrapper function to simplify a contour
std::vector<Point2D_int> approxPolyDP(std::vector<Point2D_int>& contour, float epsilon) {
    std::vector<Point2D_int> simplifiedContour;
    ramerDouglasPeucker(&contour, epsilon, &simplifiedContour);
    return simplifiedContour;
}

