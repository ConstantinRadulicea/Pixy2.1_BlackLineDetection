#pragma once

#include"Pixy2BlackLineDetectionService.h"
#include"ObjectEdges.h"
#include"rgb2hsv.h"
#include <unordered_map>
#include <queue>

using namespace std;




class BlackObjectEdgeDetection
{
public:
	BlackObjectEdgeDetection(Pixy2BlackLineDetectionService &_pixyService, float blackColorThreshold) : pixyService(_pixyService) {
		this->blackColorThreshold = blackColorThreshold;
	}

	bool isBlack(float pixel) {
		if (pixel <= this->blackColorThreshold)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	~BlackObjectEdgeDetection() {}

	RGBcolor getPixel(int16_t x, int16_t y) {
		return pixyService.getPixel(x, y);
	}

	float getPixelLuminosity(int16_t x, int16_t y) {
		return rgb2hsv(getPixel(x, y)).V;
	}




	std::vector<PixelCoordinates> findEdges(int16_t x, int16_t y) {
		std::unordered_map<PixelCoordinates, bool> edges;
		ObjectEdges objectEdges;
		edges = floodFill(this->pixyService.getWidth(), this->pixyService.getHeight(), x, y);

		std::vector<PixelCoordinates> keys;
		keys.reserve(edges.size());
		
		for (auto kv : edges) {
			keys.push_back(kv.first);
		}
		keys.shrink_to_fit();

		return keys;
	}

	

private:
	Pixy2BlackLineDetectionService &pixyService;
	float blackColorThreshold;


	// Function that returns true if
	// the given pixel is valid
	bool isValid(float pixel, int16_t m, int16_t n, int16_t x, int16_t y)
	{
		if (x < 0 || x >= m || y < 0 || y >= n || (!isBlack(pixel))) {
			return false;
		}
		return true;
	}

	// FloodFill function
	std::unordered_map<PixelCoordinates, bool> floodFill(int16_t m, int16_t n, int16_t x, int16_t y)
	{
		std::unordered_map<PixelCoordinates, bool> edges;
		std::unordered_map<PixelCoordinates, bool> body;
		queue<pair<int16_t, int16_t> > queue;

		if (!isValid(getPixelLuminosity(x, y), m, n, x, y)) {
			return edges;
		}
		body[PixelCoordinates{ x, y }] = true;

		// Append the position of starting
		// pixel of the component
		pair<int16_t, int16_t> p(x, y);
		queue.push(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (queue.size() > 0) {
			// Dequeue the front node
			pair<int16_t, int16_t> currPixel = queue.front();
			queue.pop();

			int16_t posX = currPixel.first;
			int16_t posY = currPixel.second;

			// Check if the adjacent
			// pixels are valid

			if ((body.find(PixelCoordinates{ (int16_t)((int16_t)(posX + 1)), posY }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)((int16_t)(posX + 1)), posY }) == edges.end()) ) {
				if (isValid(getPixelLuminosity((int16_t)(posX + 1), posY), m, n, (int16_t)(posX + 1), posY))
				{
					// Color with newC
					// if valid and enqueue
					//screen[(int16_t)(posX + 1)][posY] = newC;
					p.first = (int16_t)(posX + 1);
					p.second = posY;
					queue.push(p);
					body[PixelCoordinates{ (int16_t)((int16_t)(posX + 1)), posY }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)((int16_t)(posX + 1)), posY }] = true;
				}
			}



			if ((body.find(PixelCoordinates{ (int16_t)((int16_t)(posX - 1)), posY }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)((int16_t)(posX - 1)), posY }) == edges.end()) ) {
				if (isValid(getPixelLuminosity((int16_t)(posX - 1), posY), m, n, (int16_t)(posX - 1), posY))
				{
					//screen[(int16_t)(posX - 1)][posY] = newC;
					p.first = (int16_t)(posX - 1);
					p.second = posY;
					queue.push(p);
					body[PixelCoordinates{ (int16_t)((int16_t)(posX - 1)), posY }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)((int16_t)(posX - 1)), posY }] = true;
				}
			}




			if ((body.find(PixelCoordinates{ posX, (int16_t)(posY + 1) }) == body.end()) && (edges.find(PixelCoordinates{ posX, (int16_t)(posY + 1) }) == edges.end())) {
				if (isValid(getPixelLuminosity(posX, (int16_t)(posY + 1)), m, n, posX, (int16_t)(posY + 1)))
				{
					//screen[posX][(int16_t)(posY + 1)] = newC;
					p.first = posX;
					p.second = (int16_t)(posY + 1);
					queue.push(p);
					body[PixelCoordinates{ posX, (int16_t)(posY + 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ posX, (int16_t)(posY + 1) }] = true;
				}
			}



			if ((body.find(PixelCoordinates{ posX, (int16_t)(posY - 1) }) == body.end()) && (edges.find(PixelCoordinates{ posX, (int16_t)(posY - 1) }) == edges.end())) {
				if (isValid(getPixelLuminosity(posX, (int16_t)(posY - 1)), m, n, posX, (int16_t)(posY - 1)))
				{
					//screen[posX][(int16_t)(posY - 1)] = newC;
					p.first = posX;
					p.second = (int16_t)(posY - 1);
					queue.push(p);
					body[PixelCoordinates{ posX, (int16_t)(posY - 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ posX, (int16_t)(posY - 1) }] = true;
				}
			}




			if ((body.find(PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY + 1) }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY + 1) }) == edges.end()) ) {
				if (isValid(getPixelLuminosity((int16_t)(posX + 1), (int16_t)(posY + 1)), m, n, (int16_t)(posX + 1), (int16_t)(posY + 1)))
				{
					// Color with newC
					// if valid and enqueue
					//screen[(int16_t)(posX + 1)][posY] = newC;
					p.first = (int16_t)(posX + 1);
					p.second = (int16_t)(posY + 1);
					queue.push(p);
					body[PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY + 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY + 1) }] = true;
				}
			}


			if ((body.find(PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY - 1) }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY - 1) }) == edges.end()) ) {
				if (isValid(getPixelLuminosity((int16_t)(posX + 1), (int16_t)(posY - 1)), m, n, (int16_t)(posX + 1), (int16_t)(posY - 1)))
				{
					// Color with newC
					// if valid and enqueue
					//screen[(int16_t)(posX + 1)][posY] = newC;
					p.first = (int16_t)(posX + 1);
					p.second = (int16_t)(posY - 1);
					queue.push(p);
					body[PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY - 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)(posX + 1), (int16_t)(posY - 1) }] = true;
				}
			}


			if ((body.find(PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY - 1) }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY - 1) }) == edges.end())) {
				if (isValid(getPixelLuminosity((int16_t)(posX - 1), (int16_t)(posY - 1)), m, n, (int16_t)(posX - 1), (int16_t)(posY - 1)))
				{
					// Color with newC
					// if valid and enqueue
					//screen[(int16_t)(posX + 1)][posY] = newC;
					p.first = (int16_t)(posX - 1);
					p.second = (int16_t)(posY - 1);
					queue.push(p);
					body[PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY - 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY - 1) }] = true;
				}
			}


			if ((body.find(PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY + 1) }) == body.end()) && (edges.find(PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY + 1) }) == edges.end()) ) {
				if (isValid(getPixelLuminosity((int16_t)(posX - 1), (int16_t)(posY + 1)), m, n, (int16_t)(posX - 1), (int16_t)(posY + 1)))
				{
					// Color with newC
					// if valid and enqueue
					//screen[(int16_t)(posX + 1)][posY] = newC;
					p.first = (int16_t)(posX - 1);
					p.second = (int16_t)(posY + 1);
					queue.push(p);
					body[PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY + 1) }] = true;
				}
				else
				{
					edges[PixelCoordinates{ (int16_t)(posX - 1), (int16_t)(posY + 1) }] = true;
				}
			}
		}
		return edges;
	}

};

