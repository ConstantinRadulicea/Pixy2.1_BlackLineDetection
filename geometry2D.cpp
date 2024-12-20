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

#include "geometry2D.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

LineABC xAxisABC() {
	LineABC line;
	line.Ax = 0.0f;
	line.By = 1.0f;
	line.C = 0.0f;
	return line;
}

LineABC yAxisABC() {
	LineABC line;
	line.Ax = 1.0f;
	line.By = 0.0f;
	line.C = 0.0f;
	return line;
}

int isValidLineABC(LineABC line) {
	if (floatCmp(line.Ax, 0.0f) == 0 && floatCmp(line.By, 0.0f) == 0) {
		return 0;
	}
	return 1;
}

// result = A - B;
LineABC LineAbcSubtraction(LineABC A, LineABC B) {
	LineABC res;
	res.Ax = A.Ax - B.Ax;
	res.By = A.By - B.By;
	res.C = A.C - B.C;
	return res;
}

// polynomial_coefficients[0] = x^2
// polynomial_coefficients[1] = x
// polynomial_coefficients[3] = 1
// polynomial_degree = 2
Point2D polyval(float* polynomial_coefficients, int polynomial_degree, float x) {
	float y = 0.0f;
	float x_power = 1.0f;
	Point2D result;

	for (int i = polynomial_degree; i >= 0; i--) {
		y += polynomial_coefficients[i] * x_power;
		x_power = x_power * x;
	}
	result.x = x;
	result.y = y;
	return result;
}

int floatCmp(float num1, float num2) {
	if (fabs(num1 - num2) < FLT_EPSILON) {
		return 0;
	}
	else if (num1 > num2) {
		return 1;
	}
	return -1;
}

int arePoints2DEqual(Point2D point1, Point2D point2) {
	if (floatCmp(point1.x, point2.x) == 0 && floatCmp(point1.y, point2.y) == 0) {
		return 1;
	}
	return 0;
}

int gaussianElimination3(float A[3][3 + 1], float x[3], int n) {
	int j, i, k;
	int pivot_row;
	float factor;
	float temp;
	float sum;

	// partial_pivot
	for (i = 0; i < n; i++) {
		pivot_row = i;
		for (j = i + 1; j < n; j++) {
			if (fabsf(A[j][i]) > fabsf(A[pivot_row][i])) {
				pivot_row = j;
			}
		}
		if (pivot_row != i) {
			for (j = i; j <= n; j++) {
				temp = A[i][j];
				A[i][j] = A[pivot_row][j];
				A[pivot_row][j] = temp;
			}
		}
		for (j = i + 1; j < n; j++) {
			factor = A[j][i] / A[i][i];
			for (k = i; k <= n; k++) {
				A[j][k] -= factor * A[i][k];
			}
		}
	}


	for (i = 0; i < n; i++)
	{
		sum = 0;
		for (j = 0; j < n; j++)
		{
			sum += A[i][j];
		}
		if ((sum == 0.0f) && (A[i][n] != 0.0f)) {
			memset(x, 0, sizeof(float) * n);
			return INCONSISTENT_ECUATION_SYSTEM;
		}
	}

	// back_substitute
	for (i = n - 1; i >= 0; i--) {
		sum = 0;
		for (j = i + 1; j < n; j++) {
			sum += A[i][j] * x[j];
		}
		x[i] = (A[i][n] - sum) / A[i][i];
	}
	return CONSISTENT_ECUATION_SYSTEM;
}
int gaussianElimination2(float A[2][2 + 1], float x[2], int n) {
	int j, i, k;
	int pivot_row;
	float factor;
	float temp;
	float sum;

	// partial_pivot
	for (i = 0; i < n; i++) {
		pivot_row = i;
		for (j = i + 1; j < n; j++) {
			if (fabsf(A[j][i]) > fabsf(A[pivot_row][i])) {
				pivot_row = j;
			}
		}
		if (pivot_row != i) {
			for (j = i; j <= n; j++) {
				temp = A[i][j];
				A[i][j] = A[pivot_row][j];
				A[pivot_row][j] = temp;
			}
		}
		for (j = i + 1; j < n; j++) {
			factor = A[j][i] / A[i][i];
			for (k = i; k <= n; k++) {
				A[j][k] -= factor * A[i][k];
			}
		}
	}


	for (i = 0; i < n; i++)
	{
		sum = 0;
		for (j = 0; j < n; j++)
		{
			sum += A[i][j];
		}
		if ((sum == 0.0f) && (A[i][n] != 0.0f)) {
			memset(x, 0, sizeof(float) * n);
			return INCONSISTENT_ECUATION_SYSTEM;
		}
	}

	// back_substitute
	for (i = n - 1; i >= 0; i--) {
		sum = 0;
		for (j = i + 1; j < n; j++) {
			sum += A[i][j] * x[j];
		}
		x[i] = (A[i][n] - sum) / A[i][i];
	}
	return CONSISTENT_ECUATION_SYSTEM;
}

ParabolaABC points2parabola_3(Point2D point1, Point2D point2, Point2D point3) {
	ParabolaABC resultParabola;
	int res;
	float A[3][3 + 1] = { {point1.x * point1.x, point1.x, 1.0f, point1.y},
						  {point2.x * point2.x, point2.x, 1.0f, point2.y},
						  {point3.x * point3.x, point3.x, 1.0f, point3.y}
	};
	float x[3];

	res = gaussianElimination3(A, x, 3);
	if (res != CONSISTENT_ECUATION_SYSTEM) {
		memset(&resultParabola, 0, sizeof(ParabolaABC));
		return resultParabola;
	}
	resultParabola.A = x[0];
	resultParabola.B = x[1];
	resultParabola.C = x[2];

	return resultParabola;
}

int isValidParabola(ParabolaABC parabola) {
	if ((floatCmp(parabola.A, parabola.B) == 0) && (floatCmp(parabola.A, parabola.C) == 0) && (floatCmp(parabola.A, 0.0f) == 0)) {
		return 0;
	}
	else {
		return 1;
	}
}

Point2D parabolaVertex(ParabolaABC parabola) {
	Point2D vertex;
	float delta;
	// (h, k) = (-b/2a, -D/4a)

	delta = (parabola.B * parabola.B) - 4.0f * (parabola.A * parabola.C);
	vertex.x = (-parabola.B) / (2.0f * parabola.A);
	vertex.y = (-delta) / (4.0f * parabola.A);
	return vertex;
}

Point2D midPoint(Point2D point1, Point2D point2) {
	Point2D midpoint_;
	midpoint_.x = (point1.x + point2.x) / 2.0f;
	midpoint_.y = (point1.y + point2.y) / 2.0f;
	return midpoint_;
}

LineMQ perpendicularToLinePassingThroughPointMQ(LineMQ line, Point2D point) {
	LineMQ perpendicularLine;

	perpendicularLine.m = -(1.0f / line.m);
	perpendicularLine.q = (point.x / line.m) + point.y;

	return perpendicularLine;
}


Point2D mirrorImageABC(LineABC line, Point2D point)
{
	Point2D mirrorPoint_;
	float temp;

	temp = -2.0f * (line.Ax * point.x + line.By * point.y + line.C) / (line.Ax * line.Ax + line.By * line.By);
	mirrorPoint_.x = temp * line.Ax + point.x;
	mirrorPoint_.y = temp * line.By + point.y;

	return mirrorPoint_;
}

LineABC normalizeLineABC2MQ(LineABC line) {
	if (floatCmp(line.By, 1.0f) != 0 && floatCmp(line.By, 0.0f) != 0) {
		line.C = line.C / line.By;
		line.Ax = line.Ax / line.By;
		line.By = 1.0f;
		return line;
	}
	if (floatCmp(line.By, 0.0f) == 0 && floatCmp(line.Ax, 0.0f) != 0)
	{
		line.By = 0.0f;
		line.C = line.C / line.Ax;
		line.Ax = 1.0f;
		return line;
	}
	return line;
}

float distanceBwParallelLinesABC(LineABC line1, LineABC line2) {
	float distance;
	line2 = normalizeLineABC2MQ(line2);
	line1 = normalizeLineABC2MQ(line1);
	distance = fabsf(line2.C - line1.C) / sqrtf((line1.Ax * line1.Ax) + (line1.By * line1.By));
	//distance = fabsf((-line2.C) - (-line1.C)) / sqrtf(1.0f + ((-line1.Ax) * (-line1.Ax)));
	return distance;
}


int arePerpenticularABC(LineABC line1, LineABC line2) {
	line2 = normalizeLineABC2MQ(line2);
	line1 = normalizeLineABC2MQ(line1);

	if (floatCmp((-line1.Ax) * (-line2.Ax), -1.0f) == 0.0f) {
		return 1;
	}
	else {
		return 0;
	}
}

int areParallelABC(LineABC line1, LineABC line2) {
	line2 = normalizeLineABC2MQ(line2);
	line1 = normalizeLineABC2MQ(line1);

	if (floatCmp(line1.Ax, line2.Ax) == 0.0f && floatCmp(line1.By, line2.By) == 0.0f) {
		return 1;
	}
	else {
		return 0;
	}
}
/*
 * side:
 *		1: new line on right or bottom side
 *	   0: new line on left or upper side
*/
LineABC parallelLineAtDistanceABC(LineABC line, float distance, int side) {
	LineABC parallelLine;
	float abs_q1_minus_q2, newQ;

	newQ = 0.0f;

	line = normalizeLineABC2MQ(line);

	parallelLine = line;
	abs_q1_minus_q2 = distance * sqrtf(1.0f + ((-line.Ax) * (-line.Ax)));
	if (side > 0)
	{
		if (floatCmp(line.By, 0.0f) == 0)	//		|.
		{
			newQ = (-line.C) + distance;
		}
		else if (floatCmp((-line.Ax), 0.0f) == 0) //		-.
		{
			newQ = (-line.C) - distance;
		}
		else if ((-line.Ax) > 0.0f) //			/ .		
		{
			newQ = (-line.C) - abs_q1_minus_q2;
		}
		else if ((-line.Ax) < 0.0f) //		\ .		
		{
			newQ = (-line.C) + abs_q1_minus_q2;
		}
	}
	else
	{
		if (floatCmp(line.By, 0.0f) == 0)	//		|.
		{
			newQ = (-line.C) - distance;
		}
		else if (floatCmp((-line.Ax), 0.0f) == 0) //		-.
		{
			newQ = (-line.C) + distance;
		}
		else if ((-line.Ax) > 0.0f) //			/ .		
		{
			newQ = (-line.C) + abs_q1_minus_q2;
		}
		else if ((-line.Ax) < 0.0f) //		\ .		
		{
			newQ = (-line.C) - abs_q1_minus_q2;
		}
	}
	parallelLine.C = -newQ;
	return parallelLine;
}

int isLineParallelToXaxisABC(LineABC line) {

	if (floatCmp(line.Ax, 0.0f) == 0 && floatCmp(line.By, 0.0f) != 0)
	{
		return 1;
	}
	else {
		return 0;
	}
}

int isLineParallelToYaxisABC(LineABC line) {
	if (floatCmp(line.By, 0.0f) == 0 && floatCmp(line.Ax, 0.0f) != 0)
	{
		return 1;
	}
	else {
		return 0;
	}
}

float angleBetweenLinesMQ(LineMQ line1, LineMQ line2) {
	float angle;
	if (floatCmp((line1.m * line2.m), -1.0f) == 0) {
		return M_PI_2;
	}

	angle = atanf(fabsf((line1.m - line2.m)) / (1.0f + (line1.m * line2.m)));
	return angle;
}

// https://www.math-only-math.com/equations-of-the-bisectors-of-the-angles-between-two-straight-lines.html
//acutangle is the bisector when the lines are parallel
void bisectorsOfTwoLinesABC(LineABC line1, LineABC line2, LineABC* acuteAngle, LineABC* ottuseAngle) {
	float a1, a2, b1, b2, c1, c2;
	float aa1, aa2, bb1, bb2, cc1, cc2;
	float leftDenominator, rightDenominator;
	float gg;

	a1 = line1.Ax;
	b1 = line1.By;
	c1 = line1.C;
	a2 = line2.Ax;
	b2 = line2.By;
	c2 = line2.C;


	if (floatCmp(c1, 0.0f) < 0) {
		a1 = -a1;
		b1 = -b1;
		c1 = -c1;
	}
	if (floatCmp(c2, 0.0f) < 0) {
		a2 = -a2;
		b2 = -b2;
		c2 = -c2;
	}

	leftDenominator = sqrtf((a1 * a1) + (b1 * b1));
	rightDenominator = sqrtf((a2 * a2) + (b2 * b2));

	// +
	aa1 = (rightDenominator * a1) - (leftDenominator * a2);
	bb1 = (rightDenominator * b1) - (leftDenominator * b2);
	cc1 = (rightDenominator * c1) - (leftDenominator * c2);

	// -
	aa2 = (rightDenominator * a1) + (leftDenominator * a2);
	bb2 = (rightDenominator * b1) + (leftDenominator * b2);
	cc2 = (rightDenominator * c1) + (leftDenominator * c2);

	gg = (a1 * a2) + (b1 * b2);

	if (floatCmp(gg, 0.0f) >= 0)
	{
		if (ottuseAngle)
		{
			ottuseAngle->Ax = aa1;
			ottuseAngle->By = bb1;
			ottuseAngle->C = cc1;
		}
		if (acuteAngle)
		{
			acuteAngle->Ax = aa2;
			acuteAngle->By = bb2;
			acuteAngle->C = cc2;
		}
	}
	else if (floatCmp(gg, 0.0f) < 0) {
		if (acuteAngle)
		{
			acuteAngle->Ax = aa1;
			acuteAngle->By = bb1;
			acuteAngle->C = cc1;
		}
		if (ottuseAngle)
		{
			ottuseAngle->Ax = aa2;
			ottuseAngle->By = bb2;
			ottuseAngle->C = cc2;
		}
	}
	if (ottuseAngle) {
		*ottuseAngle = normalizeLineABC2MQ(*ottuseAngle);
	}
	if (acuteAngle) {
		*acuteAngle = normalizeLineABC2MQ(*acuteAngle);
	}
}

LineMQ points2lineMQ(Point2D point1, Point2D point2) {
	LineMQ line;
	line.m = (point1.y - point2.y) / (point1.x - point2.x);
	line.q = (line.m * (-point1.x)) + point1.y;
	return line;
}

LineABC lineMQ2ABC(LineMQ line) {
	LineABC lineAbc;
	lineAbc.Ax = -line.m;
	lineAbc.By = 1.0f;
	lineAbc.C = -line.q;
	return lineAbc;
}

LineMQ lineABC2MQ(LineABC line) {
	LineMQ lineMq;
	line = normalizeLineABC2MQ(line);
	lineMq.m = -line.Ax;
	lineMq.q = -line.C;
	return lineMq;
}

LineABC perpendicularToLinePassingThroughPointABC(LineABC line, Point2D point) {
	LineABC perpendicularLine;
	LineMQ lineMq;

	line = normalizeLineABC2MQ(line);

	if (isLineParallelToYaxisABC(line)) {
		perpendicularLine = xAxisABC();
		perpendicularLine.C = -point.y;
	}
	else if (isLineParallelToXaxisABC(line)) {
		perpendicularLine = yAxisABC();
		perpendicularLine.C = -point.x;
	}
	else
	{
		lineMq = lineABC2MQ(line);
		lineMq = perpendicularToLinePassingThroughPointMQ(lineMq, point);
		perpendicularLine = lineMQ2ABC(lineMq);
	}


	return perpendicularLine;
}

LineABC rotateLineAroundPoint(LineABC line, Point2D point, float angle) {
	// Calculate the cosine and sine of the angle
	float cosTheta = cos(angle);
	float sinTheta = sin(angle);

	// Rotate the A and B components
	float newA = line.Ax * cosTheta - line.By * sinTheta;
	float newB = line.Ax * sinTheta + line.By * cosTheta;

	// Adjust C for rotation around the point (x0, y0)
	//float newC = line.C - (line.Ax * point.x + line.By * point.y);
	//newC = newC + (newA * point.x + newB * point.y);

	// Step 2: Correct C to keep the line's position relative to the rotation point
	float rotatedC = line.C + point.x * (line.Ax - newA) + point.y * (line.By - newB);

	// Return the rotated line
	LineABC rotatedLine;
	rotatedLine.Ax = newA;
	rotatedLine.By = newB;
	rotatedLine.C = rotatedC;

	return rotatedLine;
}


Point2D rotatePointAroundPoint(Point2D point, Point2D center, float angle) {
	// Translate the point to the origin (relative to the center point)
	float translatedX = point.x - center.x;
	float translatedY = point.y - center.y;

	// Calculate the rotated coordinates
	float cosTheta = cosf(angle);
	float sinTheta = sinf(angle);
	float rotatedX = translatedX * cosTheta - translatedY * sinTheta;
	float rotatedY = translatedX * sinTheta + translatedY * cosTheta;

	// Translate the point back to the original center
	Point2D rotatedPoint;
	rotatedPoint.x = rotatedX + center.x;
	rotatedPoint.y = rotatedY + center.y;

	return rotatedPoint;
}

LineSegment rotateLineSegmentAroundPoint(LineSegment lineSegment, Point2D center, float angle) {
	LineSegment rotatedLineSegment;
	rotatedLineSegment.A = rotatePointAroundPoint(lineSegment.A, center, angle);
	rotatedLineSegment.B = rotatePointAroundPoint(lineSegment.B, center, angle);
	return rotatedLineSegment;
}

float angleBetweenLinesABC(LineABC line1, LineABC line2) {
	float angle;
	int are_parallel;
	int are_perpendicular;
	LineMQ line1Mq, line2Mq;

	are_parallel = areParallelABC(line1, line2);
	if (are_parallel) {
		return 0.0f;
	}
	are_perpendicular = arePerpenticularABC(line1, line2);
	if (are_perpendicular) {
		return M_PI_2;
	}

	if (isLineParallelToYaxisABC(line1))
	{
		line1Mq = lineABC2MQ(line2);
		line2Mq.m = 0;
		line2Mq.q = 0;
		angle = M_PI_2 - angleBetweenLinesMQ(line1Mq, line2Mq);
	}
	else if (isLineParallelToYaxisABC(line2)) {
		line1Mq = lineABC2MQ(line1);
		line2Mq.m = 0;
		line2Mq.q = 0;
		angle = M_PI_2 - angleBetweenLinesMQ(line1Mq, line2Mq);
	}
	else {
		line1Mq = lineABC2MQ(line1);
		line2Mq = lineABC2MQ(line2);
		angle = angleBetweenLinesMQ(line1Mq, line2Mq);
	}

	return angle;
}

LineABC points2lineABC(Point2D point1, Point2D point2) {
	LineMQ lineMq;
	LineABC lineAbc;

	if (arePoints2DEqual(point1, point2)) {
		return LineABC{ 0.0f, 0.0f, 0.0f };
	}


	if (floatCmp(point1.x, point2.x) == 0) { // perpendicular to y axis
		lineAbc = yAxisABC();
		lineAbc.C = -point1.x;
		return lineAbc;
	}
	lineMq = points2lineMQ(point1, point2);
	lineAbc = lineMQ2ABC(lineMq);
	return lineAbc;
}

float euclidianDistance(Point2D point1, Point2D point2) {
	float distance;
	distance = sqrtf(((point2.x - point1.x) * (point2.x - point1.x)) + ((point2.y - point1.y) * (point2.y - point1.y)));
	return distance;
}

float distance2lineMQ(Point2D point, LineMQ line) {
	float distance;
	distance = fabsf((line.m * point.x) + (-1.0f * point.y) + line.q) / sqrtf((line.m * line.m) + 1.0f);
	return distance;
}

float distance2lineABC(Point2D point, LineABC lineAbc) {
	float distance;
	LineMQ line;
	Point2D point2Temp;

	if (floatCmp(lineAbc.By, 0.0f) == 0)
	{
		lineAbc = normalizeLineABC2MQ(lineAbc);
		point2Temp.y = point.y;
		point2Temp.x = (-lineAbc.C) / lineAbc.Ax;
		return euclidianDistance(point, point2Temp);
	}
	line = lineABC2MQ(lineAbc);
	distance = distance2lineMQ(point, line);
	return distance;
}

int isPointOnLineABC(Point2D point, LineABC lineAbc) {
	float result;
	result = (lineAbc.Ax * point.x) + (lineAbc.By * point.y) + lineAbc.C;
	if (floatCmp(result, 0.0f) == 0) {
		return 1;
	}
	return 0;
}

IntersectionPoints2D_2 intersectionLineCircleMQ(Point2D circleCenter, float circleRadius, LineMQ line) {
	IntersectionPoints2D_2 points;
	float a, b, c, delta;

	memset(&points, 0, sizeof(points));

	// ax^2 + bx + c = 0
	a = 1.0f + (line.m * line.m);
	b = (2.0f * line.m * line.q) + ((-2.0f * circleCenter.x) + ((-2.0f * circleCenter.y * line.m)));
	c = ((-2.0f) * circleCenter.y * line.q) + (line.q * line.q) + (circleCenter.x * circleCenter.x) + (circleCenter.y * circleCenter.y) - (circleRadius * circleRadius);

	delta = b * b + ((-4.0f) * a * c);

	if (floatCmp(delta, 0.0f) == 0) {
		points.numPoints = 1;
	}
	else if (delta < 0.0f) {
		points.numPoints = 0;
		return points;
	}
	else {
		points.numPoints = 2;
	}

	points.point1.x = (-b + sqrtf(delta)) / (2.0f * a);
	points.point2.x = (-b - sqrtf(delta)) / (2.0f * a);
	points.point1.y = line.m * points.point1.x + line.q;
	points.point2.y = line.m * points.point2.x + line.q;

	return points;
}

IntersectionPoints2D_2 intersectionLineCircleABC(Point2D circleCenter, float circleRadius, LineABC lineAbc) {
	IntersectionPoints2D_2 points;
	LineMQ line;
	float a, b, c, x_, delta;

	x_ = 0.0f;
	memset(&line, 0, sizeof(line));
	memset(&points, 0, sizeof(points));

	lineAbc = normalizeLineABC2MQ(lineAbc);

	if (!isLineParallelToYaxisABC(lineAbc))
	{
		line = lineABC2MQ(lineAbc);
		// ax^2 + bx + c = 0
		a = 1.0f + (line.m * line.m);
		b = (2.0f * line.m * line.q) + ((-2.0f * circleCenter.x) + ((-2.0f * circleCenter.y * line.m)));
		c = ((-2.0f) * circleCenter.y * line.q) + (line.q * line.q) + (circleCenter.x * circleCenter.x) + (circleCenter.y * circleCenter.y) - (circleRadius * circleRadius);
	}
	else {
		x_ = (-lineAbc.C) / lineAbc.Ax;
		a = 1.0f;
		b = -2.0f * circleCenter.y;
		c = (x_ * x_) + (-2.0f * circleCenter.x * x_) + (circleCenter.x * circleCenter.x) - (circleRadius * circleRadius);
	}


	delta = b * b + ((-4.0f) * a * c);

	if (floatCmp(delta, 0.0f) == 0) {
		points.numPoints = 1;
	}
	else if (delta < 0.0f) {
		points.numPoints = 0;
		return points;
	}
	else {
		points.numPoints = 2;
	}


	if (!isLineParallelToYaxisABC(lineAbc))
	{
		points.point1.x = (-b + sqrtf(delta)) / (2.0f * a);
		points.point2.x = (-b - sqrtf(delta)) / (2.0f * a);
		points.point1.y = line.m * points.point1.x + line.q;
		points.point2.y = line.m * points.point2.x + line.q;
	}
	else {
		points.point1.y = (-b + sqrtf(delta)) / (2.0f * a);
		points.point2.y = (-b - sqrtf(delta)) / (2.0f * a);
		points.point1.x = x_;
		points.point2.x = x_;
	}

	return points;
}

IntersectionLines intersectionLinesABC(LineABC line1, LineABC line2) {
	IntersectionLines inters;

	memset(&inters, 0, sizeof(inters));

	if (floatCmp((line1.Ax * line2.By - line2.Ax * line1.By), 0.0f) == 0) {
		line2 = normalizeLineABC2MQ(line2);
		line1 = normalizeLineABC2MQ(line1);
		if (memcmp(&line1, &line2, sizeof(inters) == 0))
		{
			inters.info = 2;
		}
		else {
			inters.info = 1;
		}
		return inters;
	}

	inters.point.x = (line1.By * line2.C - line2.By * line1.C) / (line1.Ax * line2.By - line2.Ax * line1.By);
	inters.point.y = (line1.C * line2.Ax - line2.C * line1.Ax) / (line1.Ax * line2.By - line2.Ax * line1.By);

	return inters;
}

float triangleAngleA(float AC, float CB, float BA) {
	float angle;
	if (floatCmp(AC, 0.0f) == 0 || floatCmp(BA, 0.0f) == 0) {
		return 0.0f;
	}

	angle = acosf(((AC * AC) + (BA * BA) - (CB * CB)) / (2.0f * AC * BA));
	return angle;
}

float angleBw3Points2D(Point2D origin, Point2D point_b, Point2D point_c) {
	return triangleAngleA(euclidianDistance(origin, point_c), euclidianDistance(point_c, point_b), euclidianDistance(point_b, origin));
}

float distanceBwLinesABC(LineABC line1, LineABC line2, Point2D pointOnLine) {
	IntersectionLines intersLine;
	float circle_Radius, lines_distance_1, lines_distance_2;
	IntersectionPoints2D_2 circle_inters_1, circle_inters_2;

	if (areParallelABC(line1, line2)) {
		return distanceBwParallelLinesABC(line1, line2);
	}

	intersLine = intersectionLinesABC(line1, line2);
	circle_Radius = euclidianDistance(intersLine.point, pointOnLine);
	circle_inters_1 = intersectionLineCircleABC(intersLine.point, circle_Radius, line1);
	circle_inters_2 = intersectionLineCircleABC(intersLine.point, circle_Radius, line2);

	lines_distance_1 = euclidianDistance(circle_inters_1.point1, circle_inters_2.point1);
	lines_distance_2 = euclidianDistance(circle_inters_1.point1, circle_inters_2.point2);

	if (floatCmp(lines_distance_1, lines_distance_2) <= 0) {
		return lines_distance_1;
	}
	return lines_distance_2;
}

Point2D projectPointOnLineABC(Point2D point, LineABC line) {
	IntersectionLines projectionPoint;
	LineABC perpendicularLine;

	perpendicularLine = perpendicularToLinePassingThroughPointABC(line, point);
	projectionPoint = intersectionLinesABC(line, perpendicularLine);

	return projectionPoint.point;
}

int isPointOnSegment(LineSegment segment, Point2D point) {
	if (
		((point.y < segment.A.y) != (point.y < segment.B.y)) &&
		((point.x < segment.A.x) != (point.x < segment.B.x)) &&
		(floatCmp(((segment.B.x - segment.A.x) * (point.y - segment.A.y)), ((segment.B.y - segment.A.y) * (point.x - segment.A.x))) == 0) ||
		((floatCmp(segment.A.x, point.x) == 0) && (floatCmp(segment.A.y, point.y) == 0)) ||
		((floatCmp(segment.B.x, point.x) == 0) && (floatCmp(segment.B.y, point.y) == 0))
		)
	{
		return 1;
	}
	return 0;
}

LineSegmentsDistancePoints distancePointsBwSegments(LineSegment segment1, LineSegment segment2) {
	LineSegmentsDistancePoints segmentsDistances;
	float distanceSeg1A, distanceSeg1B, distanceSeg2A, distanceSeg2B, minDistance, maxDistance, distanceTemp;
	int projectionPresentSeg1A, projectionPresentSeg1B, projectionPresentSeg2A, projectionPresentSeg2B, minFound, maxFound, minSet, maxSet;
	LineABC lineSegment1, lineSegment2;
	IntersectionLines tempLinesIntersection;
	Point2D tempPoint;

	projectionPresentSeg1A = 0;
	projectionPresentSeg1B = 0;
	projectionPresentSeg2A = 0;
	projectionPresentSeg2B = 0;

	minFound = 0;
	maxFound = 0;

	minSet = 0;
	maxSet = 0;

	maxDistance = 0.0f;
	minDistance = 0.0f;

	lineSegment1 = points2lineABC(segment1.A, segment1.B);
	lineSegment2 = points2lineABC(segment2.A, segment2.B);

	tempLinesIntersection = intersectionLinesABC(lineSegment1, lineSegment2);
	if (tempLinesIntersection.info == 0) {
		if (isPointOnSegment(segment1, tempLinesIntersection.point) == 1) {
			minFound = 1;
			minSet = 1;
			minDistance = 0.0f;
			segmentsDistances.min.A = tempLinesIntersection.point;
			segmentsDistances.min.B = tempLinesIntersection.point;
		}
	}

	tempPoint = projectPointOnLineABC(segment1.A, lineSegment2);
	if (isPointOnSegment(segment2, tempPoint) == 1)
	{
		distanceSeg1A = euclidianDistance(tempPoint, segment1.A);
		projectionPresentSeg1A = 1;
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg1A;
			minSet = 1;
		}
		else if (floatCmp(distanceSeg1A, minDistance) < 0) {
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg1A;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg1A;
			maxSet = 1;
		}
		else if (floatCmp(distanceSeg1A, maxDistance) > 0) {
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg1A;
			maxSet = 1;
		}
	}

	tempPoint = projectPointOnLineABC(segment1.B, lineSegment2);
	if (isPointOnSegment(segment2, tempPoint) == 1)
	{
		distanceSeg1B = euclidianDistance(tempPoint, segment1.B);
		projectionPresentSeg1B = 1;
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment1.B;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg1B;
			minSet = 1;
		}
		else if (floatCmp(distanceSeg1B, minDistance) < 0) {
			segmentsDistances.min.A = segment1.B;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg1B;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment1.B;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg1B;
			maxSet = 1;
		}
		else if (floatCmp(distanceSeg1B, maxDistance) > 0) {
			segmentsDistances.max.A = segment1.B;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg1B;
			maxSet = 1;
		}
	}

	tempPoint = projectPointOnLineABC(segment2.A, lineSegment1);
	if (isPointOnSegment(segment1, tempPoint) == 1)
	{
		distanceSeg2A = euclidianDistance(tempPoint, segment2.A);
		projectionPresentSeg2A = 1;
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment2.A;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg2A;
			minSet = 1;
		}
		else if (floatCmp(distanceSeg2A, minDistance) < 0) {
			segmentsDistances.min.A = segment2.A;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg2A;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment2.A;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg2A;
			maxSet = 1;
		}
		else if (floatCmp(distanceSeg2A, maxDistance) > 0) {
			segmentsDistances.max.A = segment2.A;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg2A;
			maxSet = 1;
		}
	}

	tempPoint = projectPointOnLineABC(segment2.B, lineSegment1);
	if (isPointOnSegment(segment2, tempPoint) == 1)
	{
		distanceSeg2B = euclidianDistance(tempPoint, segment2.B);
		projectionPresentSeg2B = 1;
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment2.B;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg2B;
			minSet = 1;
		}
		else if (floatCmp(distanceSeg2B, minDistance) < 0) {
			segmentsDistances.min.A = segment2.B;
			segmentsDistances.min.B = tempPoint;
			minDistance = distanceSeg2B;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment2.B;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg2B;
			maxSet = 1;
		}
		else if (floatCmp(distanceSeg2B, maxDistance) > 0) {
			segmentsDistances.max.A = segment2.B;
			segmentsDistances.max.B = tempPoint;
			maxDistance = distanceSeg2B;
			maxSet = 1;
		}
	}

	if (/*minSet == 0*/1)
	{
		distanceTemp = euclidianDistance(segment1.A, segment2.A);
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = segment2.A;
			minDistance = distanceTemp;
			minSet = 1;
		}
		else if (floatCmp(distanceTemp, minDistance) < 0) {
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = segment2.A;
			minDistance = distanceTemp;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = segment2.A;
			maxDistance = distanceTemp;
			maxSet = 1;
		}
		else if (floatCmp(distanceTemp, maxDistance) > 0) {
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = segment2.A;
			maxDistance = distanceTemp;
			maxSet = 1;
		}

		distanceTemp = euclidianDistance(segment1.A, segment2.B);
		if (minSet == 0)
		{
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = segment2.B;
			minDistance = distanceTemp;
			minSet = 1;
		}
		else if (floatCmp(distanceTemp, minDistance) < 0) {
			segmentsDistances.min.A = segment1.A;
			segmentsDistances.min.B = segment2.B;
			minDistance = distanceTemp;
			minSet = 1;
		}

		if (maxSet == 0)
		{
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = segment2.B;
			maxDistance = distanceTemp;
			maxSet = 1;
		}
		else if (floatCmp(distanceTemp, maxDistance) > 0) {
			segmentsDistances.max.A = segment1.A;
			segmentsDistances.max.B = segment2.B;
			maxDistance = distanceTemp;
			maxSet = 1;
		}
	}
	return segmentsDistances;
}

LineABC lineSegmentToLineABC(LineSegment segment) {
	return points2lineABC(segment.A, segment.B);
}


float minDistanceLineSegmentToLine(LineSegment vectorSegment, LineABC line) {
	float point1Distance, point2Distance;
	IntersectionLines inters;

	inters = intersectionLinesABC(lineSegmentToLineABC(vectorSegment), line);
	if (inters.info == 0) {
		if (isPointOnSegment(vectorSegment, inters.point) != 0) {
			return 0.0f;
		}
	}

	point1Distance = distance2lineABC(vectorSegment.A, line);
	point2Distance = distance2lineABC(vectorSegment.B, line);

	return MIN(point1Distance, point2Distance);
}

float maxDistanceLineSegmentToLine(LineSegment vectorSegment, LineABC line) {
	float point1Distance, point2Distance;
	point1Distance = distance2lineABC(vectorSegment.A, line);
	point2Distance = distance2lineABC(vectorSegment.B, line);

	return MAX(point1Distance, point2Distance);
}


Point2D circleAngleToPoint2D(Point2D circleCenter, float circleRadius, float angleRad) {
	Point2D result_point = circleCenter;
	circleRadius = fabs(circleRadius);
	result_point.x += (circleRadius * cosf(angleRad));
	result_point.y += (circleRadius * sinf(angleRad));

	return result_point;
}

float circlePoint2DToAngle(Point2D circleCenter, Point2D point) {
	float result_angle;
	result_angle = atan2f(point.y - circleCenter.y, point.x - circleCenter.x);
	result_angle = NormalizePiToNegPi(result_angle);

	return result_angle;
}


// https://mathworld.wolfram.com/Circle-CircleIntersection.html
IntersectionPoints2D_2 intersectionBwCircles(Point2D circleCenter_1, float circleRadius_1, Point2D circleCenter_2, float circleRadius_2) {
	IntersectionPoints2D_2 intersections_result = IntersectionPoints2D_2{};
	LineABC line_passing_through_intersections;
	int cmp_result_1, cmp_result_2;
	float distance_between_centers;

	circleRadius_1 = fabs(circleRadius_1);
	circleRadius_2 = fabs(circleRadius_2);

	distance_between_centers = euclidianDistance(circleCenter_1, circleCenter_2);

	cmp_result_1 = floatCmp(distance_between_centers, 0.0f);
	cmp_result_2 = floatCmp(circleRadius_1, circleRadius_2);
	if (cmp_result_1 == 0 && cmp_result_2 != 0) {
		// the 2 circles does not intersect, one circle is inside another
		intersections_result = IntersectionPoints2D_2{};
		return intersections_result;
	}
	else if (cmp_result_1 == 0 && cmp_result_2 == 0) {
		// the 2 circles are the same
		intersections_result = IntersectionPoints2D_2{};
		intersections_result.sameEquation = 1;
		intersections_result.numPoints = 3;
		return intersections_result;
	}

	cmp_result_1 = floatCmp(distance_between_centers, circleRadius_1 + circleRadius_2);
	if (cmp_result_1 > 0) {
		// circles does not intersect, they are too far apart
		intersections_result = IntersectionPoints2D_2{};
		return intersections_result;
	}

	line_passing_through_intersections.By = -(2.0f * (circleCenter_1.y - circleCenter_2.y));
	line_passing_through_intersections.Ax = 2.0f * (circleCenter_2.x - circleCenter_1.x);
	line_passing_through_intersections.C = ((circleCenter_1.x * circleCenter_1.x) + (circleCenter_1.y * circleCenter_1.y) + (circleRadius_2 * circleRadius_2) - ((circleCenter_2.x * circleCenter_2.x) + (circleCenter_2.y * circleCenter_2.y) + (circleRadius_1 * circleRadius_1)));



	intersections_result = intersectionLineCircleABC(circleCenter_1, circleRadius_1, line_passing_through_intersections);

	return intersections_result;
}


float NormalizePiToNegPi(float angle)
{
	float newAngle = angle;
	while (newAngle <= -M_PI) newAngle += (2.0f * M_PI);
	while (newAngle > M_PI) newAngle -= (2.0f * M_PI);
	return newAngle;
}


float NormalizeZeroToPi(float angle) {
	angle = fmodf(angle, (M_PI * 2.0f));  // Get the remainder of the division
	if (angle < 0) {
		angle += (M_PI * 2.0f);  // Adjust for negative angles
	}
	return angle;
}


int isNumber(const char* str, size_t str_length) {
	int dots = 0;
	int numbers = 0;
	int pluses = 0, minuses = 0;

	for (size_t i = 0; i < str_length; i++)
	{
		if (str[i] >= '0' && str[i] <= '9') {
			numbers++;
		}
		else if (str[i] == '+') {
			pluses++;
			if (pluses > 1) {
				break;
			}
		}
		else if (str[i] == '-') {
			minuses++;
			if (minuses > 1) {
				break;
			}
		}
		else if (str[i] == '.') {
			dots++;
			if (dots > 1) {
				break;
			}
		}
		else if (str[i] == ' ') {
			if (numbers > 0 || dots > 0 || pluses > 0 || minuses > 0) {
				break;
			}
		}
		else if (str[i] == '\0') {
			break;
		}
		else {
			break;
		}
	}
	if (numbers > 0) {
		return 1;
	}
	else {
		return 0;
	}
}



Point2D closestPointToSegment(Point2D point, LineSegment seg) {
	Point2D closest_point;
	LineABC seg_line;

	seg_line = points2lineABC(seg.A, seg.B);
	closest_point = projectPointOnLineABC(point, seg_line);
	if (isPointOnSegment(seg, closest_point) != 0) {
		return closest_point;
	}
	if (euclidianDistance(point, seg.A) < euclidianDistance(point, seg.B)) {
		return seg.A;
	}
	return seg.B;
}
