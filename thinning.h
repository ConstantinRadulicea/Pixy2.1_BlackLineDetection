#pragma once


/**
 * Code for thinning a binary image using Zhang-Suen algorithm.
 *
 * Author:  Nash (nash [at] opencv-code [dot] com)
 * Website: http://opencv-code.com
 */

#include <unordered_map>
#include"Pixy2BlackLineDetectionService.h"
#include"ObjectEdges.h"

using namespace std;




typedef struct ObjectEdgeInfo
{
    size_t minX;
    size_t minY;
    size_t maxX;
    size_t maxY;
};

static ObjectEdgeInfo getObjectEdgeInfo(std::unordered_map<PixelCoordinates, bool>& map) {
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

static size_t countNonZero(std::unordered_map<PixelCoordinates, bool>& map) {
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
static void absdiff(
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
            dst[coordSrc1.first] = valueDst;
        }
    }

    for (auto coordSrc2 : src2) {
        auto dst_not_found = dst.find(coordSrc2.first);

        if (dst_not_found == dst.end()) {
            dst[dst_not_found->first] = dst_not_found->second;
        }
    }

}

static void removeZeros(std::unordered_map<PixelCoordinates, bool>& map) {
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
static void AandNotB(std::unordered_map<PixelCoordinates, bool>& A, std::unordered_map<PixelCoordinates, bool>& B) {

    for (auto coordA : A) {
        auto coordB_found = B.find(coordA.first);

        if (coordB_found != B.end()) {		// if found A in B
            A[coordA.first] &= !(coordB_found->second);
        }

        if (A[coordA.first] == false)
        {
            A.erase(coordA.first);
        }
    }
}





 /**
  * Perform one thinning iteration.
  * Normally you wouldn't call this function directly from your code.
  *
  * Parameters:
  * 		im    Binary image with range = [0,1]
  * 		iter  0=even, 1=odd
  */
static void thinningIteration(std::unordered_map<PixelCoordinates, bool>& img, int iter)
{
    
    ObjectEdgeInfo imgInfo;
    imgInfo = getObjectEdgeInfo(img);
    int nRows = imgInfo.maxY;
    int nCols = imgInfo.maxX;

    if (!(nRows > 3 && nCols > 3)) return;

    //cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);
    std::unordered_map<PixelCoordinates, bool> marker;

    int x, y;
    uchar* pAbove;
    uchar* pCurr;
    uchar* pBelow;
    uchar* nw, * no, * ne;    // north (pAbove)
    uchar* we, * me, * ea;
    uchar* sw, * so, * se;    // south (pBelow)

    uchar* pDst;

    // initialize row pointers
    pAbove = NULL;
    pCurr = img.ptr<uchar>(0);
    pBelow = img.ptr<uchar>(1);

    for (y = 1; y < img.rows - 1; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr = pBelow;
        pBelow = img.ptr<uchar>(y + 1);

        pDst = marker.ptr<uchar>(y);

        // initialize col pointers
        no = &(pAbove[0]);
        ne = &(pAbove[1]);
        me = &(pCurr[0]);
        ea = &(pCurr[1]);
        so = &(pBelow[0]);
        se = &(pBelow[1]);

        for (x = 1; x < img.cols - 1; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ne = &(pAbove[x + 1]);
            we = me;
            me = ea;
            ea = &(pCurr[x + 1]);
            sw = so;
            so = se;
            se = &(pBelow[x + 1]);

            int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
                (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
                (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
                (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                pDst[x] = 1;
        }
    }
    
    //img &= ~marker;
    AandNotB(img, marker);
}

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,255]
 * 		dst  The destination image
 */
void thinning(std::unordered_map<PixelCoordinates, bool>& src, std::unordered_map<PixelCoordinates, bool>& dst)
{
    dst = src;
    //dst /= 255;         // convert to binary image

    //cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    std::unordered_map<PixelCoordinates, bool> prev;
    //cv::Mat diff;
    std::unordered_map<PixelCoordinates, bool> diff;

    do {
        thinningIteration(dst, 0);
        thinningIteration(dst, 1);
        //cv::absdiff(dst, prev, diff);
        absdiff(dst, prev, diff);
        //dst.copyTo(prev);
        prev = dst;
    }// while (cv::countNonZero(diff) > 0);
    while (countNonZero(diff) > 0);

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
