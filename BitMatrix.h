#ifndef __BITMATRIX_H__
#define __BITMATRIX_H__



#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "geometry2D.h"
#include "bits.h"


typedef struct Point2D_Distance {
	Point2D_int point;
	float distance;
}Point2D_Distance;

typedef struct BitMatrixPosition {
	size_t row;
	size_t column;
	bool valid;
}BitMatrixPosition;

typedef struct Point2D_int16_t {
	int16_t x;
	int16_t y;
}Point2D_int16_t;



class BitMatrix;
// return true if the bit is accepted, otherwise returns false
typedef bool (*BitMatrixFillFilter)(size_t row, size_t col, BitMatrix* bit_matrix, void* _Context);
//
//#define BITARRAY_DATATYPE unsigned long long int					//unsigned long long int //unsigned char //unsigned int
//#define BITARRAY_DATATYPE_MAX_VALUE	ULLONG_MAX					//ULLONG_MAX //UCHAR_MAX //UINT_MAX
//#define BITARRAY_DATATYPE_BITS	(sizeof(BITARRAY_DATATYPE) * 8)

#define BITARRAY_DATATYPE unsigned int					//unsigned long long int //unsigned char //unsigned int
#define BITARRAY_DATATYPE_MAX_VALUE	UINT_MAX					//ULLONG_MAX //UCHAR_MAX //UINT_MAX
#define BITARRAY_DATATYPE_BITS	(sizeof(BITARRAY_DATATYPE) * 8)


class BitMatrix
{
public:
	BitMatrix(size_t nRows, size_t nColumns) {
		this->init(nRows, nColumns);
	}
	BitMatrix() {
	}

	void init(size_t nRows, size_t nColumns) {
		this->nRows = nRows;
		this->nColumns = nColumns;
		this->clear();
	}

	inline size_t getRows() {
		return this->nRows;
	}

	inline size_t getColumns() {
		return this->nColumns;
	}

	inline size_t totBlocks() {
		return data.size();
	}

	inline size_t bitSize() {
		return this->nRows * this->nColumns;
	}

	void clear() {
		size_t arrSize = 0;
		this->data.clear();

		if (this->bitSize() % BITARRAY_DATATYPE_BITS) {
			arrSize = ((nRows * nColumns) / BITARRAY_DATATYPE_BITS) + 1;
		}
		else {
			arrSize = ((nRows * nColumns) / BITARRAY_DATATYPE_BITS);
		}
		data.reserve(arrSize);
		data.resize(arrSize);
		this->setToZeros();
		this->settedBits = 0;
	}

	void free() {
		this->nColumns = 0;
		this->nRows = 0;
		this->settedBits = 0;

		this->data.resize(0);
		this->data.reserve(0);
		this->data.shrink_to_fit();
	}

	inline bool getBit(register size_t row, register size_t col) {
		register size_t offset = (row * this->nColumns) + col;
		return 1 & (this->data[offset / BITARRAY_DATATYPE_BITS] >> (offset % BITARRAY_DATATYPE_BITS));
	}
	
	inline void setBitValue(size_t row, size_t col, bool value) {
		if (value) {
			this->setBit(row, col);
		}
		else {
			this->unsetBit(row, col);
		}
	}

	inline void setBitValueXY(size_t x, size_t y, bool value) {
		this->setBitValue(y, x, value);
	}
	inline bool getBitXY(size_t x, size_t y) {
		return this->getBit(y, x);
	}

	inline void setBit(register size_t row, register size_t col) {
		if (this->getBit(row, col) == true) {	// bit already setted
			return;
		}
		size_t offset = (row * this->nColumns) + col;
		size_t index = offset / BITARRAY_DATATYPE_BITS;
		this->data[index] = (BITARRAY_DATATYPE)(this->data[index]) | (BITARRAY_DATATYPE)((BITARRAY_DATATYPE)1 << (offset % (size_t)BITARRAY_DATATYPE_BITS));
		this->settedBits++;
	}

	inline void setBitXY(size_t x, size_t y) {
		this->setBit(y, x);
	}

	inline void unsetBit(size_t row, size_t col) {
		size_t offset;
		BITARRAY_DATATYPE value;
		if (this->getBit(row, col) == false) {	// bit already unsetted
			return;
		}
		offset = (row * this->nColumns) + col;
		value = this->data[offset / BITARRAY_DATATYPE_BITS];
		value = value & ~((BITARRAY_DATATYPE)((BITARRAY_DATATYPE)1 << (offset % BITARRAY_DATATYPE_BITS)));
		this->data[offset / BITARRAY_DATATYPE_BITS] = value;
		this->settedBits--;
	}

	inline void unsetBitXY(size_t x, size_t y) {
		this->unsetBit(y, x);
	}

	inline BITARRAY_DATATYPE getBlockValue(size_t index) {
		if (index == (this->totBlocks()-1))
		{
			return this->data.at(index) & (((BITARRAY_DATATYPE)BITARRAY_DATATYPE_MAX_VALUE) >> ((size_t)BITARRAY_DATATYPE_BITS) - (this->bitSize() % (size_t)BITARRAY_DATATYPE_BITS));
		}
		else {
			return this->data.at(index);
		}
	}

	inline void setBlockValue(size_t index, BITARRAY_DATATYPE value) {
		BITARRAY_DATATYPE oldValue;
		oldValue = this->getBlockValue(index);

		if (oldValue == value) {
			return;
		}
		if (index == (this->totBlocks() - 1)) {
			value &= (((BITARRAY_DATATYPE)BITARRAY_DATATYPE_MAX_VALUE) >> ((size_t)BITARRAY_DATATYPE_BITS) - (this->bitSize() % (size_t)BITARRAY_DATATYPE_BITS));
		}
		
		this->settedBits += ((int)(this->countSettedBits(value)) - (int)(this->countSettedBits(oldValue)));
		this->data[index] = value;
	}

	size_t minX() {
		for (size_t col = 0; col < this->nColumns; col++)
		{
			for (size_t row = 0; row < this->nRows; row++) {
				if (this->getBit(row, col))
				{
					return col;
				}
			}
		}
		return this->nColumns;
	}

	size_t minY() {
		for (size_t row = 0; row < this->nRows; row++) {
			for (size_t col = 0; col < this->nColumns; col++)
			{
				if (this->getBit(row, col))
				{
					return row;
				}
			}
		}
		return this->nRows;
	}

	size_t maxX() {
		for (int col = this->nColumns - 1; col >= 0; col--)
		{
			for (size_t row = 0; row < this->nRows; row++) {
				if (this->getBit(row, col))
				{
					return col;
				}
			}
		}
		return this->nColumns;
	}

	size_t maxY() {
		for (int row = this->nRows - 1; row >= 0; row--) {
			for (size_t col = 0; col < this->nColumns; col++)
			{
				if (this->getBit(row, col))
				{
					return row;
				}
			}
		}
		return this->nRows;
	}

	size_t countNonZero() {
		return this->settedBits;
	}

	inline static void absdiff(BitMatrix& src1, BitMatrix& src2, BitMatrix& dst) {
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < src1.totBlocks(); i++)
		{
			valueSrc1 = src1.getBlockValue(i);
			valueSrc2 = src2.getBlockValue(i);
			newValue = valueSrc1 ^ valueSrc2;
			dst.setBlockValue(i, newValue);
		}
	}

	inline static void absdiff(BitMatrix* src1, BitMatrix* src2, BitMatrix* dst) {
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < src1->totBlocks(); i++)
		{
			valueSrc1 = src1->getBlockValue(i);
			valueSrc2 = src2->getBlockValue(i);
			newValue = valueSrc1 ^ valueSrc2;
			dst->setBlockValue(i, newValue);
		}
	}

	inline static void AandNotB(BitMatrix& dst_A, BitMatrix& B) {
		AandNotB(&dst_A, &B);
	}

	static void AandNotB(BitMatrix* dst_A, BitMatrix* B)
	{
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < dst_A->totBlocks(); i++)
		{
			valueSrc2 = B->getBlockValue(i);
			if (valueSrc2 == 0) {
				continue;
			}
			valueSrc1 = dst_A->getBlockValue(i);
			if (valueSrc1 == 0) {
				continue;
			}
			/*else if (valueSrc2 == BITARRAY_DATATYPE_MAX_VALUE) {
				dst_A->setBlockValue(i, (BITARRAY_DATATYPE)0);
				continue;
			}*/
			
			newValue = valueSrc1 & (~valueSrc2);
			dst_A->setBlockValue(i, newValue);
		}
	}

	inline void logicAnd(BitMatrix& B) {
		BITARRAY_DATATYPE valueSrc, valueB, newValue;

		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc & valueB;
			this->setBlockValue(i, newValue);
		}
	}

	inline void logicOr(BitMatrix& B) {
		BITARRAY_DATATYPE valueSrc, valueB, newValue;

		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc | valueB;
			this->setBlockValue(i, newValue);
		}
	}

	inline void setToZeros() {
		memset(this->data.data(), 0, this->data.size() * sizeof(BITARRAY_DATATYPE));
		//for (size_t i = 0; i < this->totBlocks(); i++)
		//{
		//	this->setBlockValue(i, (BITARRAY_DATATYPE)0);
		//}
		this->settedBits = 0;
		this->getFirstSetPixel_last_index = 0;
	}

	inline void setToOnes() {
		memset(this->data.data(), 255, this->data.size() * sizeof(BITARRAY_DATATYPE));
		//for (size_t i = 0; i < this->totBlocks(); i++)
		//{
		//	this->setBlockValue(i, (BITARRAY_DATATYPE)1);
		//}
		this->settedBits = this->data.size() * sizeof(BITARRAY_DATATYPE) * 8;
	}

	inline bool isInsideBoundaries(size_t row, size_t col) {
		if (row < 0 || row >= this->getRows() || col < 0 || col >= this->getColumns()) {
			return false;
		}
		return true;
	}

	inline BitMatrix floodFill(size_t row, size_t col, BitMatrixFillFilter filter_function, void* _Context) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFill(row, col, filter_function, _Context, &filledZone);
		return filledZone;
	}


	// returns 0 on success
	bool floodFill(size_t row, size_t col, BitMatrixFillFilter filter_function, void* _Context, BitMatrix* filledZone) {
		std::queue<std::pair<int16_t, int16_t>> queue;
		int16_t posRow;
		int16_t posCol;

		filledZone->clear();

		if (!isInsideBoundaries(row, col) || !filter_function(row, col, this, _Context)) {
			return 1;
		}
		filledZone->setBit(row, col);

		// Append the position of starting
		// pixel of the component
		std::pair<int16_t, int16_t> p(row, col);
		std::pair<int16_t, int16_t> currPixel;
		queue.push(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (queue.size() > 0) {
			// Dequeue the front node
			currPixel = queue.front();
			queue.pop();

			posRow = currPixel.first;
			posCol = currPixel.second;

			// Check if the adjacent
			// pixels are valid

			if (isInsideBoundaries((int16_t)(posRow + 1), posCol) && !filledZone->getBit((int16_t)(posRow + 1), posCol) && filter_function((int16_t)(posRow + 1), posCol, this, _Context))
			{
				p.first = (int16_t)(posRow + 1);
				p.second = posCol;
				queue.push(p);
				filledZone->setBit((int16_t)((int16_t)(posRow + 1)), posCol);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), posCol) && !filledZone->getBit((int16_t)(posRow - 1), posCol) && filter_function((int16_t)(posRow - 1), posCol, this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = posCol;
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), posCol);
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol + 1)) && !filledZone->getBit(posRow, (int16_t)(posCol + 1)) && filter_function(posRow, (int16_t)(posCol + 1), this, _Context))
			{
				//screen[posRow][(int16_t)(posCol + 1)] = newC;
				p.first = posRow;
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit(posRow, (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol - 1)) && !filledZone->getBit(posRow, (int16_t)(posCol - 1)) && filter_function(posRow, (int16_t)(posCol - 1), this, _Context))
			{
				p.first = posRow;
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit(posRow, (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.first = (int16_t)(posRow + 1);
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.first = (int16_t)(posRow + 1);
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), (int16_t)(posCol + 1));
			}
		}
	}


	// returns 0 on success
	bool floodFillOnesDelete(size_t row, size_t col, BitMatrix* filledZone) {
		std::queue<Point2D_int16_t> queue;
		int16_t posRow;
		int16_t posCol;
		BitMatrixFillFilter filter_function = BitMatrix::floodFillFilterFunctionOnes;
		void* _Context = NULL;

		filledZone->clear();

		if (!isInsideBoundaries(row, col) || !filter_function(row, col, this, _Context)) {
			return 1;
		}
		filledZone->setBit(row, col);
		this->unsetBit(row, col);

		// Append the position of starting
		// pixel of the component
		Point2D_int16_t p = Point2D_int16_t{ (int16_t)row, (int16_t)col };
		Point2D_int16_t currPixel;
		queue.push(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (queue.size() > 0) {
			// Dequeue the front node
			currPixel = queue.front();
			queue.pop();

			posRow = currPixel.x;
			posCol = currPixel.y;

			// Check if the adjacent
			// pixels are valid

			if (isInsideBoundaries((int16_t)(posRow + 1), posCol) && !filledZone->getBit((int16_t)(posRow + 1), posCol) && filter_function((int16_t)(posRow + 1), posCol, this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = posCol;
				queue.push(p);
				filledZone->setBit((int16_t)((int16_t)(posRow + 1)), posCol);
				this->unsetBit((int16_t)((int16_t)(posRow + 1)), posCol);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), posCol) && !filledZone->getBit((int16_t)(posRow - 1), posCol) && filter_function((int16_t)(posRow - 1), posCol, this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = posCol;
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), posCol);
				this->unsetBit((int16_t)(posRow - 1), posCol);
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol + 1)) && !filledZone->getBit(posRow, (int16_t)(posCol + 1)) && filter_function(posRow, (int16_t)(posCol + 1), this, _Context))
			{
				//screen[posRow][(int16_t)(posCol + 1)] = newC;
				p.x = posRow;
				p.y = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit(posRow, (int16_t)(posCol + 1));
				this->unsetBit(posRow, (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol - 1)) && !filledZone->getBit(posRow, (int16_t)(posCol - 1)) && filter_function(posRow, (int16_t)(posCol - 1), this, _Context))
			{
				p.x = posRow;
				p.y = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit(posRow, (int16_t)(posCol - 1));
				this->unsetBit(posRow, (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol + 1));
				this->unsetBit((int16_t)(posRow + 1), (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol - 1));
				this->unsetBit((int16_t)(posRow + 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), (int16_t)(posCol - 1));
				this->unsetBit((int16_t)(posRow - 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBit((int16_t)(posRow - 1), (int16_t)(posCol + 1));
				this->unsetBit((int16_t)(posRow - 1), (int16_t)(posCol + 1));
			}
		}
	}


	// returns 0 on success
	inline bool floodFillOnes(size_t row, size_t col, BitMatrix* filledZone) {
		return floodFill(row, col, BitMatrix::floodFillFilterFunctionOnes, NULL, filledZone);
	}

	// returns 0 on success
	inline bool floodFillZeroes(size_t row, size_t col, BitMatrix* filledZone) {
		return floodFill(row, col, BitMatrix::floodFillFilterFunctionZeroes, NULL, filledZone);
	}

	
	inline BitMatrix floodFillOnes(size_t row, size_t col) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFillOnes(row, col, &filledZone);
		return filledZone;
	}

	
	inline BitMatrix floodFillZeroes(size_t row, size_t col) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFillZeroes(row, col, &filledZone);
		return filledZone;
	}


	inline std::vector<Point2D_int> findLongestPath() {
		std::vector<Point2D_int> path;
		BitMatrix visited(this->getRows(), this->getColumns());
		findLongestPath(this, &path, &visited);
		return path;
	}

	inline void findLongestPath(std::vector<Point2D_int>* longest_path) {
		BitMatrix visited(this->getRows(), this->getColumns());
		BitMatrix::findLongestPath(this, longest_path, &visited);
	}

	inline void findLongestPath(std::vector<Point2D_int>* longest_path, BitMatrix* visited) {
		BitMatrix::findLongestPath(this, longest_path, visited);
	}
	// Find the longest path in the skeleton
	// uses 1 additional BitMatrix
	static void findLongestPath(BitMatrix* skeleton, std::vector<Point2D_int>* longest_path, BitMatrix* visited) {
		// Direction vectors for 8-connected neighbors
		static int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		static int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };
		// Start BFS from any skeleton point
		Point2D_int start;
		BitMatrixPosition pos;
		Point2D_Distance farthestFromStart;
		Point2D_Distance longestPathResult;

		struct _local_path {
			Point2D_int start;
			std::vector<Point2D_int> path;
		};



		longest_path->clear();
		if (skeleton->countNonZero() <= 0) {
			return;
		}


		pos = skeleton->getFirstSetPixel();
		if (pos.valid == false) {
			return;
		}

		start.x = pos.column;
		start.y = pos.row;

		//BitMatrix visited(skeleton->getRows(), skeleton->getColumns());
		visited->setToZeros();
		// First BFS to find the farthest point from 'start'
		farthestFromStart = BitMatrix::bfs(&start, skeleton, visited);
		visited->setToZeros();
		// Second BFS from the farthest point found
		longestPathResult = BitMatrix::bfs(&(farthestFromStart.point), skeleton, visited);
		visited->setToZeros();


		// To store the points of the longest path, run BFS again and record the path
		std::deque<_local_path> q;
		_local_path current;
		Point2D_int p;
		struct _local_path temp_local_path;
		//std::vector<Point2D_int> longest_path;

		q.push_back({ longestPathResult.point, {longestPathResult.point} });
		visited->setBit(longestPathResult.point.y, longestPathResult.point.x);

		while (!q.empty()) {
			current = q.front();
			p = current.start;
			std::vector<Point2D_int>* path = &(current.path);
			q.pop_front();

			if (path->size() > longest_path->size()) {
				*longest_path = *path;
			}

			for (int i = 0; i < 8; ++i) {
				int newX = p.x + dx[i];
				int newY = p.y + dy[i];

				if (isValid(newX, newY, skeleton, visited)) {
					visited->setBit(newY, newX);
					//std::vector<Point2D_int> newPath = path;
					//newPath.push_back(Point2D_int{ (float)newX, (float)newY });
					//q.push({ Point2D_int{(float)newX, (float)newY}, newPath });

					temp_local_path.start.x = newX;
					temp_local_path.start.y = newY;
					temp_local_path.path = *path;
					temp_local_path.path.push_back(temp_local_path.start);
					q.push_back(temp_local_path);
				}
			}
		}

		//return longestPath;
	}

	// Find the longest path in the skeleton
	// uses 1 additional BitMatrix
	static void findLongestPath2(BitMatrix* skeleton, std::vector<Point2D_int>* longest_path, BitMatrix* visited) {
		// Direction vectors for 8-connected neighbors
		static int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		static int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };
		// Start BFS from any skeleton point
		Point2D_int start;
		BitMatrixPosition pos;
		Point2D_Distance farthestFromStart;
		Point2D_Distance longestPathResult;
		bool used_current_path = false;
		size_t current_path_original_size = 0;

		struct _local_path {
			Point2D_int start;
			std::vector<Point2D_int> *path;
		};



		longest_path->clear();
		if (skeleton->countNonZero() <= 0) {
			return;
		}


		pos = skeleton->getFirstSetPixel();
		if (pos.valid == false) {
			return;
		}

		start.x = pos.column;
		start.y = pos.row;

		//BitMatrix visited(skeleton->getRows(), skeleton->getColumns());
		visited->setToZeros();
		// First BFS to find the farthest point from 'start'
		farthestFromStart = BitMatrix::bfs(&start, skeleton, visited);
		visited->setToZeros();
		// Second BFS from the farthest point found
		longestPathResult = BitMatrix::bfs(&(farthestFromStart.point), skeleton, visited);
		visited->setToZeros();
		

		// To store the points of the longest path, run BFS again and record the path
		std::deque<_local_path *> q;
		_local_path *current;
		Point2D_int p;
		struct _local_path temp_local_path, *temp_ptr_local_path;
		//std::vector<Point2D_int> longest_path;

		temp_ptr_local_path = new _local_path;
		temp_ptr_local_path->start = longestPathResult.point;
		temp_ptr_local_path->path = new std::vector<Point2D_int>;
		temp_ptr_local_path->path->push_back(longestPathResult.point);

		q.push_back(temp_ptr_local_path);
		visited->setBit(longestPathResult.point.y, longestPathResult.point.x);

		while (!q.empty()) {
			current = q.front();
			p = current->start;
			std::vector<Point2D_int> *path = (current->path);
			q.pop_front();
			used_current_path = false;
			current_path_original_size = path->size();

			if (path->size() > longest_path->size()) {
				*longest_path = *path;
			}

			for (int i = 0; i < 8; ++i) {
				int newX = p.x + dx[i];
				int newY = p.y + dy[i];

				if (isValid(newX, newY, skeleton, visited)) {
					visited->setBit(newY, newX);
					//std::vector<Point2D_int> newPath = path;
					//newPath.push_back(Point2D_int{ (float)newX, (float)newY });
					//q.push({ Point2D_int{(float)newX, (float)newY}, newPath });

					temp_ptr_local_path = new _local_path;

					temp_ptr_local_path->start.x = newX;
					temp_ptr_local_path->start.y = newY;

					if (used_current_path == false) {
						used_current_path = true;
						temp_ptr_local_path->path = path;
					}
					else {
						temp_ptr_local_path->path = new std::vector<Point2D_int>;
						temp_ptr_local_path->path->reserve(current_path_original_size+1);
						temp_ptr_local_path->path->resize(current_path_original_size);
						memcpy(temp_ptr_local_path->path->data(), path->data(), sizeof(Point2D_int) * current_path_original_size);
					}
					
					temp_ptr_local_path->path->push_back(temp_ptr_local_path->start);
					q.push_back(temp_ptr_local_path);
				}
			}
			if (used_current_path == false) {
				current->path->~vector();
				delete (current->path);
			}
			delete current;
		}

		//return longestPath;
	}


	// BFS to find the farthest point from a given start point
	// uses 1 additional BitMatrix
	static Point2D_Distance bfs(Point2D_int* start, BitMatrix* skeleton, BitMatrix *visited) {
		// Direction vectors for 8-connected neighbors
		static int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		static int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

		//BitMatrix visited(skeleton->getRows(), skeleton->getColumns());
		std::queue<Point2D_Distance> q;

		q.push(Point2D_Distance{ *start, 0 });
		visited->setBit(start->y, start->x);

		Point2D_int farthest = *start;
		int maxDist = 0;

		while (!q.empty()) {
			auto current = q.front();
			Point2D_int p = current.point;
			int dist = current.distance;
			q.pop();

			// Update the farthest point found
			if (dist > maxDist) {
				maxDist = dist;
				farthest = p;
			}

			// Explore 8-connected neighbors
			for (int i = 0; i < 8; ++i) {
				int newX = p.x + dx[i];
				int newY = p.y + dy[i];

				if (isValid(newX, newY, skeleton, visited)) {
					visited->setBit(newY, newX);
					q.push(Point2D_Distance{ Point2D_int{(int)newX, (int)newY}, dist + (float)1.0 });
				}
			}
		}

		return { farthest, (float)maxDist };
	}


	inline BitMatrixPosition getFirstSetPixel2() {
		for (size_t row = 0; row < this->getRows(); row++) {
			for (size_t col = 0; col < this->getColumns(); col++)
			{
				if (this->getBit(row, col) == true)
				{
					return BitMatrixPosition{ row, col, true };
				}
			}
		}
		return BitMatrixPosition{ 0, 0, false };
	}

	BitMatrixPosition getFirstSetPixel() {
		if (this->settedBits <= 0) {
			return BitMatrixPosition{ 0, 0, false };
		}
		BitMatrixPosition pos;
		size_t bit_index;
		size_t i = getFirstSetPixel_last_index;
		size_t start_index = getFirstSetPixel_last_index;
		for (; i < this->data.size(); i++) {
			if (this->data[i] != 0)
			{
				bit_index = indexOfFirstSettedBit(this->data[i]);
				pos = this->indexToPosition(i, bit_index);
				pos.valid = true;
				return pos;
			}
		}
		if (getFirstSetPixel_last_index >= this->data.size()) {
			getFirstSetPixel_last_index = 0;
		}

		for (; i < start_index; i++) {
			if (this->data[i] != 0)
			{
				bit_index = indexOfFirstSettedBit(this->data[i]);
				pos = this->indexToPosition(i, bit_index);
				pos.valid = true;
				return pos;
			}
		}
		this->settedBits = 0;
		return BitMatrixPosition{ 0, 0, false };
	}

	BitMatrixPosition getFirstUnsetPixel() {
		for (size_t row = 0; row < this->getRows(); row++) {
			for (size_t col = 0; col < this->getColumns(); col++)
			{
				if (this->getBit(row, col) == false)
				{
					return BitMatrixPosition{ row, col, true };
				}
			}
		}
		return BitMatrixPosition{ 0, 0, false };
	}

	static void downscale_3(BitMatrix* _dst, BitMatrix* _src, float min_treshold) {
		BITARRAY_DATATYPE up_word, mid_word, down_word;
		BITARRAY_DATATYPE right_up_word, right_mid_word, right_down_word;

		size_t new_columns, new_rows;
		size_t n_settedbits;

		_dst->clear();
		new_columns = _src->getColumns() / (size_t)3;
		new_rows = _src->getRows() / (size_t)3;

		for (size_t row = 1; row < _src->getRows() - 1; row += 3)
		{
			for (size_t col = 1; col < _src->getColumns() - 1; col += 3)
			{
				n_settedbits = 0;
				n_settedbits += (size_t)(_src->getBit(row - 1, col - 1) & true);
				n_settedbits += (size_t)(_src->getBit(row - 1, col) & true);
				n_settedbits += (size_t)(_src->getBit(row - 1, col + 1) & true);
				n_settedbits += (size_t)(_src->getBit(row, col - 1) & true);
				n_settedbits += (size_t)(_src->getBit(row, col) & true);
				n_settedbits += (size_t)(_src->getBit(row, col + 1) & true);
				n_settedbits += (size_t)(_src->getBit(row + 1, col - 1) & true);
				n_settedbits += (size_t)(_src->getBit(row + 1, col) & true);
				n_settedbits += (size_t)(_src->getBit(row + 1, col + 1) & true);

				if (n_settedbits >= (size_t)(9.0f * min_treshold)) {
					_dst->setBit(row / 3, col / 3);
				}
			}
		}

	}


	static void downscale(BitMatrix* _dst, BitMatrix* _src, size_t downscale_rate, float min_treshold) {
		BITARRAY_DATATYPE up_word, mid_word, down_word;
		BITARRAY_DATATYPE right_up_word, right_mid_word, right_down_word;

		size_t new_columns, new_rows;
		size_t n_settedbits;
		//size_t downscale_rate = 2;
		_dst->clear();
		new_columns = _src->getColumns() / (size_t)downscale_rate;
		new_rows = _src->getRows() / (size_t)downscale_rate;

		for (size_t row = 0; row < _src->getRows() - downscale_rate; row += downscale_rate)
		{
			for (size_t col = 0; col < _src->getColumns() - downscale_rate; col += downscale_rate)
			{
				n_settedbits = 0;
				for (size_t i = 0; i < downscale_rate; i++)
				{
					for (size_t j = 0; j < downscale_rate; j++)
					{
						n_settedbits += (size_t)(_src->getBit(row+i, col+j) & true);
					}
				}
				if (n_settedbits >= (size_t)((downscale_rate * downscale_rate) * min_treshold)) {
					_dst->setBit(row / downscale_rate, col / downscale_rate);
				}
			}
		}

	}


	~BitMatrix() {
		data.~vector();
	}


private:
	std::vector<BITARRAY_DATATYPE> data;
	size_t nRows;
	size_t nColumns;
	size_t settedBits;
	size_t getFirstSetPixel_last_index = 0;

	BitMatrixPosition indexToPosition(size_t index, size_t bit_index) {
		size_t row, col;
		bool valid = false;
		row = ((index * BITARRAY_DATATYPE_BITS) + bit_index) / this->getColumns();
		col = ((index * BITARRAY_DATATYPE_BITS) + bit_index) % this->getColumns();
		if (row >= this->getRows()) {
			valid = false;
		}
		else {
			valid = true;
		}

		if (col >= this->getColumns()) {
			valid = false;
		}
		else {
			valid = true;
		}
		return BitMatrixPosition{ row , col, valid };
	}

	size_t indexOfFirstSettedBit(BITARRAY_DATATYPE byte) {
		for (size_t i = 0; i < BITARRAY_DATATYPE_BITS; i++)
		{
			if (GetFlag(&byte, i) == true) {
				return i;
			}
		}
		return BITARRAY_DATATYPE_BITS;
	}

	size_t countSettedBits(BITARRAY_DATATYPE byte)
	{
		size_t settedBits = 0;
		for (size_t i = 0; i < sizeof(BITARRAY_DATATYPE); i++)
		{
			settedBits += this->count_ones((uint8_t)((byte >> (i*8)) & 0xFF));
		}
		return settedBits;
	}

	size_t count_ones(uint8_t byte)
	{
		static const uint8_t NIBBLE_LOOKUP[16] =
		{
		  0, 1, 1, 2, 1, 2, 2, 3,
		  1, 2, 2, 3, 2, 3, 3, 4
		};

		return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
	}

	static bool floodFillFilterFunctionOnes(size_t row, size_t col, BitMatrix* bit_matrix, void* _Context) {
		return bit_matrix->getBit(row, col);
	}

	static bool floodFillFilterFunctionZeroes(size_t row, size_t col, BitMatrix* bit_matrix, void* _Context) {
		return !(bit_matrix->getBit(row, col));
	}

	// Utility function to check if a point is valid
	static bool isValid(int col, int row, BitMatrix* main, BitMatrix* visited) {
		return (col >= 0 && col < main->getColumns() && row >= 0 && row < main->getRows() &&
			main->getBit(row, col) == true && visited->getBit(row, col) == false);
	}
};

#endif // !__BITMATRIX_H__
