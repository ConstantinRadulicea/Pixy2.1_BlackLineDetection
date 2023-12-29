#pragma once

#include <vector>
#include <stdint.h>



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

	void unsetBit(size_t row, size_t col) {
		if (!(this->getBit(row, col))) {	// bit already unsetted
			return;
		}
		size_t offset = (row * this->nColumns) + col;
		this->data[offset / BITARRAY_DATATYPE_BITS] = this->data[offset / BITARRAY_DATATYPE_BITS] & ~(1 << (offset % BITARRAY_DATATYPE_BITS));
		this->settedBits--;
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

	void setToZeros() {
		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			this->setBlockValue(i, (BITARRAY_DATATYPE)0);
		}
	}

	void setToOnes() {
		for (size_t i = 0; i < this->totBlocks(); i++)
		{
			this->setBlockValue(i, (BITARRAY_DATATYPE)1);
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
};


