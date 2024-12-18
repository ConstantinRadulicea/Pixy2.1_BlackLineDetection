#ifndef __BITMATRIX_H__
#define __BITMATRIX_H__



#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "geometry2D.h"
#include "bits.h"
#include "rgb2hsv.h"


typedef struct Point2D_Distance {
	Point2D_int point;
	float distance;
}Point2D_Distance;

typedef struct BitMatrixPosition {
	size_t row;
	size_t column;
	bool valid;
}BitMatrixPosition;

typedef struct BitMatrixIndex {
	size_t index;
	size_t bit;
	bool valid;
}BitMatrixIndex;

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
#define LOG2_BITARRAY_DATATYPE_BITS 5		// LOG2(BITARRAY_DATATYPE_BITS)

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
		this->_total_bits = nRows * nColumns;

		if (this->bitSize() % BITARRAY_DATATYPE_BITS) {
			this->_total_blocks = (this->_total_bits / BITARRAY_DATATYPE_BITS) + 1;
		}
		else {
			this->_total_blocks = (this->_total_bits / BITARRAY_DATATYPE_BITS);
		}
		this->data.reserve(this->_total_blocks);
		this->data.resize(this->_total_blocks);

		this->clear();
	}

	void resize_no_shrinktofit(size_t _nRows, size_t _nColumns) {
		size_t new_total_bits = _nRows * _nColumns;
		bool grow_buffer = false;
		if (new_total_bits > this->_total_bits) {
			grow_buffer = true;
		}

		this->nRows = _nRows;
		this->nColumns = _nColumns;
		this->_total_bits = new_total_bits;

		if (this->_total_bits % BITARRAY_DATATYPE_BITS) {
			this->_total_blocks = (this->_total_bits / BITARRAY_DATATYPE_BITS) + 1;
		}
		else {
			this->_total_blocks = (this->_total_bits / BITARRAY_DATATYPE_BITS);
		}

		if (grow_buffer) {
			this->data.reserve(this->_total_blocks);
			this->data.resize(this->_total_blocks);
		}

		//this->clear();
	}

	inline size_t getRows() {
		return this->nRows;
	}

	inline size_t getColumns() {
		return this->nColumns;
	}

	inline size_t totBlocks() {
		return this->_total_blocks;
	}

	inline size_t bitSize() {
		return this->_total_bits;
	}

	void clear() {
		this->setToZeros();
	}

	void free() {
		this->nColumns = 0;
		this->nRows = 0;
		this->settedBits = 0;

		this->data.resize(0);
		this->data.reserve(0);
		this->data.shrink_to_fit();
	}

	//inline bool getBit(size_t row, size_t col) {
	//	size_t offset = (row * this->nColumns) + col;
	//	return (BITARRAY_DATATYPE)1 & (this->data[offset / BITARRAY_DATATYPE_BITS] >> (offset % BITARRAY_DATATYPE_BITS));
	//}


	inline bool getBit(size_t row, size_t col) const {
		size_t offset = row * this->nColumns + col;
		size_t dataIndex = offset >> LOG2_BITARRAY_DATATYPE_BITS;
		size_t bitIndex = offset & (BITARRAY_DATATYPE_BITS - 1);
		return (this->data[dataIndex] >> bitIndex) & 1;
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

	//inline void setBit(size_t row, size_t col) {
	//	if (this->getBit(row, col)) {	// bit already setted
	//		return;
	//	}
	//	size_t offset = (row * this->nColumns) + col;
	//	size_t index = offset / BITARRAY_DATATYPE_BITS;
	//	this->data[index] = (BITARRAY_DATATYPE)(this->data[index]) | (BITARRAY_DATATYPE)((BITARRAY_DATATYPE)1 << (offset % (size_t)BITARRAY_DATATYPE_BITS));
	//	this->settedBits++;
	//}


	inline void setBit(size_t row, size_t col) {
		size_t offset = (row * this->nColumns) + col;
		size_t index = offset >> LOG2_BITARRAY_DATATYPE_BITS;
		BITARRAY_DATATYPE bitMask = (BITARRAY_DATATYPE)1 << (offset & (BITARRAY_DATATYPE_BITS - 1));

		if (!(this->data[index] & bitMask)) {
			this->data[index] |= bitMask;
			++this->settedBits;
		}
	}

	inline void setBitXY(size_t x, size_t y) {
		this->setBit(y, x);
	}

	//inline void unsetBit(size_t row, size_t col) {
	//	size_t offset;
	//	BITARRAY_DATATYPE value;
	//	if (!(this->getBit(row, col))) {	// bit already unsetted
	//		return;
	//	}
	//	offset = (row * this->nColumns) + col;
	//	value = this->data[offset / BITARRAY_DATATYPE_BITS];
	//	value = value & ~((BITARRAY_DATATYPE)((BITARRAY_DATATYPE)1 << (offset % BITARRAY_DATATYPE_BITS)));
	//	this->data[offset / BITARRAY_DATATYPE_BITS] = value;
	//	this->settedBits--;
	//}


	inline void unsetBit(size_t row, size_t col) {
		size_t offset = (row * this->nColumns) + col;
		size_t index = offset >> LOG2_BITARRAY_DATATYPE_BITS;
		BITARRAY_DATATYPE bitMask = (BITARRAY_DATATYPE)1 << (offset & (BITARRAY_DATATYPE_BITS - 1));

		if (this->data[index] & bitMask) {
			this->data[index] &= ~bitMask;
			--this->settedBits;
		}
	}

	inline void unsetBitXY(size_t x, size_t y) {
		this->unsetBit(y, x);
	}

	inline BITARRAY_DATATYPE getBlockValue(size_t index) {
		if (index < this->totBlocks())
		{
			return this->data.at(index);
		}
		else {
			return this->data.at(index) & (((BITARRAY_DATATYPE)BITARRAY_DATATYPE_MAX_VALUE) >> ((size_t)BITARRAY_DATATYPE_BITS) - (this->bitSize() % (size_t)BITARRAY_DATATYPE_BITS));
		}
	}

	inline void setBlockValue( size_t index, BITARRAY_DATATYPE value) {
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
		size_t tot_blocks = dst_A->totBlocks();

		for (size_t i = 0; i < tot_blocks; i++)
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
		size_t tot_blocks = this->totBlocks();
		for (size_t i = 0; i < tot_blocks; i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc & valueB;
			this->setBlockValue(i, newValue);
		}
	}

	inline void logicOr(BitMatrix& B) {
		BITARRAY_DATATYPE valueSrc, valueB, newValue;
		size_t tot_blocks = this->totBlocks();
		for (size_t i = 0; i < tot_blocks; i++)
		{
			valueSrc = this->getBlockValue(i);
			valueB = B.getBlockValue(i);
			newValue = valueSrc | valueB;
			this->setBlockValue(i, newValue);
		}
	}

	inline void setToZeros() {
		memset(this->data.data(), 0, this->totBlocks() * sizeof(BITARRAY_DATATYPE));
		//for (size_t i = 0; i < this->totBlocks(); i++)
		//{
		//	this->setBlockValue(i, (BITARRAY_DATATYPE)0);
		//}
		this->settedBits = 0;
		this->getFirstSetPixel_last_index = 0;
	}

	inline void setToOnes() {
		memset(this->data.data(), 255, this->totBlocks() * sizeof(BITARRAY_DATATYPE));
		//for (size_t i = 0; i < this->totBlocks(); i++)
		//{
		//	this->setBlockValue(i, (BITARRAY_DATATYPE)1);
		//}
		this->settedBits = this->totBlocks() * sizeof(BITARRAY_DATATYPE) * 8;
	}

	inline bool isInsideBoundaries(size_t row, size_t col) {
		if (row < 0 || row >= this->nRows || col < 0 || col >= this->nColumns) {
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
		std::deque<Point2D_int16_t> queue;
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
		queue.push_back(p);

		// Color the pixel with the new color
		//screen[x][y] = newC;

		// While the queue is not empty i.e. the
		// whole component having prevC color
		// is not colored with newC color
		while (!queue.empty()) {
			// Dequeue the front node
			currPixel = queue.front();
			queue.pop_front();

			posRow = currPixel.x;
			posCol = currPixel.y;

			// Check if the adjacent
			// pixels are valid

			if (isInsideBoundaries((int16_t)(posRow + 1), posCol) && !filledZone->getBit((int16_t)(posRow + 1), posCol) && filter_function((int16_t)(posRow + 1), posCol, this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = posCol;
				queue.push_back(p);
				filledZone->setBit((int16_t)((int16_t)(posRow + 1)), posCol);
				this->unsetBit((int16_t)((int16_t)(posRow + 1)), posCol);
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), posCol) && !filledZone->getBit((int16_t)(posRow - 1), posCol) && filter_function((int16_t)(posRow - 1), posCol, this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = posCol;
				queue.push_back(p);
				filledZone->setBit((int16_t)(posRow - 1), posCol);
				this->unsetBit((int16_t)(posRow - 1), posCol);
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol + 1)) && !filledZone->getBit(posRow, (int16_t)(posCol + 1)) && filter_function(posRow, (int16_t)(posCol + 1), this, _Context))
			{
				//screen[posRow][(int16_t)(posCol + 1)] = newC;
				p.x = posRow;
				p.y = (int16_t)(posCol + 1);
				queue.push_back(p);
				filledZone->setBit(posRow, (int16_t)(posCol + 1));
				this->unsetBit(posRow, (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries(posRow, (int16_t)(posCol - 1)) && !filledZone->getBit(posRow, (int16_t)(posCol - 1)) && filter_function(posRow, (int16_t)(posCol - 1), this, _Context))
			{
				p.x = posRow;
				p.y = (int16_t)(posCol - 1);
				queue.push_back(p);
				filledZone->setBit(posRow, (int16_t)(posCol - 1));
				this->unsetBit(posRow, (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = (int16_t)(posCol + 1);
				queue.push_back(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol + 1));
				this->unsetBit((int16_t)(posRow + 1), (int16_t)(posCol + 1));
			}

			if (isInsideBoundaries((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow + 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow + 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.x = (int16_t)(posRow + 1);
				p.y = (int16_t)(posCol - 1);
				queue.push_back(p);
				filledZone->setBit((int16_t)(posRow + 1), (int16_t)(posCol - 1));
				this->unsetBit((int16_t)(posRow + 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol - 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol - 1), this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = (int16_t)(posCol - 1);
				queue.push_back(p);
				filledZone->setBit((int16_t)(posRow - 1), (int16_t)(posCol - 1));
				this->unsetBit((int16_t)(posRow - 1), (int16_t)(posCol - 1));
			}

			if (isInsideBoundaries((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && !filledZone->getBit((int16_t)(posRow - 1), (int16_t)(posCol + 1)) && filter_function((int16_t)(posRow - 1), (int16_t)(posCol + 1), this, _Context))
			{
				p.x = (int16_t)(posRow - 1);
				p.y = (int16_t)(posCol + 1);
				queue.push_back(p);
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

	// Find the longest path in the skeleton
	// uses 1 additional BitMatrix
	// the returned vector is dynamically allocated and must be freed (use delete operator)
	static std::vector<Point2D_int>* findLongestPath(BitMatrix* skeleton, BitMatrix* visited) {
		// Direction vectors for 8-connected neighbors
		const static int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		const static int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };
		std::vector<Point2D_int>* longest_path = NULL;
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



		//longest_path->clear();
		if (skeleton->countNonZero() <= 0) {
			return NULL;
		}


		pos = skeleton->getFirstSetPixel();
		if (pos.valid == false) {
			return NULL;
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

			if (longest_path == NULL)
			{
				longest_path = path;
			}
			else if (path->size() > longest_path->size()) {
				longest_path = path;
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
				if (current->path != longest_path)
				{
					current->path->~vector();
					delete (current->path);
				}
			}
			delete current;
		}

		return longest_path;
	}


	// BFS to find the farthest point from a given start point
	// uses 1 additional BitMatrix
	static Point2D_Distance bfs(Point2D_int* start, BitMatrix* skeleton, BitMatrix *visited) {
		// Direction vectors for 8-connected neighbors
		static int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
		static int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };
		Point2D_Distance current;
		int maxDist = 0;
		std::deque<Point2D_Distance> q;
		Point2D_int farthest;
		Point2D_int p;
		int dist;

		q.push_back(Point2D_Distance{ *start, 0.0f });
		visited->setBit(start->y, start->x);

		farthest = *start;
		

		while (!q.empty()) {
			current = q.front();
			p = current.point;
			dist = current.distance;
			q.pop_front();

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
					q.push_back(Point2D_Distance{ Point2D_int{(int)newX, (int)newY}, dist + (float)1.0 });
				}
			}
		}

		return { farthest, (float)maxDist };
	}


BitMatrixPosition getFirstSetPixel() {
		if (this->settedBits <= 0) {
			return BitMatrixPosition{ 0, 0, false };
		}
		BitMatrixPosition pos;
		size_t bit_index;
		size_t i = getFirstSetPixel_last_index;
		size_t start_index = getFirstSetPixel_last_index;
		size_t data_size = this->totBlocks();
		for (; i < data_size; i++) {
			if (this->data[i] != 0)
			{
				bit_index = indexOfFirstSettedBit(this->data[i]);
				pos = this->indexToPosition(i, bit_index);
				pos.valid = true;
				return pos;
			}
		}
		if (getFirstSetPixel_last_index >= data_size) {
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

	BitMatrixPosition getNextSetPixel(size_t row, size_t col) {
		BITARRAY_DATATYPE temp_datatype;
		BitMatrixPosition pos;
		size_t bit_index;
		size_t data_size;
		BitMatrixIndex start_index = this->PositionToIndex(BitMatrixPosition{row, col, true});

		if (start_index.valid == false)
		{
			return BitMatrixPosition{ 0, 0, false };
		}

		temp_datatype = getBlockValue(start_index.index);
		temp_datatype = temp_datatype >> (start_index.bit);
		if (temp_datatype != 0)
		{
			bit_index = indexOfFirstSettedBit(temp_datatype);
			pos = this->indexToPosition(start_index.index, start_index.bit + bit_index);
			pos.valid = true;
			return pos;
		}
		data_size = this->totBlocks();
		for (size_t i = (start_index.index + 1); i < data_size; i++)
		{
			temp_datatype = getBlockValue(i);
			if (temp_datatype != 0)
			{
				bit_index = indexOfFirstSettedBit(this->data[i]);
				pos = this->indexToPosition(i, bit_index);
				pos.valid = true;
				return pos;
			}
		}
		return BitMatrixPosition{ 0, 0, false };
	}

	static void downscale(BitMatrix* _dst, BitMatrix* _src, size_t downscale_rate, float min_treshold) {
		if (downscale_rate == 1) {
			*_dst = *_src;
			return;
		}

		BITARRAY_DATATYPE up_word, mid_word, down_word;
		BITARRAY_DATATYPE right_up_word, right_mid_word, right_down_word;

		size_t src_last_col, src_last_row;
		size_t n_settedbits;
		//size_t downscale_rate = 2;
		_dst->clear();
		src_last_col = _src->getColumns() - downscale_rate;
		src_last_row = _src->getRows() - downscale_rate;

		for (size_t row = 0; row < src_last_row; row += downscale_rate)
		{
			for (size_t col = 0; col < src_last_col; col += downscale_rate)
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

	BitMatrixPosition indexToPosition(BitMatrixIndex _index) {
		return indexToPosition(_index.index, _index.bit);
	}

	BitMatrixPosition indexToPosition(size_t index, size_t bit_index) {
		size_t row, col;
		bool valid = false;
		row = ((index * BITARRAY_DATATYPE_BITS) + bit_index) / this->nColumns;
		col = ((index * BITARRAY_DATATYPE_BITS) + bit_index) % this->nColumns;
		if (row >= this->nRows) {
			valid = false;
		}
		else {
			valid = true;
		}

		if (col >= this->nColumns) {
			valid = false;
		}
		else {
			valid = true;
		}
		return BitMatrixPosition{ row , col, valid };
	}

	BitMatrixIndex PositionToIndex(BitMatrixPosition pos) {
		BitMatrixIndex _index;
		if (pos.row > this->nRows || pos.column > this->nColumns)
		{
			return BitMatrixIndex{ 0, 0, false };
		}
		 size_t offset = (pos.row * this->nColumns) + pos.column;
		_index.valid = true;
		_index.index = offset / BITARRAY_DATATYPE_BITS;
		_index.bit = offset % BITARRAY_DATATYPE_BITS;
		return _index;
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
	size_t _total_bits;
	size_t _total_blocks;


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

	//// Utility function to check if a point is valid
	//static bool isValid(int col, int row, BitMatrix* main, BitMatrix* visited) {
	//	return (col >= 0 && col < main->getColumns() && row >= 0 && row < main->getRows() &&
	//		main->getBit(row, col) == true && visited->getBit(row, col) == false);
	//}

	// Utility function to check if a point is valid
	static bool isValid(int col, int row, BitMatrix* main, BitMatrix* visited) {
		if (col < 0 || col >= main->getColumns() || row < 0 || row >= main->getRows()) {
			return false;
		}
		return main->getBit(row, col) && !visited->getBit(row, col);
	}
};


static void baiernToBitmatrixDownscale_minPooling(BitMatrix* _dst, uint8_t* _src, uint16_t height, uint16_t width, size_t downscale_rate, float min_black_treshold_percentual, float window_black_pixels_percentual) {
	RGBcolor rgb_pixel;
	size_t src_last_col, src_last_row;
	size_t n_settedbits;
	size_t min_setted_bits = (size_t)(window_black_pixels_percentual * (float)(downscale_rate * downscale_rate));
	uint8_t min_color_treshold = (uint8_t)(min_black_treshold_percentual * 255.0f);
	_dst->clear();
	src_last_col = width - downscale_rate;
	src_last_row = height - downscale_rate;

	for (size_t row = 0; row < src_last_row; row += downscale_rate)
	{
		for (size_t col = 0; col < src_last_col; col += downscale_rate)
		{
			n_settedbits = 0;
			for (size_t i = 0; i < downscale_rate; i++)
			{
				for (size_t j = 0; j < downscale_rate; j++)
				{
					if (_src[((row + i) * width) + (col + j)] < min_color_treshold) {
						n_settedbits++;
						if (n_settedbits > min_setted_bits) {
							_dst->setBit(row / downscale_rate, col / downscale_rate);
							goto next_window;
						}
					}
				}
			}
		next_window: {}
		}
	}
}


#endif // !__BITMATRIX_H__
