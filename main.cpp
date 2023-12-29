#include <windows.h>
#include <iostream>
#include <fstream>

#include "BlackObjectEdgeDetection.h"
#include "img1.h"
#include "img2.h"
#include "img3.h"
#include "img4.h"
//#include "403647780_349539491154334_1837665137385227264_n.h"
#include "BitMatrix.h"

#define IMG img1

using namespace std;


// https://notisrac.github.io/FileToCArray/




int main() {
	clock_t start, stop, delta;

	BitMatrix result(UNTITLED_HEIGHT, UNTITLED_WIDTH);
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


	writeMatlabImage("image.csv", IMG, UNTITLED_WIDTH, UNTITLED_HEIGHT);

	Pixy2BlackLineDetectionService pixy2BlackLineDetectionService(IMG, UNTITLED_WIDTH, UNTITLED_HEIGHT);
	BlackObjectEdgeDetection blackObjectEdgeDetection(pixy2BlackLineDetectionService, 0.25f);
	

	start = clock();
	blackObjectEdgeDetection.readHorizontalLines(3);
	blackObjectEdgeDetection.fillRandomBlackPixels();

	//blackObjectEdgeDetection.getObjectSkeleton(0, 0, result);
	//blackObjectEdgeDetection.getObjectSkeleton(0, 199, result);
	stop = clock();
	std::cout << "Thinning Clocks passed: " << stop - start << std::endl;


	//writeMatlabEdges("edges.csv", bitMatrixToVector(result));
	
	//CloseHandle(serialHandle);

	return 0;
}