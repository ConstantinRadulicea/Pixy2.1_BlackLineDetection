#ifndef __BITIMAGE_H__
#define __BITIMAGE_H__

#include "rgb2hsv.h"
#include "BitMatrix.h"


// horizontal pixels are rows, vertical pixels are columns
class BitImage : public BitMatrix
{
public:
	BitImage(size_t image_height, size_t image_width) : BitMatrix(image_height, image_width){
	}

	~BitImage(){
	}

	bool GetPixel(size_t x, size_t y) {
		return this->getBitXY(x, y);
	}
	void SetPixel(size_t x, size_t y) {
		this->setBitXY(x, y);
	}
	void SetPixel(size_t x, size_t y, bool value) {
		this->setBitValueXY(x, y, value);
	}
	void UnsetPixel(size_t x, size_t y) {
		this->unsetBitXY(x, y);
	}
	size_t GetWidth() {
		return this->getColumns();
	}
	size_t GetHeight() {
		return this->getRows();
	}

private:

};



#endif // !__BITIMAGE_H__
