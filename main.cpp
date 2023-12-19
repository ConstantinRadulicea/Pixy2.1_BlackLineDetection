#include <windows.h>
#include <iostream>
#include <fstream>

#include "BlackObjectEdgeDetection.h"
#include "img1.h"

using namespace std;


// https://notisrac.github.io/FileToCArray/


void writeMatlabImage(string filename, const int *image, int width, int height) {
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
			if (x < width-1)
			{
				myfile << to_string(rgb2hsv(pixel).V) + ",";
			}
			else
			{
				myfile << to_string(rgb2hsv(pixel).V);
			}
		}
		if (y < height-1)
		{
			myfile << "\n";
		}
	}
	
	myfile.close();
}

void writeMatlabEdges(string filename, std::vector<PixelCoordinates> vector) {
	std::ofstream myfile;

	myfile.open(filename, ios::out | ios::trunc);

	for (size_t i = 0; i < vector.size(); i++)
	{
		myfile << to_string(vector[i].x) + "," + to_string(vector[i].y);
		if ((i+1) < vector.size())
		{
			myfile << "\n";
		}
	}

	myfile.close();
}

int main() {
	// Open serial port
	//HANDLE serialHandle;
	//char buffer[] = "ciao\n";

	//serialHandle = CreateFileA("\\\\.\\COM1", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	//// Do some basic settings
	//DCB serialParams = { 0 };
	//serialParams.DCBlength = sizeof(serialParams);

	//GetCommState(serialHandle, &serialParams);
	//serialParams.BaudRate = 9600;
	//serialParams.ByteSize = 8;
	//serialParams.StopBits = TWOSTOPBITS;
	//serialParams.Parity = NOPARITY;
	//SetCommState(serialHandle, &serialParams);

	//// Set timeouts
	//COMMTIMEOUTS timeout = { 0 };
	//timeout.ReadIntervalTimeout = 50;
	//timeout.ReadTotalTimeoutConstant = 50;
	//timeout.ReadTotalTimeoutMultiplier = 50;
	//timeout.WriteTotalTimeoutConstant = 50;
	//timeout.WriteTotalTimeoutMultiplier = 10;

	//SetCommTimeouts(serialHandle, &timeout);

	//WriteFile(serialHandle, buffer, sizeof(buffer), NULL, NULL);


	writeMatlabImage("image.csv", Untitled, UNTITLED_WIDTH, UNTITLED_HEIGHT);

	Pixy2BlackLineDetectionService pixy2BlackLineDetectionService(Untitled, UNTITLED_WIDTH, UNTITLED_HEIGHT);
	BlackObjectEdgeDetection blackObjectEdgeDetection(pixy2BlackLineDetectionService, 0.1);
	writeMatlabEdges("edges.csv", blackObjectEdgeDetection.findEdges(259, 174));
	
	//CloseHandle(serialHandle);

	return 0;
}