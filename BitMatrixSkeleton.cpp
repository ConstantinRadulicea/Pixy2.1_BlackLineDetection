#ifndef __THINNING_H__
#define __THINNING_H__


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
#include "BitMatrixSkeleton.h"


/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * Parameters:
 * 		im    Binary image with range = [0,1]
 * 		iter  0=even, 1=odd
 */
void BitMatrixSkeletonIteration(BitMatrix& img, BitMatrix& marker, int iter, int nRows, int rowOffset, int nCols, int colOffset)
{
    if (img.countNonZero() <= 0) {
        return;
    }

    //cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);
    //BitMatrix marker(img.getRows(), img.getColumns());
    //marker.setToZeros();

    int x, y;
    PixelCoordinates pAbove;
    PixelCoordinates pCurr;
    PixelCoordinates pBelow;
    PixelCoordinates tempCoord;
    int nw, no, ne;    // north (pAbove)
    int we, me, ea;
    int sw, so, se;    // south (pBelow)

    PixelCoordinates pDst;

    for (y = rowOffset; y < nRows; y++) {
        //// shift the rows up by one

        pAbove.x = colOffset - 1;
        pAbove.y = y - 1;

        //pCurr = img.ptr<uchar>(0);
        pCurr.x = colOffset - 1;
        pCurr.y = y;

        //pBelow = img.ptr<uchar>(1);
        pBelow.x = colOffset - 1;
        pBelow.y = y + 1;

        //pDst = marker.ptr<uchar>(y);
        pDst.x = colOffset - 1;
        pDst.y = y;

        // initialize col pointers

        //no = &(pAbove[0]);
        //ne = &(pAbove[1]);
        //me = &(pCurr[0]);
        //ea = &(pCurr[1]);
        //so = &(pBelow[0]);
        //se = &(pBelow[1]);


        (pAbove.y < rowOffset || pAbove.x < colOffset) ? no = 0 : no = (int)img.getBitXY(pAbove.x, pAbove.y);
        (pAbove.y < rowOffset) ? ne = 0 : ne = (int)img.getBitXY(pAbove.x + 1, pAbove.y);
        (pCurr.x < colOffset) ? me = 0 : me = (int)img.getBitXY(pCurr.x, pCurr.y);
        ea = (int)img.getBitXY(pCurr.x + 1, pCurr.y);
        (pBelow.y >= nRows || pBelow.x < colOffset) ? so = 0 : so = (int)img.getBitXY(pBelow.x, pBelow.y);
        (pBelow.y >= nRows) ? se = 0 : se = (int)img.getBitXY(pBelow.x + 1, pBelow.y);

        for (x = colOffset; x < nCols; x++) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ((x + 1 >= nCols) || pAbove.y >= nRows || pAbove.y < rowOffset) ? ne = 0 : ne = (int)img.getBitXY(x + 1, pAbove.y);
            we = me;
            me = ea;
            (x + 1 >= nCols) ? ea = 0 : ea = (int)img.getBitXY(x + 1, pCurr.y);
            sw = so;
            so = se;
            (pBelow.y >= nRows || (x + 1 >= nCols)) ? se = 0 : se = (int)img.getBitXY(x + 1, pBelow.y);

            int A = (no == 0 && ne == 1) + (ne == 0 && ea == 1) +
                (ea == 0 && se == 1) + (se == 0 && so == 1) +
                (so == 0 && sw == 1) + (sw == 0 && we == 1) +
                (we == 0 && nw == 1) + (nw == 0 && no == 1);
            int B = no + ne + ea + se + so + sw + we + nw;
            int m1 = iter == 0 ? (no * ea * so) : (no * ea * we);
            int m2 = iter == 0 ? (ea * so * we) : (no * so * we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
            {
                //pDst[x] = 1;
                tempCoord = pDst;
                tempCoord.x = x;

                //marker[tempCoord] = true;
                marker.setBitValueXY(tempCoord.x, tempCoord.y, true);
            }
        }
    }

    //img &= ~marker;
    //AandNotB(img, marker);
    BitMatrix::AandNotB(img, marker);
}

void BitMatrixSkeleton(BitMatrix& src, BitMatrix& dst)
{
    if (src.countNonZero() <= 0) {
        return;
    }
    dst = src;

    int nRows = src.maxY();
    int nCols = src.maxX();
    int rowOffset = src.minY();
    int colOffset = src.minX();


    nRows += 1;
    nCols += 1;

    
    BitMatrix prev(src.getRows(), src.getColumns());
    BitMatrix diff(src.getRows(), src.getColumns());
    BitMatrix marker(src.getRows(), src.getColumns());

    do {
        BitMatrixSkeletonIteration(dst, marker, 0, nRows, rowOffset, nCols, colOffset);
        BitMatrixSkeletonIteration(dst, marker, 1, nRows, rowOffset, nCols, colOffset);
        //cv::absdiff(dst, prev, diff);
        //absdiff(dst, prev, diff);
        BitMatrix::absdiff(dst, prev, diff);
        //writeMatlabEdges("edges.csv", mapToVector(dst));
        //dst.copyTo(prev);
        prev = dst;
    }// while (cv::countNonZero(diff) > 0);
    //while (countNonZero(diff) > 0);
    while (diff.countNonZero() > 0);
}


#endif // !__THINNING_H__