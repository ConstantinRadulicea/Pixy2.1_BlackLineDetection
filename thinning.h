#pragma once


/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 *
 * Author:  Nash (nash [at] opencv-code [dot] com)
 * Website: http://opencv-code.com
 */

#include <unordered_map>
#include"Pixy2BlackLineDetectionService.h"
#include "BlackObjectEdgeDetection.h"
#include"ObjectEdges.h"
#include "BitMatrix.h"

using namespace std;




typedef struct ObjectEdgeInfo
{
    size_t minX;
    size_t minY;
    size_t maxX;
    size_t maxY;
};

bool getCoord(std::unordered_map<PixelCoordinates, bool>& map, int x, int y) {
    PixelCoordinates coord;
    coord.x = x;
    coord.y = y;

    auto foundCoord = map.find(coord);

    if (foundCoord != map.end()) {
        return foundCoord->second;
    }
    else
    {
        return false;
    }
}

ObjectEdgeInfo getObjectEdgeInfo(std::unordered_map<PixelCoordinates, bool>& map) {
    ObjectEdgeInfo info;
    memset(&info, 0, sizeof(ObjectEdgeInfo));

    for (auto kv : map) {
        info.maxX = MAX(info.maxX, kv.first.x);
        info.maxY = MAX(info.maxY, kv.first.y);
        info.minX = MIN(info.minX, kv.first.x);
        info.minY = MIN(info.minY, kv.first.y);
    }
    return info;
}

size_t countNonZero(std::unordered_map<PixelCoordinates, bool>& map) {
    size_t count = 0;

    for (auto kv : map) {
        if (kv.second == true) {
            count++;
        }
    }
    return count;
}


/*
 1 - 1 = 0
 1 - 0 = 1
 0 - 1 = 1
 0 - 0 = 0
 dsti = saturate( | src1 - src2 | )
*/
void absdiff(
    std::unordered_map<PixelCoordinates, bool>& src1,                      // First input array or matrix
    std::unordered_map<PixelCoordinates, bool>& src2,                     // Second input array or matrix
    std::unordered_map<PixelCoordinates, bool>& dst                    // Result array or matrix
) {
    PixelCoordinates coordSrc1;
    bool valueDst;

    dst.clear();

    for (auto coordSrc1 : src1) {
        valueDst = coordSrc1.second;
        auto coordSrc2_found = src2.find(coordSrc1.first);

        if (coordSrc2_found != src2.end()) {
            if (coordSrc1.second != coordSrc2_found->second)
            {
                valueDst = true;
            }
            else if (coordSrc1.second == 1)		// it should add the 2 values but we work with bool so we cant add
            {
                valueDst = false;
            }
            else
            {
                valueDst = false;
            }
        }
        if (valueDst)
        {
            dst[coordSrc1.first] = valueDst;
        }
    }

    for (auto coordSrc2 : src2) {
        auto src1_not_found = src1.find(coordSrc2.first);

        if (src1_not_found == src1.end()) {
            dst[coordSrc2.first] = coordSrc2.second;
        }
    }

}

void removeZeros(std::unordered_map<PixelCoordinates, bool>& map) {
    for (auto it = map.begin(); it != map.end();)
    {
        if (it->second == false)
        {
            it = map.erase(it);
        }
        else {
            ++it;
        }
    }
}

/*
1 & ~(1) = 1 & 0 = 0;
1 & ~(0) = 1 & 1 = 1;
0 & ~(1) = 0 & 0 = 0;
0 & ~(0) = 0 & 1 = 0;
*/
void AandNotB(std::unordered_map<PixelCoordinates, bool>& A, std::unordered_map<PixelCoordinates, bool>& B) {

    for (auto coordA : A) {
        auto coordB_found = B.find(coordA.first);

        if (coordB_found != B.end()) {		// if found A in B
            A[coordA.first] &= !(coordB_found->second);
        }
        else
        {
            A[coordA.first] &= !(false);
        }
    }

    removeZeros(A);
}





 /**
  * Perform one thinning iteration.
  * Normally you wouldn't call this function directly from your code.
  *
  * Parameters:
  * 		im    Binary image with range = [0,1]
  * 		iter  0=even, 1=odd
  */
void thinningIteration(BitMatrix& img, int iter)
{
    
    //ObjectEdgeInfo imgInfo;
    //imgInfo = getObjectEdgeInfo(img);
    //int nRows = imgInfo.maxY+1;
    //int nCols = imgInfo.maxX+1;
    //int rowOffset = imgInfo.minY;
    //int colOffset = imgInfo.minX;


    int nRows = img.maxY() + 1;
    int nCols = img.maxX() + 1;
    int rowOffset = img.minY();
    int colOffset = img.minX();

    if (!(nRows > 3 && nCols > 3)) return;

    //cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);
    BitMatrix marker(img.getRows(), img.getColumns());

    int x, y;
    PixelCoordinates pAbove;
    PixelCoordinates pCurr;
    PixelCoordinates pBelow;
    PixelCoordinates tempCoord;
    int nw, no, ne;    // north (pAbove)
    int we, me, ea;
    int sw, so, se;    // south (pBelow)

    PixelCoordinates pDst;

    // initialize row pointers
    //pAbove = NULL;
    pAbove.x = -1;
    pAbove.y = -1;

    //pCurr = img.ptr<uchar>(0);
    pCurr.x = colOffset;
    pCurr.y = rowOffset;

    //pBelow = img.ptr<uchar>(1);
    pBelow.x = colOffset;
    pBelow.y = rowOffset+1;

    for (y = rowOffset+1; y < rowOffset + nRows; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr = pBelow;

        //pBelow = img.ptr<uchar>(y + 1);
        pBelow.x = colOffset;
        pBelow.y = y + 1;

        
        //pDst = marker.ptr<uchar>(y);
        pDst.x = colOffset;
        pDst.y = y;

        // initialize col pointers

        //no = &(pAbove[0]);
        //ne = &(pAbove[1]);
        //me = &(pCurr[0]);
        //ea = &(pCurr[1]);
        //so = &(pBelow[0]);
        //se = &(pBelow[1]);

        //no = (int)getCoord(img, pAbove.x, pAbove.y);
        //ne = (int)getCoord(img, pAbove.x + 1, pAbove.y);
        //me = (int)getCoord(img, pCurr.x, pCurr.y);
        //ea = (int)getCoord(img, pCurr.x + 1, pCurr.y);
        //so = (int)getCoord(img, pBelow.x, pBelow.y);
        //se = (int)getCoord(img, pBelow.x + 1, pBelow.y);

        no = (int)img.getBitXY(pAbove.x, pAbove.y);
        ne = (int)img.getBitXY(pAbove.x + 1, pAbove.y);
        me = (int)img.getBitXY(pCurr.x, pCurr.y);
        ea = (int)img.getBitXY(pCurr.x + 1, pCurr.y);
        so = (int)img.getBitXY(pBelow.x, pBelow.y);
        se = (int)img.getBitXY(pBelow.x + 1, pBelow.y);
        

        for (x = colOffset + 1; x < colOffset + nCols; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            //ne = &(pAbove[x + 1]);
            //ne = (int)getCoord(img, x + 1, pAbove.y);
            ne = (int)img.getBitXY(x + 1, pAbove.y);
            we = me;
            me = ea;
            //ea = &(pCurr[x + 1]);
            //ea = (int)getCoord(img, x + 1, pCurr.y);
            ea = (int)img.getBitXY(x + 1, pCurr.y);

            sw = so;
            so = se;
            //se = &(pBelow[x + 1]);
            //se = (int)getCoord(img, x + 1, pBelow.y);
            se = (int)img.getBitXY(x + 1, pBelow.y);

            //int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
            //    (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
            //    (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
            //    (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            //int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            //int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            //int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);


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

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,255]
 * 		dst  The destination image
 */
static void thinning(BitMatrix& src, BitMatrix& dst)
{
    dst = src;
    //dst /= 255;         // convert to binary image

    //cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    BitMatrix prev(src.getRows(), src.getColumns());
    //cv::Mat diff;
    BitMatrix diff(src.getRows(), src.getColumns());

    do {
        thinningIteration(dst, 0);
        thinningIteration(dst, 1);
        //cv::absdiff(dst, prev, diff);
        //absdiff(dst, prev, diff);
        BitMatrix::absdiff(dst, prev, diff);
        //writeMatlabEdges("edges.csv", mapToVector(dst));
        //dst.copyTo(prev);
        prev = dst;
    }// while (cv::countNonZero(diff) > 0);
    //while (countNonZero(diff) > 0);
    while (diff.countNonZero() > 0);

    //dst *= 255;
}

/**
 * This is an example on how to call the thinning funciton above
 */
/*
int main()
{
    cv::Mat src = cv::imread("image.png");
    if (!src.data)
        return -1;

    cv::Mat bw;
    cv::cvtColor(src, bw, CV_BGR2GRAY);
    cv::threshold(bw, bw, 10, 255, CV_THRESH_BINARY);

    thinning(bw, bw);

    cv::imshow("src", src);
    cv::imshow("dst", bw);
    cv::waitKey();
    return 0;
}
*/
