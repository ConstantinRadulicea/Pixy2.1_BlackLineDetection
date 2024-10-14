#ifndef __BITMATRIX_H__
#define __BITMATRIX_H__



#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "geometry2D.h"


typedef struct Point2D_Distance {
	Point2D point;
	float distance;
}Point2D_Distance;

typedef struct BitMatrixPosition {
	size_t row;
	size_t column;
	bool valid;
}BitMatrixPosition;



class BitMatrix;
// return true if the bit is accepted, otherwise returns false
typedef bool (*BitMatrixFillFilter)(size_t row, size_t col, BitMatrix* bit_matrix, void* _Context);

#define BITARRAY_DATATYPE unsigned int
#define BITARRAY_DATATYPE_MAX_VALUE UINT_MAX
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

	size_t getRows() {
		return this->nRows;
	}

	size_t getColumns() {
		return this->nColumns;
	}

	size_t totBlocks() {
		return data.size();
	}

	size_t bitSize() {
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

	bool getBit(size_t row, size_t col) {
		size_t offset = (row * this->nColumns) + col;
		return 1 & (this->data[offset / BITARRAY_DATATYPE_BITS] >> (offset % BITARRAY_DATATYPE_BITS));
	}
	
	void setBitValue(size_t row, size_t col, bool value) {
		size_t offset = (row * this->nColumns) + col;
		if (value) {
			this->setBit(row, col);
		}
		else {
			this->unsetBit(row, col);
		}
	}

	void setBitValueXY(size_t x, size_t y, bool value) {
		this->setBitValue(y, x, value);
	}
	bool getBitXY(size_t x, size_t y) {
		return this->getBit(y, x);
	}

	void setBit(size_t row, size_t col) {
		if (this->getBit(row, col)) {	// bit already setted
			return;
		}
		size_t offset = (row * this->nColumns) + col;
		this->data[offset / BITARRAY_DATATYPE_BITS] = this->data[offset / BITARRAY_DATATYPE_BITS] | (1 << (offset % BITARRAY_DATATYPE_BITS));
		this->settedBits++;
	}

	void setBitXY(size_t x, size_t y) {
		this->setBit(y, x);
	}

	void unsetBit(size_t row, size_t col) {
		if (!(this->getBit(row, col))) {	// bit already unsetted
			return;
		}
		size_t offset = (row * this->nColumns) + col;
		this->data[offset / BITARRAY_DATATYPE_BITS] = this->data[offset / BITARRAY_DATATYPE_BITS] & ~(1 << (offset % BITARRAY_DATATYPE_BITS));
		this->settedBits--;
	}

	void unsetBitXY(size_t x, size_t y) {
		this->unsetBit(y, x);
	}

	BITARRAY_DATATYPE getBlockValue(size_t index) {
		if (index == (this->totBlocks()-1))
		{
			return this->data[index] & (((BITARRAY_DATATYPE)BITARRAY_DATATYPE_MAX_VALUE) >> (this->bitSize() % BITARRAY_DATATYPE_BITS));
		}
		else {
			return this->data[index];
		}
	}

	void setBlockValue(size_t index, BITARRAY_DATATYPE value) {
		BITARRAY_DATATYPE oldValue;
		oldValue = this->getBlockValue(index);

		value &= (((BITARRAY_DATATYPE)BITARRAY_DATATYPE_MAX_VALUE) >> (this->bitSize() % BITARRAY_DATATYPE_BITS));

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

	static void absdiff(BitMatrix& src1, BitMatrix& src2, BitMatrix& dst) {
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < src1.totBlocks(); i++)
		{
			valueSrc1 = src1.getBlockValue(i);
			valueSrc2 = src2.getBlockValue(i);
			newValue = valueSrc1 ^ valueSrc2;
			dst.setBlockValue(i, newValue);
		}
	}

	static void AandNotB(BitMatrix& A, BitMatrix& B)
	{
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < A.totBlocks(); i++)
		{
			valueSrc1 = A.getBlockValue(i);
			valueSrc2 = B.getBlockValue(i);
			newValue = valueSrc1 & (~valueSrc2);
			A.setBlockValue(i, newValue);
		}
	}

	static void AandNotB(BitMatrix* dst_A, BitMatrix* B)
	{
		BITARRAY_DATATYPE valueSrc1, valueSrc2, newValue;

		for (size_t i = 0; i < dst_A->totBlocks(); i++)
		{
			valueSrc1 = dst_A->getBlockValue(i);
			valueSrc2 = B->getBlockValue(i);
			newValue = valueSrc1 & (~valueSrc2);
			dst_A->setBlockValue(i, newValue);
		}
	}

	void logicAnd(BitMatrix& B) {
		BITARRAY_DATATYPE valueSrc, valueB, newValue;

		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc & valueB;
			this->setBlockValue(i, newValue);
		}
	}

	void logicOr(BitMatrix& B) {
		BITARRAY_DATATYPE valueSrc, valueB, newValue;

		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc | valueB;
			this->setBlockValue(i, newValue);
		}
	}

	void setToZeros() {
		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			this->setBlockValue(i, (BITARRAY_DATATYPE)0);
		}
		this->settedBits = 0;
	}

	void setToOnes() {
		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			this->setBlockValue(i, (BITARRAY_DATATYPE)1);
		}
		this->settedBits = this->data.size() * sizeof(BITARRAY_DATATYPE) * 8;
	}

	bool isInsideBoundaries(size_t row, size_t col) {
		if (row < 0 || row >= this->getRows() || col < 0 || col >= this->getColumns()) {
			return false;
		}
		return true;
	}

	BitMatrix floodFill(size_t row, size_t col, BitMatrixFillFilter filter_function, void* _Context) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFill(row, col, filter_function, _Context, &filledZone);
		return filledZone;
	}


	// returns 0 on success
	bool floodFill(size_t row, size_t col, BitMatrixFillFilter filter_function, void* _Context, BitMatrix *filledZone) {
		std::queue<std::pair<int16_t, int16_t>> queue;
		int16_t posRow;
		int16_t posCol;

		filledZone->clear();

		if (!isInsideBoundaries(row, col) || !filter_function(row, col, this, _Context)) {
			return 1;
		}
		filledZone->setBitValue(row, col, true);

		// Append the position of starting
		// pixel of the component
		std::pair<int16_t, int16_t> p(row, col);
		queue.push(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (queue.size() > 0) {
			// Dequeue the front node
			std::pair<int16_t, int16_t> currPixel = queue.front();
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
				filledZone->setBitValue((int16_t)((int16_t)(posRow + 1)), posCol, true);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), posCol) && !filledZone->getBit((int16_t)(posRow - 1), posCol) && filter_function((int16_t)(posRow - 1), posCol, this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = posCol;
				queue.push(p);
				filledZone->setBitValue((int16_t)(posRow - 1), posCol, true);
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol + 1)) && !filledZone->getBit(posRow, (int16_t)(posCol + 1)) && filter_function(posRow, (int16_t)(posCol + 1), this, _Context))
			{
				//screen[posRow][(int16_t)(posCol + 1)] = newC;
				p.first = posRow;
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBitValue(posRow, (int16_t)(posCol + 1), true);
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol - 1)) && !filledZone->getBit(posRow, (int16_t)(posCol - 1)) && filter_function(posRow, (int16_t)(posCol - 1), this, _Context))
			{
				p.first = posRow;
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBitValue(posRow, (int16_t)(posCol - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.first = (int16_t)(posRow + 1);
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBitValue((int16_t)(posRow + 1), (int16_t)(posCol + 1), true);
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.first = (int16_t)(posRow + 1);
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBitValue((int16_t)(posRow + 1), (int16_t)(posCol - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = (int16_t)(posCol - 1);
				queue.push(p);
				filledZone->setBitValue((int16_t)(posRow - 1), (int16_t)(posCol - 1), true);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.first = (int16_t)(posRow - 1);
				p.second = (int16_t)(posCol + 1);
				queue.push(p);
				filledZone->setBitValue((int16_t)(posRow - 1), (int16_t)(posCol + 1), true);
			}
		}
	}

	// returns 0 on success
	bool floodFillOnes(size_t row, size_t col, BitMatrix* filledZone) {
		return floodFill(row, col, BitMatrix::floodFillFilterFunctionOnes, NULL, filledZone);
	}

	// returns 0 on success
	bool floodFillZeroes(size_t row, size_t col, BitMatrix* filledZone) {
		return floodFill(row, col, BitMatrix::floodFillFilterFunctionZeroes, NULL, filledZone);
	}

	
	BitMatrix floodFillOnes(size_t row, size_t col) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFillOnes(row, col, &filledZone);
		return filledZone;
	}

	
	BitMatrix floodFillZeroes(size_t row, size_t col) {
		BitMatrix filledZone(this->getRows(), this->getColumns());
		this->floodFillZeroes(row, col, &filledZone);
		return filledZone;
	}


	std::vector<Point2D> findLongestPath() {
		std::vector<Point2D> path;
		findLongestPath(this, &path);
		return path;
	}

	void findLongestPath(std::vector<Point2D>* longest_path) {
		BitMatrix::findLongestPath(this, longest_path);
	}

	// Find the longest path in the skeleton
	// uses 1 additional BitMatrix
	static void findLongestPath(BitMatrix* skeleton, std::vector<Point2D>* longest_path) {
		// Direction vectors for 8-connected neighbors
		int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };
		// Start BFS from any skeleton point
		Point2D start;
		bool foundStart = false;
		longest_path->clear();

		// Find any pixel in the skeleton to start the BFS
		for (int y = 0; y < skeleton->getRows() && !foundStart; y++) {
			for (int x = 0; x < skeleton->getColumns() && !foundStart; x++) {
				if (skeleton->getBit(y, x) == true) {
					start = Point2D{ (float)x, (float)y };
					foundStart = true;
				}
			}
		}

		// First BFS to find the farthest point from 'start'
		Point2D_Distance farthestFromStart = BitMatrix::bfs(&start, skeleton);

		// Second BFS from the farthest point found
		Point2D_Distance longestPathResult = BitMatrix::bfs(&(farthestFromStart.point), skeleton);

		BitMatrix visited(skeleton->getRows(), skeleton->getColumns());

		// To store the points of the longest path, run BFS again and record the path
		std::queue<std::pair<Point2D, std::vector<Point2D>>> q;
		//std::vector<Point2D> longest_path;

		q.push({ longestPathResult.point, {longestPathResult.point} });
		visited.setBit(longestPathResult.point.y, longestPathResult.point.x);

		while (!q.empty()) {
			auto current = q.front();
			Point2D p = current.first;
			std::vector<Point2D> path = current.second;
			q.pop();

			if (path.size() > longest_path->size()) {
				*longest_path = path;
			}

			for (int i = 0; i < 8; ++i) {
				int newX = p.x + dx[i];
				int newY = p.y + dy[i];

				if (isValid(newX, newY, skeleton, &visited)) {
					visited.setBit(newY, newX);
					std::vector<Point2D> newPath = path;
					newPath.push_back(Point2D{ (float)newX, (float)newY });
					q.push({ Point2D{(float)newX, (float)newY}, newPath });
				}
			}
		}

		//return longestPath;
	}


	// BFS to find the farthest point from a given start point
	// uses 1 additional BitMatrix
	static Point2D_Distance bfs(Point2D* start, BitMatrix* skeleton) {
		// Direction vectors for 8-connected neighbors
		int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

		BitMatrix visited(skeleton->getRows(), skeleton->getColumns());
		std::queue<Point2D_Distance> q;

		q.push(Point2D_Distance{ *start, 0 });
		visited.setBit(start->y, start->x);

		Point2D farthest = *start;
		int maxDist = 0;

		while (!q.empty()) {
			auto current = q.front();
			Point2D p = current.point;
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

				if (isValid(newX, newY, skeleton, &visited)) {
					visited.setBit(newY, newX);
					q.push(Point2D_Distance{ Point2D{(float)newX, (float)newY}, (float)dist + (float)1.0 });
				}
			}
		}

		return { farthest, (float)maxDist };
	}


	BitMatrixPosition getFirstSetPixel() {
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


	~BitMatrix() {
		data.~vector();
	}


private:
	std::vector<BITARRAY_DATATYPE> data;
	size_t nRows;
	size_t nColumns;
	size_t settedBits;



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
