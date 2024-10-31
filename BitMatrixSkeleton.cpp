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
void BitMatrixSkeletonIteration(BitMatrix* img, BitMatrix* marker, int iter, int nRows, int rowOffset, int nCols, int colOffset)
{
    if (img->countNonZero() <= 0) {
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


        (pAbove.y < rowOffset || pAbove.x < colOffset) ? no = 0 : no = (int)img->getBitXY(pAbove.x, pAbove.y);
        (pAbove.y < rowOffset) ? ne = 0 : ne = (int)img->getBitXY(pAbove.x + 1, pAbove.y);
        (pCurr.x < colOffset) ? me = 0 : me = (int)img->getBitXY(pCurr.x, pCurr.y);
        ea = (int)img->getBitXY(pCurr.x + 1, pCurr.y);
        (pBelow.y >= nRows || pBelow.x < colOffset) ? so = 0 : so = (int)img->getBitXY(pBelow.x, pBelow.y);
        (pBelow.y >= nRows) ? se = 0 : se = (int)img->getBitXY(pBelow.x + 1, pBelow.y);

        for (x = colOffset; x < nCols; x++) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ((x + 1 >= nCols) || pAbove.y >= nRows || pAbove.y < rowOffset) ? ne = 0 : ne = (int)img->getBitXY(x + 1, pAbove.y);
            we = me;
            me = ea;
            (x + 1 >= nCols) ? ea = 0 : ea = (int)img->getBitXY(x + 1, pCurr.y);
            sw = so;
            so = se;
            (pBelow.y >= nRows || (x + 1 >= nCols)) ? se = 0 : se = (int)img->getBitXY(x + 1, pBelow.y);

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
                marker->setBitValueXY(tempCoord.x, tempCoord.y, true);
            }
        }
    }

    //img &= ~marker;
    //AandNotB(img, marker);
    BitMatrix::AandNotB(img, marker);
}

void BitMatrixSkeleton(BitMatrix* src, BitMatrix* dst)
{
    if (src->countNonZero() <= 0) {
        return;
    }
    *dst = *src;

    int nRows = src->maxY();
    int nCols = src->maxX();
    int rowOffset = src->minY();
    int colOffset = src->minX();


    nRows += 1;
    nCols += 1;


    BitMatrix prev(src->getRows(), src->getColumns());
    BitMatrix diff(src->getRows(), src->getColumns());
    BitMatrix marker(src->getRows(), src->getColumns());

    do {
        BitMatrixSkeletonIteration(dst, &marker, 0, nRows, rowOffset, nCols, colOffset);
        BitMatrixSkeletonIteration(dst, &marker, 1, nRows, rowOffset, nCols, colOffset);
        //cv::absdiff(dst, prev, diff);
        //absdiff(dst, prev, diff);
        BitMatrix::absdiff(dst, &prev, &diff);
        //writeMatlabEdges("edges.csv", mapToVector(dst));
        //dst.copyTo(prev);
        prev = *dst;
    }// while (cv::countNonZero(diff) > 0);
    //while (countNonZero(diff) > 0);
    while (diff.countNonZero() > 0);
}


void BitMatrixSkeleton(BitMatrix* matrix)
{
    if (matrix->countNonZero() <= 0) {
        return;
    }

    int nRows = matrix->maxY();
    int nCols = matrix->maxX();
    int rowOffset = matrix->minY();
    int colOffset = matrix->minX();


    nRows += 1;
    nCols += 1;


    BitMatrix prev(matrix->getRows(), matrix->getColumns());
    BitMatrix diff(matrix->getRows(), matrix->getColumns());
    BitMatrix marker(matrix->getRows(), matrix->getColumns());

    do {
        BitMatrixSkeletonIteration(matrix, &marker, 0, nRows, rowOffset, nCols, colOffset);
        BitMatrixSkeletonIteration(matrix, &marker, 1, nRows, rowOffset, nCols, colOffset);
        //cv::absdiff(dst, prev, diff);
        //absdiff(dst, prev, diff);
        BitMatrix::absdiff(matrix, &prev, &diff);
        //writeMatlabEdges("edges.csv", mapToVector(dst));
        //dst.copyTo(prev);
        prev = *matrix;
    }// while (cv::countNonZero(diff) > 0);
    //while (countNonZero(diff) > 0);
    while (diff.countNonZero() > 0);
}



/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
struct mat_9x9 {
    bool P1, P2, P3, P4, P5, P6, P7, P8, P9;
};


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
inline int getWhiteBlackTransitions(struct mat_9x9* window) {
    return
        (int)(window->P2 == false && window->P3 == true) +
        (int)(window->P3 == false && window->P4 == true) +
        (int)(window->P4 == false && window->P5 == true) +
        (int)(window->P5 == false && window->P6 == true) +
        (int)(window->P6 == false && window->P7 == true) +
        (int)(window->P7 == false && window->P8 == true) +
        (int)(window->P8 == false && window->P9 == true) +
        (int)(window->P9 == false && window->P2 == true);
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edge_top_left(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = 0;
    col = 0;
    window.P2 = false;
    window.P3 = false;
    window.P7 = false;
    window.P8 = false;
    window.P9 = false;
    for (col; col < 1; col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P4 = img->getBit(row, col + 1);

        if (iter == 0) {
            window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P4 && window.P6) {
                continue;
            }

            if (window.P4 && window.P6 && window.P8) {
                continue;
            }
        }
        else if (iter == 1) {

            if (window.P2 && window.P4 && window.P8) {
                continue;
            }

            window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P6 && window.P8) {
                continue;
            }
        }

        window.P5 = img->getBit(row + 1, col + 1);

        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}

/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edge_top_right(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = 0;
    col = img->getColumns() - 1;
    window.P2 = false;
    window.P3 = false;
    window.P4 = false;
    window.P5 = false;
    window.P9 = false;
    for (col; col < img->getColumns(); col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        //window.P4 = img->getBit(row, col + 1);

        if (iter == 0) {
            window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P4 && window.P6) {
                continue;
            }
            window.P8 = img->getBit(row, col - 1);
            if (window.P4 && window.P6 && window.P8) {
                continue;
            }
        }
        else if (iter == 1) {
            window.P8 = img->getBit(row, col - 1);
            if (window.P2 && window.P4 && window.P8) {
                continue;
            }

            window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P6 && window.P8) {
                continue;
            }
        }

        //window.P5 = img->getBit(row + 1, col + 1);
        window.P7 = img->getBit(row + 1, col - 1);

        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}

/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edge_bottom_left(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = img->getRows()-1;
    col = 0;
    //window.P2 = false;
    //window.P3 = false;
    //window.P4 = false;

    window.P5 = false;
    window.P9 = false;
    window.P6 = false;
    window.P7 = false;
    window.P8 = false;
    for (col; col < 1; col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P2 = img->getBit(row - 1, col);
        window.P4 = img->getBit(row, col + 1);

        if (iter == 0) {
            //window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P4 && window.P6) {
                continue;
            }
            //window.P8 = img->getBit(row, col - 1);
            if (window.P4 && window.P6 && window.P8) {
                continue;
            }
        }
        else if (iter == 1) {
            //window.P8 = img->getBit(row, col - 1);
            if (window.P2 && window.P4 && window.P8) {
                continue;
            }

            //window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P6 && window.P8) {
                continue;
            }
        }

        window.P3 = img->getBit(row - 1, col + 1);
        //window.P5 = img->getBit(row + 1, col + 1);
        //window.P7 = img->getBit(row + 1, col - 1);
        //window.P9 = img->getBit(row - 1, col - 1);

        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edge_bottom_right(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = img->getRows() - 1;
    col = img->getColumns() - 1;;
    //window.P2 = false;
    window.P3 = false;
    window.P4 = false;
    window.P5 = false;
    window.P6 = false;
    window.P7 = false;
    //window.P8 = false;
    //window.P9 = false;

    for (col; col < img->getColumns(); col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P2 = img->getBit(row - 1, col);
        //window.P4 = img->getBit(row, col + 1);

        if (iter == 0) {
            //window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P4 && window.P6) {
                continue;
            }
            window.P8 = img->getBit(row, col - 1);
            if (window.P4 && window.P6 && window.P8) {
                continue;
            }
        }
        else if (iter == 1) {
            window.P8 = img->getBit(row, col - 1);
            if (window.P2 && window.P4 && window.P8) {
                continue;
            }

            //window.P6 = img->getBit(row + 1, col);
            if (window.P2 && window.P6 && window.P8) {
                continue;
            }
        }

        //window.P3 = img->getBit(row - 1, col + 1);
        //window.P5 = img->getBit(row + 1, col + 1);
        //window.P7 = img->getBit(row + 1, col - 1);
        window.P9 = img->getBit(row - 1, col - 1);

        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}

/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edges_top(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = 0;
    col = 1;

    window.P2 = false;
    window.P3 = false;
    //window.P4 = false;
    //window.P5 = false;
    //window.P6 = false;
    //window.P7 = false;
    //window.P8 = false;
    window.P9 = false;

    for (; col < img->getColumns() - 1; col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        //window.P2 = img->getBit(row - 1, col);
        window.P4 = img->getBit(row, col + 1);


        if (iter == 0) {
            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P4 && window.P6)) {
                continue;
            }

            window.P8 = img->getBit(row, col - 1);
            if ((window.P4 && window.P6 && window.P8)) {
                continue;
            }
        }
        else if (iter == 1) {
            window.P8 = img->getBit(row, col - 1);
            if ((window.P2 && window.P4 && window.P8)) {
                continue;
            }

            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P6 && window.P8)) {
                continue;
            }
        }

        //window.P3 = img->getBit(row - 1, col + 1);
        window.P5 = img->getBit(row + 1, col + 1);
        window.P7 = img->getBit(row + 1, col - 1);
        //window.P9 = img->getBit(row - 1, col - 1);



        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edges_bottom(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = img->getRows() - 1;
    col = 1;

    //window.P2 = false;
    //window.P3 = false;
    //window.P4 = false;
    window.P5 = false;
    window.P6 = false;
    window.P7 = false;
    //window.P8 = false;
    //window.P9 = false;

    for (; col < img->getColumns() - 1; col++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P2 = img->getBit(row - 1, col);
        window.P4 = img->getBit(row, col + 1);


        if (iter == 0) {
            //window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P4 && window.P6)) {
                continue;
            }

            window.P8 = img->getBit(row, col - 1);
            if ((window.P4 && window.P6 && window.P8)) {
                continue;
            }
        }
        else if (iter == 1) {
            window.P8 = img->getBit(row, col - 1);
            if ((window.P2 && window.P4 && window.P8)) {
                continue;
            }

            //window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P6 && window.P8)) {
                continue;
            }
        }

        window.P3 = img->getBit(row - 1, col + 1);
        //window.P5 = img->getBit(row + 1, col + 1);
        //window.P7 = img->getBit(row + 1, col - 1);
        window.P9 = img->getBit(row - 1, col - 1);



        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edges_left(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = 1;
    col = 0;

    //window.P2 = false;
    //window.P3 = false;
    //window.P4 = false;
    //window.P5 = false;
    //window.P6 = false;
    window.P7 = false;
    window.P8 = false;
    window.P9 = false;

    for (; row < img->getRows() - 1; row++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P2 = img->getBit(row - 1, col);
        window.P4 = img->getBit(row, col + 1);


        if (iter == 0) {
            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P4 && window.P6)) {
                continue;
            }

            //window.P8 = img->getBit(row, col - 1);
            if ((window.P4 && window.P6 && window.P8)) {
                continue;
            }
        }
        else if (iter == 1) {
            //window.P8 = img->getBit(row, col - 1);
            if ((window.P2 && window.P4 && window.P8)) {
                continue;
            }

            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P6 && window.P8)) {
                continue;
            }
        }

        window.P3 = img->getBit(row - 1, col + 1);
        window.P5 = img->getBit(row + 1, col + 1);
        //window.P7 = img->getBit(row + 1, col - 1);
        //window.P9 = img->getBit(row - 1, col - 1);



        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_edges_right(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t row, col;
    int bit_sum;

    bits_deleted = 0;

    row = 1;
    col = img->getColumns()-1;

    //window.P2 = false;
    window.P3 = false;
    window.P4 = false;
    window.P5 = false;
    //window.P6 = false;
    //window.P7 = false;
    //window.P8 = false;
    //window.P9 = false;

    for (; row < img->getRows() - 1; row++) {
        window.P1 = img->getBit(row, col);
        if (window.P1 == false) {
            continue;
        }

        window.P2 = img->getBit(row - 1, col);
        //window.P4 = img->getBit(row, col + 1);


        if (iter == 0) {
            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P4 && window.P6)) {
                continue;
            }

            window.P8 = img->getBit(row, col - 1);
            if ((window.P4 && window.P6 && window.P8)) {
                continue;
            }
        }
        else if (iter == 1) {
            window.P8 = img->getBit(row, col - 1);
            if ((window.P2 && window.P4 && window.P8)) {
                continue;
            }

            window.P6 = img->getBit(row + 1, col);
            if ((window.P2 && window.P6 && window.P8)) {
                continue;
            }
        }

        //window.P3 = img->getBit(row - 1, col + 1);
        //window.P5 = img->getBit(row + 1, col + 1);
        window.P7 = img->getBit(row + 1, col - 1);
        window.P9 = img->getBit(row - 1, col - 1);



        bit_sum =
            (int)window.P2 +
            (int)window.P3 +
            (int)window.P4 +
            (int)window.P5 +
            (int)window.P6 +
            (int)window.P7 +
            (int)window.P8 +
            (int)window.P9;

        if (!((2 <= bit_sum) && (bit_sum <= 6))) {
            continue;

        }
        if (getWhiteBlackTransitions(&window) == 1) {
            marker->setBit(row, col);
            bits_deleted++;
        }
    }
    return bits_deleted;
}


/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t n_rows, n_cols;
    int bit_sum;

    bits_deleted = 0;
    n_rows = img->getRows() - 1;
    n_cols = img->getColumns() - 1;

    for (size_t row = 1; row < n_rows; row++) {
        for (size_t col = 1; col < n_cols; col++) {
            window.P1 = img->getBit(row, col);
            if (window.P1 == false) {
                continue;
            }

            window.P2 = img->getBit(row - 1, col);
            window.P4 = img->getBit(row, col + 1);


            if (iter == 0) {
                window.P6 = img->getBit(row + 1, col);
                if ((window.P2 && window.P4 && window.P6)) {
                    continue;
                }

                window.P8 = img->getBit(row, col - 1);
                if ((window.P4 && window.P6 && window.P8)) {
                    continue;
                }
            }
            else if (iter == 1) {
                window.P8 = img->getBit(row, col - 1);
                if ((window.P2 && window.P4 && window.P8)) {
                    continue;
                }

                window.P6 = img->getBit(row + 1, col);
                if ((window.P2 && window.P6 && window.P8)) {
                    continue;
                }
            }

            window.P3 = img->getBit(row - 1, col + 1);
            window.P5 = img->getBit(row + 1, col + 1);
            window.P7 = img->getBit(row + 1, col - 1);
            window.P9 = img->getBit(row - 1, col - 1);



            bit_sum =
                (int)window.P2 +
                (int)window.P3 +
                (int)window.P4 +
                (int)window.P5 +
                (int)window.P6 +
                (int)window.P7 +
                (int)window.P8 +
                (int)window.P9;

            if (!((2 <= bit_sum) && (bit_sum <= 6))) {
                continue;

            }
            if (getWhiteBlackTransitions(&window) == 1) {
                marker->setBit(row, col);
                bits_deleted++;
            }
        }
    }
    return bits_deleted;
}



/*
P9  P2  P3
P8  P1  P4
P7  P6  P5
*/
int iteration1_2(BitMatrix* img, BitMatrix* marker, int iter) {
    struct mat_9x9 window;
    size_t bits_deleted;
    size_t n_rows, n_cols;
    int bit_sum;
    BitMatrixPosition pos;
    size_t row, col;

    bits_deleted = 0;
    n_rows = img->getRows() - 1;
    n_cols = img->getColumns() - 1;

    row = 1;
    col = 0;
    //for (size_t row = 1; row < n_rows; row++) {
    //    for (size_t col = 1; col < n_cols; col++) {
    window.P1 = true;
        while (true) {
            //row += 1;
            col += 1;
            pos = img->getNextSetPixel(row, col);
            if (pos.valid == false) {
                break;
            }
            row = pos.row;
            col = pos.column;

            if ((row < 1 || row >= n_rows || col < 1 || row >= n_cols)) {
                continue;
            }
            //window.P1 = img->getBit(row, col);
            //if (window.P1 == false) {
            //    continue;
            //}

            

            window.P2 = img->getBit(row - 1, col);
            window.P4 = img->getBit(row, col + 1);


            if (iter == 0) {
                window.P6 = img->getBit(row + 1, col);
                if ((window.P2 && window.P4 && window.P6)) {
                    continue;
                }

                window.P8 = img->getBit(row, col - 1);
                if ((window.P4 && window.P6 && window.P8)) {
                    continue;
                }
            }
            else if (iter == 1) {
                window.P8 = img->getBit(row, col - 1);
                if ((window.P2 && window.P4 && window.P8)) {
                    continue;
                }

                window.P6 = img->getBit(row + 1, col);
                if ((window.P2 && window.P6 && window.P8)) {
                    continue;
                }
            }

            window.P3 = img->getBit(row - 1, col + 1);
            window.P5 = img->getBit(row + 1, col + 1);
            window.P7 = img->getBit(row + 1, col - 1);
            window.P9 = img->getBit(row - 1, col - 1);



            bit_sum =
                (int)window.P2 +
                (int)window.P3 +
                (int)window.P4 +
                (int)window.P5 +
                (int)window.P6 +
                (int)window.P7 +
                (int)window.P8 +
                (int)window.P9;

            if (!((2 <= bit_sum) && (bit_sum <= 6))) {
                continue;

            }
            if (getWhiteBlackTransitions(&window) == 1) {
                marker->setBit(row, col);
                bits_deleted++;
            }
            //    }
            //}
        }
    return bits_deleted;
}




// https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm#C
void BitMatrixSkeletonZS(BitMatrix* matrix, BitMatrix *marker) {
    size_t bits_deleted = 0;
    if (matrix->countNonZero() <= 0) {
        return;
    }
    marker->clear();

    bits_deleted = iteration1_edge_top_left(matrix, marker, 0);
    bits_deleted += iteration1_edge_top_right(matrix, marker, 0);
    bits_deleted += iteration1_edge_bottom_left(matrix, marker, 0);
    bits_deleted += iteration1_edge_bottom_right(matrix, marker, 0);
    bits_deleted += iteration1_edges_top(matrix, marker, 0);
    bits_deleted += iteration1_edges_bottom(matrix, marker, 0);
    bits_deleted += iteration1_edges_left(matrix, marker, 0);
    bits_deleted += iteration1_edges_right(matrix, marker, 0);
    bits_deleted += iteration1_2(matrix, marker, 0);
    BitMatrix::AandNotB(matrix, marker);

    bits_deleted += iteration1_edge_top_left(matrix, marker, 1);
    bits_deleted += iteration1_edge_top_right(matrix, marker, 1);
    bits_deleted += iteration1_edge_bottom_left(matrix, marker, 1);
    bits_deleted += iteration1_edge_bottom_right(matrix, marker, 1);
    bits_deleted += iteration1_edges_top(matrix, marker, 1);
    bits_deleted += iteration1_edges_bottom(matrix, marker, 1);
    bits_deleted += iteration1_edges_left(matrix, marker, 1);
    bits_deleted += iteration1_edges_right(matrix, marker, 1);
    bits_deleted += iteration1_2(matrix, marker, 1);
    BitMatrix::AandNotB(matrix, marker);


    while (bits_deleted > 0) {
        bits_deleted = iteration1_2(matrix, marker, 0);
        BitMatrix::AandNotB(matrix, marker);
        bits_deleted += iteration1_2(matrix, marker, 1);
        BitMatrix::AandNotB(matrix, marker);
    }
}


// https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm#C
void BitMatrixSkeletonZS(BitMatrix* matrix) {
    BitMatrix marker(matrix->getRows(), matrix->getColumns());
    BitMatrixSkeletonZS(matrix, &marker);
}

#endif // !__THINNING_H__