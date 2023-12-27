#include <windows.h>
#include <iostream>
#include <fstream>

#include "thinning.h"
#include "BlackObjectEdgeDetection.h"
#include "img1.h"

using namespace std;


// https://notisrac.github.io/FileToCArray/




int main() {
	std::unordered_map<PixelCoordinates, bool> result;
	std::unordered_map<PixelCoordinates, bool> objectBody;
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
	objectBody = blackObjectEdgeDetection.findEdges(59, 59);
	thinning(objectBody, result);


	writeMatlabEdges("edges.csv", mapToVector(result));
	
	//CloseHandle(serialHandle);

	return 0;
}