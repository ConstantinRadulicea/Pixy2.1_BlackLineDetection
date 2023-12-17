#pragma once

#include"Pixy2BlackLineDetectionService.h"
#include"ObjectEdges.h"
#include"rgb2hsv.h"
#include <unordered_map>


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

	ObjectEdges findEdges(int x, int y) {
		std::unordered_map<PixelCoordinates, bool> edges;
		ObjectEdges objectEdges;
		int x_i = 0, y_i = 0;
		int x_1 = x, y_1 = y;

		if (!isBlack(getPixelLuminosity(x, y))) {
			return objectEdges;
		}

		for (y_i = y; y_i < pixyService.getHeight(); y_i++)		// pixel y++
		{
			y_1 = y_i;
			x_1 = x + 1;
			for (x_i = x_1; x_i < pixyService.getWidth(); x_i++)	// pixel x++
			{
				if (!isBlack(getPixelLuminosity(x_i, y_i)))
				{
					edges[PixelCoordinates{ x_1, y_1 }] = true;
					break;
				}
				x_1 = x_i;
			}
			if (x_i == pixyService.getWidth())
			{
				edges[PixelCoordinates{ x_1, y_1 }] = true;
			}

			x_1 = x - 1;
			for (x_i = x_1; x_i >= 0; x_i--)	// x-- pixel
			{
				if (!isBlack(getPixelLuminosity(x_i, y_i)))
				{
					edges[PixelCoordinates{ x_1, y_1 }] = true;
					break;
				}
				x_1 = x_i;
			}
			if (x_i == -1)
			{
				edges[PixelCoordinates{ x_1, y_1 }] = true;
			}
		}
		if (y_i == pixyService.getHeight())
		{
			edges[PixelCoordinates{ x_1, y_1 }] = true;
		}


		for (y_i = y; y_i >= 0; y_i--)		// pixel y--
		{
			y_1 = y_i;
			x_1 = x + 1;
			for (x_i = x_1; x_i < pixyService.getWidth(); x_i++)	// pixel x++
			{
				if (!isBlack(getPixelLuminosity(x_i, y_i)))
				{
					edges[PixelCoordinates{ x_1, y_1 }] = true;
					break;
				}
				x_1 = x_i;
			}
			if (x_i == pixyService.getWidth())
			{
				edges[PixelCoordinates{ x_1, y_1 }] = true;
			}
			x_1 = x - 1;
			for (x_i = x_1; x_i >= 0; x_i--)	// x-- pixel
			{
				if (!isBlack(getPixelLuminosity(x_i, y_i)))
				{
					edges[PixelCoordinates{ x_1, y_1 }] = true;
					break;
				}
				x_1 = x_i;
			}
			if (x_i == -1)
			{
				edges[PixelCoordinates{ x_1, y_1 }] = true;
			}
		}
		if (y_i == -1)
		{
			edges[PixelCoordinates{ x_1, y_1 }] = true;
		}

		return objectEdges;
	}

	

private:
	Pixy2BlackLineDetectionService &pixyService;
	float blackColorThreshold;
};

