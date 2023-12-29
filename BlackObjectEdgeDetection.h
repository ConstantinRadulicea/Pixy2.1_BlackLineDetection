#pragma once

#include"Pixy2BlackLineDetectionService.h"
#include"ObjectEdges.h"
#include"rgb2hsv.h"
#include "thinning.h"
#include "BitMatrix.h"
#include <unordered_map>
#include <queue>

using namespace std;

static void writeMatlabImage(string filename, const int* image, int width, int height) {
	std::ofstream myfile;
	int offset;
	RGBcolor pixel;


	myfile.open(filename, ios::out | ios::trunc);


	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			offset = (y * width) + x;
			pixel.R = (image[offset] >> 0) & (int)0xff;
			pixel.G = (image[offset] >> 8) & (int)0xff;
			pixel.B = (image[offset] >> 16) & (int)0xff;
			if (x < width - 1)
			{
				myfile << to_string(rgb2hsv(pixel).V) + ",";
			}
			else
			{
				myfile << to_string(rgb2hsv(pixel).V);
			}
		}
		if (y < height - 1)
		{
			myfile << "\n";
		}
	}

	myfile.close();
}

static void writeMatlabEdges(string filename, std::vector<PixelCoordinates> vector) {
	std::ofstream myfile;

	myfile.open(filename, ios::out | ios::trunc);

	for (size_t i = 0; i < vector.size(); i++)
	{
		myfile << to_string(vector[i].x) + "," + to_string(vector[i].y);
		if ((i + 1) < vector.size())
		{
			myfile << "\n";
		}
	}

	myfile.close();
}


static std::vector<PixelCoordinates> mapToVector(std::unordered_map<PixelCoordinates, bool>& map) {
	std::vector<PixelCoordinates> keys;
	keys.reserve(map.size());

	for (auto kv : map) {
		keys.push_back(kv.first);
	}
	keys.shrink_to_fit();

	return keys;
}


static std::vector<PixelCoordinates> bitMatrixToVector(BitMatrix& map) {
	std::vector<PixelCoordinates> keys;
	PixelCoordinates coord;
	keys.reserve(map.countNonZero());

	for (size_t x = 0; x < map.getColumns(); x++)
	{
		for (size_t y = 0; y < map.getRows(); y++) {
			if (map.getBitXY(x, y))
			{
				coord.x = x;
				coord.y = y;
				keys.push_back(coord);
			}
		}
	}
	keys.shrink_to_fit();
	return keys;
}


class BlackObjectEdgeDetection
{
public:
	BlackObjectEdgeDetection(Pixy2BlackLineDetectionService &_pixyService, float blackColorThreshold) : pixyService(_pixyService) {
		this->blackColorThreshold = blackColorThreshold;

		this->blackPixels.init(this->pixyService.getHeight(), this->pixyService.getWidth());
		this->whitePixels.init(this->pixyService.getHeight(), this->pixyService.getWidth());
	}
	~BlackObjectEdgeDetection() {}

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

	bool isBlackAt(int16_t x, int16_t y) {
		float luminosity;

		if (this->blackPixels.getBitXY(x, y))
		{
			return true;
		}
		else if (this->whitePixels.getBitXY(x, y)) {
			return false;
		}
		else {
			luminosity = this->getPixelLuminosity(x, y);

			if (this->isBlack(luminosity)) {
				this->blackPixels.setBitValueXY(x, y, true);
				return true;
			}
			else {
				this->whitePixels.setBitValueXY(x, y, true);
				return false;
			}
		}
	}

	RGBcolor getRgbPixel(int16_t x, int16_t y) {
		return pixyService.getRgbPixel(x, y);
	}

	float getPixelLuminosity(int16_t x, int16_t y) {
		return rgb2hsv(getRgbPixel(x, y)).V;
	}

	void getObject(int16_t x, int16_t y, BitMatrix&objectBody) {
		floodFill(this->pixyService.getWidth(), this->pixyService.getHeight(), x, y, objectBody);
	}

	void getObjectSkeleton(int16_t x, int16_t y, BitMatrix&objectSkeleton) {
		BitMatrix objectBody(this->pixyService.getHeight(), this->pixyService.getWidth());
		objectSkeleton.clear();
		getObject(x, y, objectBody);
		thinning(objectBody, objectSkeleton);
	}


	void clear() {
		this->blackPixels.clear();
		this->whitePixels.clear();
	}

private:
	Pixy2BlackLineDetectionService &pixyService;
	BitMatrix blackPixels;
	BitMatrix whitePixels;
	float blackColorThreshold;

	// Function that returns true if
	// the given pixel is valid
	bool isValid(float pixel, int16_t width, int16_t height, int16_t x, int16_t y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height || (!isBlack(pixel))) {
			return false;
		}
		return true;
	}

	bool isInsideBoundaries(int16_t x, int16_t y) {
		if (x < 0 || x >= this->pixyService.getWidth() || y < 0 || y >= this->pixyService.getHeight()) {
			return false;
		}
		return true;
	}

	// FloodFill function
	void floodFill(int16_t m, int16_t n, int16_t x, int16_t y, BitMatrix & body)
	{
		BitMatrix edges(n, m);
		//std::unordered_map<PixelCoordinates, bool> body;
		queue<pair<int16_t, int16_t> > queue;
		//body.clear();

		if ( !isInsideBoundaries(x, y) || !isBlackAt(x, y) ) {
			return;
		}
		//body[PixelCoordinates{ x, y }] = true;
		body.setBitValueXY(x, y, true);

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

			if (isInsideBoundaries((int16_t)(posX + 1), posY) && !body.getBitXY((int16_t)(posX + 1), posY) && isBlackAt((int16_t)(posX + 1), posY))
			{
				p.first = (int16_t)(posX + 1);
				p.second = posY;
				queue.push(p);
				body.setBitValueXY((int16_t)((int16_t)(posX + 1)), posY, true);
			}

			if (isInsideBoundaries((int16_t)(posX - 1), posY) && !body.getBitXY((int16_t)(posX - 1), posY) && isBlackAt((int16_t)(posX - 1), posY))
			{
				p.first = (int16_t)(posX - 1);
				p.second = posY;
				queue.push(p);
				body.setBitValueXY((int16_t)(posX - 1), posY, true);
			}

			if (isInsideBoundaries(posX, (int16_t)(posY + 1)) && !body.getBitXY(posX, (int16_t)(posY + 1)) && isBlackAt(posX, (int16_t)(posY + 1)))
			{
				//screen[posX][(int16_t)(posY + 1)] = newC;
				p.first = posX;
				p.second = (int16_t)(posY + 1);
				queue.push(p);
				body.setBitValueXY(posX, (int16_t)(posY + 1), true);
			}

			if (isInsideBoundaries(posX, (int16_t)(posY - 1)) && !body.getBitXY(posX, (int16_t)(posY - 1)) && isBlackAt(posX, (int16_t)(posY - 1)))
			{
				p.first = posX;
				p.second = (int16_t)(posY - 1);
				queue.push(p);
				body.setBitValueXY(posX, (int16_t)(posY - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posX + 1), (int16_t)(posY + 1)) && !body.getBitXY((int16_t)(posX + 1), (int16_t)(posY + 1)) && isBlackAt((int16_t)(posX + 1), (int16_t)(posY + 1)))
			{
				p.first = (int16_t)(posX + 1);
				p.second = (int16_t)(posY + 1);
				queue.push(p);
				body.setBitValueXY((int16_t)(posX + 1), (int16_t)(posY + 1), true);
			}

			if (isInsideBoundaries((int16_t)(posX + 1), (int16_t)(posY - 1)) && !body.getBitXY((int16_t)(posX + 1), (int16_t)(posY - 1)) && isBlackAt((int16_t)(posX + 1), (int16_t)(posY - 1)))
			{
				p.first = (int16_t)(posX + 1);
				p.second = (int16_t)(posY - 1);
				queue.push(p);
				body.setBitValueXY((int16_t)(posX + 1), (int16_t)(posY - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posX - 1), (int16_t)(posY - 1)) && !body.getBitXY((int16_t)(posX - 1), (int16_t)(posY - 1)) && isBlackAt((int16_t)(posX - 1), (int16_t)(posY - 1)))
			{
				p.first = (int16_t)(posX - 1);
				p.second = (int16_t)(posY - 1);
				queue.push(p);
				body.setBitValueXY((int16_t)(posX - 1), (int16_t)(posY - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posX - 1), (int16_t)(posY + 1)) && !body.getBitXY((int16_t)(posX - 1), (int16_t)(posY + 1)) && isBlackAt((int16_t)(posX - 1), (int16_t)(posY + 1)))
			{
				p.first = (int16_t)(posX - 1);
				p.second = (int16_t)(posY + 1);
				queue.push(p);
				body.setBitValueXY((int16_t)(posX - 1), (int16_t)(posY + 1), true);
			}
		}
	}
};

