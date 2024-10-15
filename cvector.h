//
// cvector.h
//
//      Written by RADULICEA Constantin D.P. . All rights reserved.
//
// Implentation of a dynamic vector, stack, string
//
// to-do: set error if memory allocation fail

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "memswap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CVECTOR_SUCCESS 0
#define CVECTOR_ERROR_MEMORY_ALLOCATION 1
#define CVECTOR_ERROR_MEMORY_REALLOCATION 2
#define CVECTOR_ERROR_INVALID_PARAMETERS 3

#define CVECTOR_DEFAULT(vec) vec.data=NULL; vec.size=(size_t)0; vec.capacity=(size_t)0; vec.size_type=(size_t)0
#define CVECTOR_CAST(ptr, type) ((type)(ptr))
#define CVECTOR_CAST_PTR_TO_VALUE(ptr, type) (*((type*)(ptr)))

#define LOG_ERROR(_message_) fprintf(stderr,_message_);
#define LOG_ERROR_ON(_statement_,_condition_,_message_) do { if ((_statement_)==_condition_) fprintf(stderr,_message_); } while(0)
#define LOG_ERROR_ON_NULL(_statement_,_message_) LOG_ERROR_ON(_statement_,NULL,_message_)



/***********************************************cvector*************************************************************************/

typedef struct cvector {
    void* data;         // pointer to the array conteining the array of elements
    size_t size;        // total elements conteined
    size_t capacity;    // maximum number of elements that can be conteined before allocate more memory
    size_t size_type;   // sizeof(element) in bytes
}cvector;

// Utility function to initialize the cvector
int cvector_init(cvector* _Cvector, size_t _NumberOfElementsToReserve, size_t _SizeOfElement);

// Utility function get a pointer to the buffer
void* cvector_data(cvector* pt);

// Returns the number of elements in the cvector
size_t cvector_size(cvector* pt);

// Returns the maximum number of elements that the cvector can hold.
size_t cvector_capacity(cvector* pt);

// Returns the size of an element
size_t cvector_size_type(cvector* pt);

// Requests that the cvector capacity be at least enough to contain n elements
// return 0 on success, -1 on failure
int cvector_reserve(cvector* pt, size_t _NumOfElements);

// Resizes the container so that it contains ‘n’  dropping the newest elements in the cvector.
int cvector_resize(cvector* pt, size_t _NewSize);

// Reduces the capacity of the cvector to fit its size and destroys all elements beyond the capacity.
int cvector_shrink_to_fit(cvector* pt);

// Utility function get the element at intex x
void* cvector_at(cvector* pt, size_t _Index);

// Utility function to insert a number of elements at a specific index
int cvector_insert(cvector* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Assigns a new value to the element, replacing its current contents.
int cvector_assign(cvector* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Utility function set the element at index _Index
int cvector_set(cvector* pt, size_t _Index, const void* _Element);

// Removes from the vector single or multiple elements
void cvector_erase(cvector* pt, size_t _Pos, size_t _Count);

// It is used to pop all the elements from the cvector
void cvector_clear(cvector* pt);

// Utility function to check if the cvector is empty or not
int cvector_empty(cvector* pt);

// Utility function to check if the cvector is full or not
int cvector_full(cvector* pt);

// Utility function to add multiple elements to the end of the cvector
int cvector_append(cvector* pt, const void* _Element, size_t _NumOfElements);

// Utility function to get the first element in the cvector
void* cvector_front(cvector* pt);

// Utility function to get the last element in the cvector
void* cvector_back(cvector* pt);

// Utility function to pop a top element from the cvector
void cvector_pop_back(cvector* pt);

// Utility function to push back an element `x` to the cvector
int cvector_push_back(cvector* pt, const void* _Element);

// Utility function to swap 2 vectors
void cvector_swap(cvector* _Vector_1, cvector* _Vector_2);

// returns total elements shifted
size_t cvector_shift_left(cvector* pt, size_t start_index, size_t positions_to_shift);

// returns total elements shifted
size_t cvector_shift_right(cvector* pt, size_t start_index, size_t positions_to_shift);

int cvector_isempty(cvector* _cvector);

int cvector_isvalid(cvector* _cvector);

// return a cloned a string
int cvector_clone(cvector* _dst_Vector, cvector* _src_Vector);

// free the memory of the vector
void cvector_free(cvector* pt);

// free the memory of an array of vectors
void cvector_free_array(cvector* pt);

#ifdef __cplusplus
}
#endif

#endif // !__CVECTOR_H__
