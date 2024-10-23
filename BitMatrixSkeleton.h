#ifndef __BITMATRIXSKELETON_H__
#define __BITMATRIXSKELETON_H__


/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 *
 * Author:  Nash (nash [at] opencv-code [dot] com)
 * Website: http://opencv-code.com
 * 
 */

//#include <unordered_map>
//#include"Pixy2BlackLineDetectionService.h"
//#include "BlackObjectEdgeDetection.h"
//#include"ObjectEdges.h"
#include "BitMatrix.h"


typedef struct PixelCoordinates {
    int16_t x;
    int16_t y;
    bool operator==(const PixelCoordinates& other) const
    {
        return (x == other.x
            && y == other.y);
    }
}PixelCoordinates;


/**
 * Function for thinning the given binary image
 * you need 3 more BitMatrix-es of the same size simoultaneously
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,1]
 * 		dst  The destination image
 */
void BitMatrixSkeleton(BitMatrix* src, BitMatrix* dst);
void BitMatrixSkeleton(BitMatrix* matrix);

void BitMatrixSkeletonZS(BitMatrix* matrix, BitMatrix* marker);
void BitMatrixSkeletonZS(BitMatrix* matrix);


#endif // !__BITMATRIXSKELETON_H__