#include <windows.h>
#include <iostream>
#include <fstream>

#include "BlackObjectEdgeDetection.h"
#include "img1.h"
#include "img2.h"
#include "img3.h"
#include "img4.h"
#include "img5.h"
#include "BitMatrix.h"

#define IMG img5

using namespace std;


// https://notisrac.github.io/FileToCArray/


HANDLE serialSetup(const char *port = "\\\\.\\COM1", int baudrate = 115200) {
	// Open serial port
	HANDLE serialHandle;

	serialHandle = CreateFileA(port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = baudrate;
	serialParams.ByteSize = 8;
	serialParams.StopBits = TWOSTOPBITS;
	serialParams.Parity = NOPARITY;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serialHandle, &timeout);
	return serialHandle;
}

static uint16_t serial_read_uint16_t(HANDLE serialHandle) {
	uint16_t res;
	DWORD bytesRead = 0;
	DWORD bytesToRead = sizeof(res);

	while (bytesToRead > 0)
	{
		BOOL awd = ReadFile(serialHandle, &(((char*)&res)[bytesRead]), bytesToRead, &bytesRead, NULL);
		bytesToRead -= bytesRead;
	}
	return res;
}


static uint8_t serial_read_uint8_t(HANDLE serialHandle) {
	uint8_t res;
	DWORD bytesRead = 0;
	DWORD bytesToRead = sizeof(res);

	while (bytesToRead > 0)
	{
		BOOL awd = ReadFile(serialHandle, &(((char*)&res)[bytesRead]), bytesToRead, &bytesRead, NULL);
		bytesToRead -= bytesRead;
	}
	return res;
}

static void serial_write_uint16_t(HANDLE serialHandle, uint16_t val) {
	DWORD bytesWrite = 0;
	DWORD bytesToWrite = sizeof(val);

	while (bytesToWrite > 0)
	{
		BOOL awd = WriteFile(serialHandle, &(((char*)&val)[bytesWrite]), bytesToWrite, &bytesWrite, NULL);
		bytesToWrite -= bytesWrite;
	}
}





int main() {
	clock_t start, stop, delta;
	HANDLE serialHandle;

	BitMatrix result(UNTITLED_HEIGHT, UNTITLED_WIDTH);

	//serialHandle = serialSetup("\\\\.\\COM6", 115200);

	//int r, g, b;
	//uint16_t x, y;


	//for (y = 0; y < 190; y++)
	//{
	//	for (x = 0; x < 300; x++)
	//	{
	//		serial_write_uint16_t(serialHandle, x);
	//		serial_write_uint16_t(serialHandle, y);
	//		r = serial_read_uint8_t(serialHandle);
	//		g = serial_read_uint8_t(serialHandle);
	//		b = serial_read_uint8_t(serialHandle);
	//		std::cout << x << " " << y << " " << r << " " << g << " " << b << std::endl;
	//		//std::cout << x << " " << y << std::endl;
	//	}
	//}

	//CloseHandle(serialHandle);


	writeMatlabImage("image.csv", IMG, UNTITLED_WIDTH, UNTITLED_HEIGHT);

	Pixy2BlackLineDetectionService pixy2BlackLineDetectionService(IMG, UNTITLED_WIDTH, UNTITLED_HEIGHT);
	BlackObjectEdgeDetection blackObjectEdgeDetection(pixy2BlackLineDetectionService, 0.25f);
	

	start = clock();
	blackObjectEdgeDetection.readHorizontalLines(4);
	blackObjectEdgeDetection.fillRandomBlackPixels(20);

	//blackObjectEdgeDetection.getObjectSkeleton(0, 0, result);
	//blackObjectEdgeDetection.getObjectSkeleton(0, 199, result);
	stop = clock();
	std::cout << "Thinning Clocks passed: " << stop - start << std::endl;


	//writeMatlabEdges("edges.csv", bitMatrixToVector(result));
	
	//CloseHandle(serialHandle);

	return 0;
}