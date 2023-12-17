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

	RGBcolor getPixel(int x, int y) {
		return pixyService.getPixel(x, y);
	}

	float getPixelLuminosity(int x, int y) {
		return rgb2hsv(getPixel(x, y)).V;
	}




	std::vector<PixelCoordinates> findEdges(int x, int y) {
		std::unordered_map<PixelCoordinates, bool> edges;
		ObjectEdges objectEdges;
		edges = floodFill(this->pixyService.getWidth(), this->pixyService.getHeight(), x, y);

		std::vector<PixelCoordinates> keys;
		keys.reserve(edges.size());

		for (auto kv : edges) {
			if (kv.second == true)
			{
				keys.push_back(kv.first);
			}
		}
		keys.shrink_to_fit();

		return keys;
	}

	

private:
	Pixy2BlackLineDetectionService &pixyService;
	float blackColorThreshold;


	// Function that returns true if
	// the given pixel is valid
	bool isValid(float pixel, int m, int n, int x, int y)
	{
		if (x < 0 || x >= m || y < 0 || y >= n || (!isBlack(pixel))) {
			return false;
		}
		return true;
	}

	// FloodFill function
	std::unordered_map<PixelCoordinates, bool> floodFill(int m, int n, int x, int y)
	{
		std::unordered_map<PixelCoordinates, bool> edges;
		std::unordered_map<PixelCoordinates, bool> body;
		queue<pair<int, int> > queue;

		if (!isValid(getPixelLuminosity(x, y), m, n, x, y)) {
			return edges;
		}
		body[PixelCoordinates{ x, y }] = true;

		// Append the position of starting
		// pixel of the component
		pair<int, int> p(x, y);
		queue.push(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (queue.size() > 0) {
			// Dequeue the front node
			pair<int, int> currPixel = queue.front();
			queue.pop();

			int posX = currPixel.first;
			int posY = currPixel.second;

			// Check if the adjacent
			// pixels are valid

			if (body[PixelCoordinates{ posX + 1, posY }] != true && edges[PixelCoordinates{ posX + 1, posY }] != true && isValid(getPixelLuminosity(posX + 1, posY), m, n, posX + 1, posY)) {
				// Color with newC
				// if valid and enqueue
				//screen[posX + 1][posY] = newC;
				p.first = posX + 1;
				p.second = posY;
				queue.push(p);
				body[PixelCoordinates{ posX + 1, posY }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX + 1, posY }] = true;
			}


			if (body[PixelCoordinates{ posX - 1, posY }] != true && edges[PixelCoordinates{ posX - 1, posY }] != true && isValid(getPixelLuminosity(posX - 1, posY), m, n, posX - 1, posY)) {
				//screen[posX - 1][posY] = newC;
				p.first = posX - 1;
				p.second = posY;
				queue.push(p);
				body[PixelCoordinates{ posX - 1, posY }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX - 1, posY }] = true;
			}



			if (body[PixelCoordinates{ posX, posY + 1 }] != true && edges[PixelCoordinates{ posX, posY + 1 }] != true && isValid(getPixelLuminosity(posX, posY + 1), m, n, posX, posY + 1)) {
				//screen[posX][posY + 1] = newC;
				p.first = posX;
				p.second = posY + 1;
				queue.push(p);
				body[PixelCoordinates{ posX, posY + 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX, posY + 1 }] = true;
			}


			if (body[PixelCoordinates{ posX, posY - 1 }] != true && edges[PixelCoordinates{ posX, posY - 1 }] != true && isValid(getPixelLuminosity(posX, posY - 1), m, n, posX, posY - 1)) {
				//screen[posX][posY - 1] = newC;
				p.first = posX;
				p.second = posY - 1;
				queue.push(p);
				body[PixelCoordinates{ posX, posY - 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX, posY - 1 }] = true;
			}



			if (body[PixelCoordinates{ posX + 1, posY + 1 }] != true && edges[PixelCoordinates{ posX + 1, posY + 1 }] != true && isValid(getPixelLuminosity(posX + 1, posY + 1), m, n, posX + 1, posY + 1)) {
				// Color with newC
				// if valid and enqueue
				//screen[posX + 1][posY] = newC;
				p.first = posX + 1;
				p.second = posY + 1;
				queue.push(p);
				body[PixelCoordinates{ posX + 1, posY + 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX + 1, posY + 1 }] = true;
			}

			if (body[PixelCoordinates{ posX + 1, posY - 1 }] != true && edges[PixelCoordinates{ posX + 1, posY - 1 }] != true && isValid(getPixelLuminosity(posX + 1, posY - 1), m, n, posX + 1, posY - 1)) {
				// Color with newC
				// if valid and enqueue
				//screen[posX + 1][posY] = newC;
				p.first = posX + 1;
				p.second = posY - 1;
				queue.push(p);
				body[PixelCoordinates{ posX + 1, posY - 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX + 1, posY - 1 }] = true;
			}

			if (body[PixelCoordinates{ posX - 1, posY - 1 }] != true && edges[PixelCoordinates{ posX - 1, posY - 1 }] != true && isValid(getPixelLuminosity(posX - 1, posY - 1), m, n, posX - 1, posY - 1)) {
				// Color with newC
				// if valid and enqueue
				//screen[posX + 1][posY] = newC;
				p.first = posX - 1;
				p.second = posY - 1;
				queue.push(p);
				body[PixelCoordinates{ posX - 1, posY - 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX - 1, posY - 1 }] = true;
			}

			if (body[PixelCoordinates{ posX - 1, posY + 1 }] != true && edges[PixelCoordinates{ posX - 1, posY + 1 }] != true && isValid(getPixelLuminosity(posX - 1, posY + 1), m, n, posX - 1, posY + 1)) {
				// Color with newC
				// if valid and enqueue
				//screen[posX + 1][posY] = newC;
				p.first = posX - 1;
				p.second = posY + 1;
				queue.push(p);
				body[PixelCoordinates{ posX - 1, posY + 1 }] = true;
			}
			else
			{
				edges[PixelCoordinates{ posX - 1, posY + 1 }] = true;
			}
		}
		return edges;
	}

};

