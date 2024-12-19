#include "LinesRoutine.h"


std::vector<std::vector<Point2D_int>> LinesRoutine(uint8_t* baiern_image, float vector_approximation_epsilon, size_t _downscale_factor) {
	static size_t frame_height = CAM_RES2_HEIGHT;
	static size_t frame_width = CAM_RES2_WIDTH;
	static size_t downscale_factor = MIN_DOWNSCALE_FACTOR;
	static size_t scaled_down_frame_height = CAM_RES2_HEIGHT / MIN_DOWNSCALE_FACTOR;
	static size_t scaled_down_frame_width = CAM_RES2_WIDTH / MIN_DOWNSCALE_FACTOR;

	static BitMatrix image(scaled_down_frame_height, scaled_down_frame_width);
	static BitMatrix body(scaled_down_frame_height, scaled_down_frame_width);
	static BitMatrix temp(scaled_down_frame_height, scaled_down_frame_width);
	static std::vector<Point2D_int>* longestPath;
	static std::vector<Point2D_int> approxCurve;
	static std::vector<std::vector<Point2D_int>> vectors;
	static BitMatrixPosition pixelPosition;

	vectors.clear();

#ifdef LINESROUTINE_DEBUG
	// Start time
	auto start = std::chrono::high_resolution_clock::now();
#endif // LINESROUTINE_DEBUG

	if (_downscale_factor != downscale_factor)
	{
		if (_downscale_factor < MIN_DOWNSCALE_FACTOR) {
			_downscale_factor = MIN_DOWNSCALE_FACTOR;
		}
		downscale_factor = _downscale_factor;
		scaled_down_frame_width = frame_width / downscale_factor;
		scaled_down_frame_height = frame_height / downscale_factor;

		image.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
		body.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
		temp.resize_no_shrinktofit(scaled_down_frame_height, scaled_down_frame_width);
	}

	baiernToBitmatrixDownscale_minPooling(&image, (uint8_t*)(baiern_image), CAM_RES2_HEIGHT, CAM_RES2_WIDTH, downscale_factor, BLACK_TRERSHOLD, DOWNSCALE_COLOR_TRESHOLD);
	BitMatrixSkeletonZS(&image, &temp);
	for (;;)
	{
		pixelPosition = image.getFirstSetPixel();
		if (!(pixelPosition.valid)) {
			break;
		}
		image.floodFillOnesDelete(pixelPosition.row, pixelPosition.column, &body);

		if (body.countNonZero() < MIN_LINE_LENGTH) {
			continue;
		}

		longestPath = BitMatrix::findLongestPath(&body, &temp);
		if (longestPath == NULL) {
			continue;
		}
		ramerDouglasPeucker(longestPath, vector_approximation_epsilon, &approxCurve);
		delete longestPath;
		if (approxCurve.size() > 0) {
			vectors.push_back(approxCurve);
		}
		approxCurve.clear();
	}

#ifdef LINESROUTINE_DEBUG
	// End time
	auto end = std::chrono::high_resolution_clock::now();
	// Calculate the duration
	std::chrono::duration<double> duration = end - start;
	// Output the result in seconds
	std::cout << "Function execution time: " << duration.count() << " seconds" << std::endl;
#endif // LINESROUTINE_DEBUG

	return vectors;
}