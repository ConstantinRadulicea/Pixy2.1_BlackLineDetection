#ifndef __VECTORLINE_H__
#define __VECTORLINE_H__
#include <vector>
#include "geometry2D.h"

class VectorLine
{
public:
	VectorLine(std::vector<Point2D> line) {
		this->line = line;
	}
	VectorLine(std::vector<Point2D_int> line) {
		this->line.clear();
		this->line.reserve(line.size());
		for (size_t i = 0; i < line.size(); i++) {
			this->addPoint(line[i]);
		}
	}
	VectorLine() {
	}
	~VectorLine(){}

	void addPoint(Point2D point) {
		this->line.push_back(point);
	}

	Point2D closestToLine(Point2D point) {
		Point2D closest_point;
		Point2D temp_closest_point;
		LineSegment temp_segment;
		
		float min_distance = FLT_MAX;
		float temp_distance;
		closest_point = point;

		for (int i = 0; i < (int)(this->line.size())-1; i++)
		{
			temp_segment.A = line[i];
			temp_segment.B = line[i+1];
			temp_closest_point = closestPointToSegment(point, temp_segment);
			temp_distance = euclidianDistance(temp_closest_point, point);
			if (floatCmp(temp_distance, min_distance) < 0)
			{
				closest_point = temp_closest_point;
				min_distance = temp_distance;
			}
		}
		return closest_point;
	}
	void addPoint(Point2D_int point) {
		Point2D temp;
		temp.x = point.x;
		temp.y = point.y;
		this->line.push_back(temp);
	}


private:
	std::vector<Point2D> line;
};




#endif // !__VECTORLINE_H__
