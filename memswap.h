#ifndef __MEMSWAP_H__
#define __MEMSWAP_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// swap 2 blocks of memory
void memswap(register void* _a, register void* _b, register size_t _size);

#ifdef __cplusplus
}
#endif

#endif
