#include "memswap.h"

void memswap(register void* _a, register void* _b, register size_t _size) {
	register size_t temp;
	while (_size >= sizeof(size_t))		// multiple of size_t
	{
		temp = *((size_t*)_a);
		*((size_t*)_a) = *((size_t*)_b);
		*((size_t*)_b) = temp;

		_a = ((size_t*)_a) + 1;
		_b = ((size_t*)_b) + 1;
		_size -= sizeof(size_t);
	}
#if UINTMAX_MAX > UINT64_MAX
	if (_size >= sizeof(uint64_t))
	{
		temp = *((uint64_t*)_a);
		*((uint64_t*)_a) = *((uint64_t*)_b);
		*((uint64_t*)_b) = temp;

		_a = ((uint64_t*)_a) + 1;
		_b = ((uint64_t*)_b) + 1;
		_size -= sizeof(uint64_t);
	}
#endif
#if UINTMAX_MAX > UINT32_MAX
	if (_size >= sizeof(uint32_t))
	{
		temp = *((uint32_t*)_a);
		*((uint32_t*)_a) = *((uint32_t*)_b);
		*((uint32_t*)_b) = temp;

		_a = ((uint32_t*)_a) + 1;
		_b = ((uint32_t*)_b) + 1;
		_size -= sizeof(uint32_t);
	}
#endif
#if UINTMAX_MAX > UINT16_MAX
	if (_size >= sizeof(uint16_t))
	{
		temp = *((uint16_t*)_a);
		*((uint16_t*)_a) = *((uint16_t*)_b);
		*((uint16_t*)_b) = temp;

		_a = ((uint16_t*)_a) + 1;
		_b = ((uint16_t*)_b) + 1;
		_size -= sizeof(uint16_t);
	}
#endif
#if UINTMAX_MAX > UINT8_MAX
	while (_size >= sizeof(uint8_t))	// in case at this point _size > 1, we will use a loop to copy remaining bytes
	{
		temp = *((uint8_t*)_a);
		*((uint8_t*)_a) = *((uint8_t*)_b);
		*((uint8_t*)_b) = temp;

		_a = ((uint8_t*)_a) + 1;
		_b = ((uint8_t*)_b) + 1;
		_size -= sizeof(uint8_t);
	}
#endif
}