
#include "BlackObjectEdgeDetection.h"
#include "img1.h"


int main() {

	Pixy2BlackLineDetectionService pixy2BlackLineDetectionService(Untitled, UNTITLED_WIDTH, UNTITLED_HEIGHT);
	BlackObjectEdgeDetection blackObjectEdgeDetection(pixy2BlackLineDetectionService, 0.1);
	blackObjectEdgeDetection.findEdges(1, 0);
	


	return 0;
}