/*
* Copyright 2023 Constantin Dumitru Petre RĂDULICEA
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __GEOMETRY2D_H__
#define __GEOMETRY2D_H__

#include <math.h>
#include <float.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


#define M_PI       3.14159265358979323846f   // pi
#define M_PI_2     1.57079632679489661923f   // pi/2
#define DEG_TO_RAD 0.01745329251994329576f
#define RAD_TO_DEG 57.2957795130823208767f
#define G_CONSTANT 9.80665f

#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define HzToSec(hz) (1.0/(hz))
#define MilliToMicro(val) ((val)*1000.0)
#define MicroToMilli(val) ((val)/1000.0)
#define MicroToUnit(val) ((val)/1000000.0)
#define UnitToMicro(val) ((val)*1000000.0)


#define INCONSISTENT_ECUATION_SYSTEM 1
#define CONSISTENT_ECUATION_SYSTEM 2

typedef struct LineMQ {
	float m;
	float q;
}LineMQ;

typedef struct ParabolaABC {
	float A;
	float B;
	float C;
}ParabolaABC;

typedef struct LineABC {
	float Ax;
	float By;
	float C;
}LineABC;

typedef struct Point2D {
	float x;
	float y;
}Point2D;

typedef struct Point2D_int {
	int x;
	int y;
}Point2D_int;

typedef struct LineSegment {
	Point2D A;
	Point2D B;
}LineSegment;

typedef struct LineSegmentsDistancePoints {
	LineSegment min;
	LineSegment max;
}LineSegmentsDistancePoints;

typedef struct IntersectionPoints2D_2
{
	Point2D point1;
	Point2D point2;
	int numPoints;
	int sameEquation;
}IntersectionPoints2D_2;

typedef struct IntersectionLines {
	Point2D point;
	int info; // 0: one intersection, 1: lines are parallel, 2: the lines are equal
}IntersectionLines;

float NormalizePiToNegPi(float angle);

LineABC xAxisABC();

LineABC yAxisABC();

int isValidLineABC(LineABC line);

// result = A - B;
LineABC LineAbcSubtraction(LineABC A, LineABC B);

// polynomial_coefficients[0] = x^2
// polynomial_coefficients[1] = x
// polynomial_coefficients[3] = 1
// polynomial_degree = 2
Point2D polyval(float* polynomial_coefficients, int polynomial_degree, float x);

int floatCmp(float num1, float num2);

int gaussianElimination3(float A[3][3 + 1], float x[3], int n);
int gaussianElimination2(float A[2][2 + 1], float x[2], int n);

ParabolaABC points2parabola_3(Point2D point1, Point2D point2, Point2D point3);

int isValidParabola(ParabolaABC parabola);

Point2D parabolaVertex(ParabolaABC parabola);

Point2D midPoint(Point2D point1, Point2D point2);

LineMQ perpendicularToLinePassingThroughPointMQ(LineMQ line, Point2D point);


Point2D mirrorImageABC(LineABC line, Point2D point);

LineABC normalizeLineABC2MQ(LineABC line);

float distanceBwParallelLinesABC(LineABC line1, LineABC line2);

int arePerpenticularABC(LineABC line1, LineABC line2);

int areParallelABC(LineABC line1, LineABC line2);
/*
 * side:
 *		1: new line on right or bottom side
 *	   0: new line on left or upper side
*/
LineABC parallelLineAtDistanceABC(LineABC line, float distance, int side);
int isLineParallelToXaxisABC(LineABC line);

int isLineParallelToYaxisABC(LineABC line);

float angleBetweenLinesMQ(LineMQ line1, LineMQ line2);

// https://www.math-only-math.com/equations-of-the-bisectors-of-the-angles-between-two-straight-lines.html
//acutangle is the bisector when the lines are parallel
void bisectorsOfTwoLinesABC(LineABC line1, LineABC line2, LineABC* acuteAngle, LineABC* ottuseAngle);
LineMQ points2lineMQ(Point2D point1, Point2D point2);

LineABC lineMQ2ABC(LineMQ line);

LineMQ lineABC2MQ(LineABC line);

LineABC perpendicularToLinePassingThroughPointABC(LineABC line, Point2D point);

LineABC rotateLineAroundPoint(LineABC line, Point2D point, float angle);

Point2D rotatePointAroundPoint(Point2D point, Point2D center, float angle);

LineSegment rotateLineSegmentAroundPoint(LineSegment lineSegment, Point2D center, float angle);

float angleBetweenLinesABC(LineABC line1, LineABC line2);

LineABC points2lineABC(Point2D point1, Point2D point2);

float euclidianDistance(Point2D point1, Point2D point2);
float distance2lineMQ(Point2D point, LineMQ line);

float distance2lineABC(Point2D point, LineABC lineAbc);
int isPointOnLineABC(Point2D point, LineABC lineAbc);

IntersectionPoints2D_2 intersectionLineCircleMQ(Point2D circleCenter, float circleRadius, LineMQ line);

IntersectionPoints2D_2 intersectionLineCircleABC(Point2D circleCenter, float circleRadius, LineABC lineAbc);
IntersectionLines intersectionLinesABC(LineABC line1, LineABC line2);

Point2D circleAngleToPoint2D(Point2D circleCenter, float circleRadius, float angleRad);



float triangleAngleA(float AC, float CB, float BA);

float distanceBwLinesABC(LineABC line1, LineABC line2, Point2D pointOnLine);
Point2D projectPointOnLineABC(Point2D point, LineABC line);

int isPointOnSegment(LineSegment segment, Point2D point);

LineSegmentsDistancePoints distancePointsBwSegments(LineSegment segment1, LineSegment segment2);

LineABC lineSegmentToLineABC(LineSegment segment);


float minDistanceLineSegmentToLine(LineSegment vectorSegment, LineABC line);

float maxDistanceLineSegmentToLine(LineSegment vectorSegment, LineABC line);

IntersectionPoints2D_2 intersectionBwCircles(Point2D circleCenter_1, float circleRadius_1, Point2D circleCenter_2, float circleRadius_2);

float circlePoint2DToAngle(Point2D circleCenter, Point2D point);

float angleBw3Points2D(Point2D origin, Point2D point_b, Point2D point_c);

float NormalizeZeroToPi(float angle);

int isNumber(const char* str, size_t str_length);

Point2D closestPointToSegment(Point2D point, LineSegment seg);
#endif // !__GEOMETRY2D_H__
