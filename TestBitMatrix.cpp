#include "BitMatrix.h"
#include "rgb2hsv.h"
#include "BitMatrixSkeleton.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Matrix.h"
#include "LinesRoutine.h"


//#define IMG_PATH "img/img1.png"
//#define IMG_PATH "img/black.png"
//#define IMG_PATH "img/test1.png"
//#define IMG_PATH "img/test2.png"
//#define IMG_PATH "img/test3.png"
#define IMG_PATH "img/20241002_194857.jpg" // intersection 1

//#define IMG_PATH "img/20241002_194755.jpg" // straight with start lines
//#define IMG_PATH "img/20241002_194910.jpg" // intersection shiny
//#define IMG_PATH "img/20241002_194812.jpg" // curve 1 with noise
//#define IMG_PATH "img/20241002_194947.jpg" // curve 2
//#define IMG_PATH "img/20241002_194842.jpg" // curve 3



/*

R G R G R
G B G B G
R G R G R


Blue pixel:
Red = (R1 + R2 + R3 + R4) / 4
Blue = Blue
Green = (G1 + G2 + G3 + G4) / 4

Green pixel:
Red = (R1 + R2) / 2
Blue = (B1 + B2) / 2
Green = Green

Red pixel:
Red = Red
Blue = (B1 + B2 + B3 + B4) / 4
Green = (G1 + G2 + G3 + G4) / 4
*/


void interpolate(uint8_t* frame, uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint8_t* r, uint8_t* g, uint8_t* b)
{
	uint8_t* pixel = frame + y * width + x;
	if ((x > 0 && x < (width - 1)) && (y > 0 && y < (height - 1)))
	{
		if (y & 1)
		{
			if (x & 1)
			{
				*r = *pixel;
				*g = (*(pixel - 1) + *(pixel + 1) + *(pixel + width) + *(pixel - width)) >> 2;
				*b = (*(pixel - width - 1) + *(pixel - width + 1) + *(pixel + width - 1) + *(pixel + width + 1)) >> 2;
			}
			else
			{
				*r = (*(pixel - 1) + *(pixel + 1)) >> 1;
				*g = *pixel;
				*b = (*(pixel - width) + *(pixel + width)) >> 1;
			}
		}
		else
		{
			if (x & 1)
			{
				*r = (*(pixel - width) + *(pixel + width)) >> 1;
				*g = *pixel;
				*b = (*(pixel - 1) + *(pixel + 1)) >> 1;
			}
			else
			{
				*r = (*(pixel - width - 1) + *(pixel - width + 1) + *(pixel + width - 1) + *(pixel + width + 1)) >> 2;
				*g = (*(pixel - 1) + *(pixel + 1) + *(pixel + width) + *(pixel - width)) >> 2;
				*b = *pixel;
			}
		}
	}
	else {
		*r = 255;
		*g = 255;
		*b = 255;
	}

	//if (y == 0) {

	//}
	//else if (y >= (height - 1)) {

	//}
	//else if (x == 0) {

	//}
	//else if (x >= (width - 1))
	//{

	//}
}

cv::Mat convertToBayerPattern(const cv::Mat& inputImage) {
	if (inputImage.empty()) {
		throw std::invalid_argument("Input image is empty.");
	}

	// Ensure the image is in BGR format
	cv::Mat bgrImage;
	if (inputImage.channels() == 1) {
		cv::cvtColor(inputImage, bgrImage, cv::COLOR_GRAY2BGR);
	}
	else {
		bgrImage = inputImage.clone();
	}

	// Create an empty Bayer pattern image
	cv::Mat bayerImage(bgrImage.size(), CV_8UC1);

	// Assign Bayer pattern (RGGB)
	for (int y = 0; y < bgrImage.rows; ++y) {
		for (int x = 0; x < bgrImage.cols; ++x) {
			// Bayer pattern: RGGB
			if ((y % 2 == 0) && (x % 2 == 0)) {
				// Red pixel (R)
				bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[2];
			}
			else if ((y % 2 == 0) && (x % 2 == 1)) {
				// Green pixel (G1)
				bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[1];
			}
			else if ((y % 2 == 1) && (x % 2 == 0)) {
				// Green pixel (G2)
				bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[1];
			}
			else {
				// Blue pixel (B)
				bayerImage.at<uchar>(y, x) = bgrImage.at<cv::Vec3b>(y, x)[0];
			}
		}
	}

	return bayerImage;
}

BitMatrix imgToBitMatrix(const char* _img_path, float black_treshold) {
	BitMatrix bitmatrix_img;
	// Read the image from file
	std::string fp = std::string(_img_path);
	//std::cout << "File path: " << fp << std::endl;
	cv::Mat image = cv::imread(fp);


	// Check if the image was loaded
	if (image.empty()) {
		std::cerr << "Error: Could not open or find the image!" << std::endl;
		return bitmatrix_img;
	}


	cv::Mat dst;
	//cv::Size newSize(320, 200);
	int width = 208;
	int height = (int)(width * (float)(316.0 / 208.0));
	cv::Size newSize(height, width);
	// Resize the image
	cv::resize(image, dst, newSize, 0.0, 0.0, cv::INTER_LANCZOS4);
	//cv::resize(image, dst, newSize);
	//image = convertToBayerPattern(dst);
	image = dst;


	bitmatrix_img.init(image.rows, image.cols);
	
	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			// Access the pixel value (BGR format for colored images)
			cv::Vec3b& color = image.at<cv::Vec3b>(row, col);  // For 3-channel color images

			// You can access individual channels: Blue, Green, Red
			uchar blue = color[0];
			uchar green = color[1];
			uchar red = color[2];

			HSVcolor hsv = rgb2hsv(RGBcolor{ red, green, blue });
			float luminosity = hsv.V;

			if (luminosity <= black_treshold) {
				bitmatrix_img.setBit(row, col);
			}
			else {
				bitmatrix_img.unsetBit(row, col);
			}
		}
	}

	BitMatrix scaled;
	size_t downscale_rate = DOWNSCALE_FACTOR;
	scaled.init(bitmatrix_img.getRows() / downscale_rate, bitmatrix_img.getColumns() / downscale_rate);
	BitMatrix::downscale(&scaled, &bitmatrix_img, downscale_rate, DOWNSCALE_COLOR_TRESHOLD);
	return scaled;

	return bitmatrix_img;
}

cv::Mat bitMatrixToMat(BitMatrix &bit_matrix) {
	cv::Mat image(bit_matrix.getRows(), bit_matrix.getColumns(), CV_8UC1);
	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			

			if (bit_matrix.getBit(row, col) == true) {
				image.at<uchar>(row, col) = 255;
			}
			else {
				image.at<uchar>(row, col) = 0;
			}
		}
	}
	return image;
}

std::vector<cv::Point> bitMatrixToArray(BitMatrix& bit_matrix) {
	std::vector<cv::Point> arr;

	for (int row = 0; row < bit_matrix.getRows(); ++row) {
		for (int col = 0; col < bit_matrix.getColumns(); ++col) {


			if (bit_matrix.getBit(row, col) == true) {
				cv::Point point;
				point.x = col;
				point.y = row;
				arr.push_back(point);
			}
 
		}
	}
	return arr;
}

cv::Mat TestFunction(Matrix<uint8_t>* baiern_image) {
	BitMatrix image(baiern_image->getRows() / DOWNSCALE_FACTOR, baiern_image->getCols() / DOWNSCALE_FACTOR);
	BitMatrix temp(image.getRows(), image.getColumns());
	cv::Mat res;
	int windowWidth = 400;  // Adjust this value to fit your screen
	int windowHeight = 320; // Adjust this value to fit your screen

	baiernToBitmatrixDownscale_minPooling(&image, (uint8_t*)(baiern_image->data()), baiern_image->getRows(), baiern_image->getCols(), DOWNSCALE_FACTOR, BLACK_TRERSHOLD, DOWNSCALE_COLOR_TRESHOLD);
	res = bitMatrixToMat(image);
	cv::namedWindow("biern_downcaled", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
	cv::resizeWindow("biern_downcaled", windowWidth, windowHeight);
	cv::imshow("biern_downcaled", res);
	
	
	BitMatrixSkeletonZS(&image, &temp);
	res = bitMatrixToMat(image);
	cv::namedWindow("skeleton", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
	cv::resizeWindow("skeleton", windowWidth, windowHeight);
	cv::imshow("skeleton", res);

	return res;
}

void lineToNParray(std::vector<Point2D_int> line) {
	std::string out;
	size_t i;
	out.append("np.array([");
	for (i = 0; i < line.size(); i++)
	{
		std::string point;
		point = std::string("[") + std::to_string(line[i].x) + std::string(", ") + std::to_string(line[i].y) + std::string("]");
		point.append(", ");
		out.append(point);
	}
	if (i > 0) {
		out.pop_back();
		out.pop_back();
	}
	out.append("])");
	std::cout << out << std::endl;
}

void lineToXY(std::vector<Point2D_int> line) {
	std::string out_x, out_y;
	size_t i;
	out_x.append("x = [");
	out_y.append("y = [");
	for (i = 0; i < line.size(); i++)
	{
		std::string point;
		point = std::to_string(line[i].x);
		point.append(" ");
		out_x.append(point);
		point = std::to_string(line[i].y);
		point.append(" ");
		out_y.append(point);
	}
	if (i > 0) {
		out_x.pop_back();
		out_y.pop_back();
	}
	out_x.append("];");
	out_y.append("];");
	std::cout << out_x << std::endl;
	std::cout << out_y << std::endl;
}



void TestVectors() {
	std::vector<std::vector<Point2D_int>> vectors;
	cv::Mat original_img = cv::imread(IMG_PATH);

	cv::Mat dst;
	//cv::Size newSize(320, 200);
	int width = CAM_RES2_HEIGHT;
	int height = (int)(width * (float)(CAM_RES2_WIDTH / CAM_RES2_HEIGHT));
	cv::Size newSize(CAM_RES2_WIDTH, CAM_RES2_HEIGHT);
	// Resize the image
	cv::resize(original_img, dst, newSize, 0.0, 0.0, cv::INTER_LANCZOS4);

	cv::Mat baiern_img = convertToBayerPattern(dst);
	Matrix<uint8_t> temp_baiern_matrix = matToMatrix<uint8_t>(baiern_img);
	for (size_t i = 0; i < 10; i++) {
		vectors = LinesRoutine((uint8_t*)(temp_baiern_matrix.data()), VECTOR_APPROXIMATION_EPSILON, DOWNSCALE_FACTOR);
	}

	std::vector<std::vector<cv::Point>> approxCurve;
	for (size_t i = 0; i < vectors.size(); i++) {
		lineToNParray(vectors[i]);
		lineToXY(vectors[i]);
		std::vector<cv::Point> temp;
		approxCurve.push_back(temp);
		for (size_t j = 0; j < vectors[i].size(); j++) {
			approxCurve[i].push_back(cv::Point(vectors[i][j].x, vectors[i][j].y));
		}
	}


	cv::Mat result = cv::Mat::zeros(baiern_img.rows / DOWNSCALE_FACTOR, baiern_img.cols / DOWNSCALE_FACTOR, CV_8UC3);  // Create a blank canvas
	cv::RNG rng(time(0));
	for (size_t i = 0; i < approxCurve.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		// Draw the simplified skeleton using the approximate curve
		for (size_t j = 0; j < approxCurve[i].size() - 1; ++j) {
			cv::line(result, approxCurve[i][j], approxCurve[i][j + 1], color, 1);
		}
	}


	// Display the result
	// Create a window
	// Resize the window to a specific size (adjust width and height as needed)
	int windowWidth = 400;  // Adjust this value to fit your screen
	int windowHeight = 320; // Adjust this value to fit your screen
	cv::namedWindow("original image", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
	cv::resizeWindow("original image", windowWidth, windowHeight);
	cv::imshow("original image", original_img);
	

	cv::namedWindow("baiern", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
	cv::resizeWindow("baiern", windowWidth, windowHeight);
	cv::imshow("baiern", baiern_img);

	cv::namedWindow("lines", cv::WINDOW_NORMAL); // WINDOW_NORMAL allows resizing
	cv::resizeWindow("lines", windowWidth, windowHeight);
	cv::imshow("lines", result);


	TestFunction(&temp_baiern_matrix);
	cv::waitKey(0);  // Wait for a key press before closing the window
}






void main() {
	TestVectors();
	//TestBitMatrix();
}




