#include "cvector.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


int cvector_init(cvector* _Cvector, size_t _NumberOfElementsToReserve, size_t _SizeOfElement) {
    if (_SizeOfElement <= (size_t)0) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }
    if (cvector_isvalid(_Cvector) && cvector_capacity(_Cvector) > (size_t)0) {
        _Cvector->capacity = (_Cvector->capacity * _Cvector->size_type) / _SizeOfElement;
        _Cvector->size_type = _SizeOfElement;
        _Cvector->size = (size_t)0;
        return cvector_reserve(_Cvector, _NumberOfElementsToReserve);
    }
    else {
        if (_NumberOfElementsToReserve > (size_t)0) {
            _Cvector->data = malloc(_SizeOfElement * _NumberOfElementsToReserve);
            if (!_Cvector->data && _NumberOfElementsToReserve != (size_t)0 && _SizeOfElement != (size_t)0) {
                return CVECTOR_ERROR_MEMORY_ALLOCATION;
            }
        }
        else {
            _Cvector->data = NULL;
        }
        _Cvector->size = (size_t)0;
        _Cvector->capacity = _NumberOfElementsToReserve;
        _Cvector->size_type = _SizeOfElement;
    }

    return CVECTOR_SUCCESS;
}

void* cvector_data(cvector* pt) {
    return pt->data;
}

size_t cvector_size(cvector* pt) {
    return pt->size;
}

size_t cvector_capacity(cvector* pt) {
    return pt->capacity;
}

size_t cvector_size_type(cvector* pt) {
    return pt->size_type;
}

int cvector_reserve(cvector* pt, size_t _NumOfElements) {
    void* temp;

    if (pt->size_type <= (size_t)0) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }
    if (pt->capacity >= _NumOfElements) {
        return CVECTOR_SUCCESS;
    }
    if (_NumOfElements <= (size_t)0) {
        pt->data = NULL;
        pt->capacity = (size_t)0;
        free(pt->data);
        return CVECTOR_SUCCESS;
    }

    if (pt->data == NULL) {
        return cvector_init(pt, _NumOfElements, pt->size_type);
    }

    temp = realloc(pt->data, pt->size_type * _NumOfElements);
    if (!temp) {
        return CVECTOR_ERROR_MEMORY_REALLOCATION;
    }
    pt->data = temp;
    pt->capacity = _NumOfElements;
    return CVECTOR_SUCCESS;
}

int cvector_resize(cvector* pt, size_t _NewSize) {
    int temp;
    if (_NewSize > pt->size) {
        temp = cvector_reserve(pt, _NewSize);
        if (temp != CVECTOR_SUCCESS) {
            return temp;
        }
    }
    pt->size = _NewSize;
    return CVECTOR_SUCCESS;
}

int cvector_shrink_to_fit(cvector* pt) {
    void* temp;

    if (pt->size_type <= (size_t)0) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }
    if (cvector_size(pt) == cvector_capacity(pt)) {
        return CVECTOR_SUCCESS;
    }
    if (cvector_size(pt) <= (size_t)0) {
        free(pt->data);
        pt->data = NULL;
        pt->size = (size_t)0;
    }
    else {
        temp = realloc(pt->data, pt->size_type * pt->size);
        if (!temp) {
            return CVECTOR_ERROR_MEMORY_REALLOCATION;
        }
        pt->data = temp;
    }

    pt->capacity = pt->size;
    return CVECTOR_SUCCESS;
}

void* cvector_at(cvector* pt, size_t _Index) {
    return &((char*)pt->data)[_Index * pt->size_type];
}

int cvector_insert(cvector* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    char* pos_ptr;
    size_t old_size = cvector_size(pt);
    size_t new_size;
    int tempResponse;

    if (_Pos < old_size) {
        new_size = old_size + _Count;
        tempResponse = cvector_resize(pt, new_size);
        if (tempResponse != CVECTOR_SUCCESS) {
            return tempResponse;
        }
        pos_ptr = (char*)cvector_at(pt, _Pos);
        memmove(cvector_at(pt, _Pos + _Count), pos_ptr, (old_size - _Pos) * cvector_size_type(pt));
        memmove(pos_ptr, _Elements, _Count * cvector_size_type(pt));
    }
    else {
        new_size = _Pos + _Count;
        tempResponse = cvector_resize(pt, new_size);
        if (tempResponse != CVECTOR_SUCCESS) {
            return tempResponse;
        }
        pos_ptr = (char*)cvector_at(pt, _Pos);
        memmove(pos_ptr, _Elements, _Count * cvector_size_type(pt));
    }
    return CVECTOR_SUCCESS;
}

int cvector_assign(cvector* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    int tempResponse;
    if (_Pos + _Count > cvector_size(pt)) {
        tempResponse = cvector_resize(pt, _Pos + _Count);
        if (tempResponse != CVECTOR_SUCCESS) {
            return tempResponse;
        }
    }
    memcpy(cvector_at(pt, _Pos), _Elements, _Count * cvector_size_type(pt));

    return CVECTOR_SUCCESS;
}

int cvector_set(cvector* pt, size_t _Index, const void* _Element) {
    return cvector_assign(pt, _Element, _Index, (size_t)1);
}

void cvector_erase(cvector* pt, size_t _Pos, size_t _Count) {
    if (_Pos >= cvector_size(pt)) {
        return;
    }
    else if (_Pos + _Count >= cvector_size(pt)) {
        cvector_resize(pt, _Pos);
    }
    else {
        memmove(cvector_at(pt, _Pos), cvector_at(pt, _Pos + _Count), (cvector_size(pt) - _Pos - _Count) * cvector_size_type(pt));
        cvector_resize(pt, cvector_size(pt) - _Count);
    }
}

void cvector_clear(cvector* pt) {
    cvector_resize(pt, (size_t)0);
}

int cvector_empty(cvector* pt) {
    return cvector_size(pt) == (size_t)0;
}

int cvector_full(cvector* pt) {
    return cvector_size(pt) == cvector_capacity(pt);
}

int cvector_append(cvector* pt, const void* _Element, size_t _NumOfElements) {
    int tempResponse;
    if (cvector_size(pt) + _NumOfElements > cvector_capacity(pt)) {
        tempResponse = cvector_reserve(pt, cvector_capacity(pt) + _NumOfElements + cvector_capacity(pt) / (size_t)8 + (size_t)8);
        if (tempResponse != CVECTOR_SUCCESS) {
            return tempResponse;
        }
    }
    memcpy(cvector_at(pt, cvector_size(pt)), _Element, cvector_size_type(pt) * _NumOfElements);

    tempResponse = cvector_resize(pt, cvector_size(pt) + _NumOfElements);
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    return CVECTOR_SUCCESS;
}

void* cvector_front(cvector* pt) {
    return cvector_at((cvector*)pt, (size_t)0);
}

void* cvector_back(cvector* pt) {
    if (cvector_empty(pt)) return NULL;
    return cvector_at(pt, cvector_size(pt) - (size_t)1);
}

void cvector_pop_back(cvector* pt) {
    if (cvector_empty(pt)) return;
    cvector_resize(pt, cvector_size(pt) - (size_t)1);
}

int cvector_push_back(cvector* pt, const void* _Element) {
    return cvector_append(pt, _Element, (size_t)1);
}

void cvector_swap(cvector* _Vector_1, cvector* _Vector_2) {
    memswap((void*)_Vector_1, (void*)_Vector_2, sizeof(cvector));
}

size_t cvector_shift_left(cvector* pt, size_t start_index, size_t positions_to_shift) {
    size_t elementsToShift, newSize;
    if (start_index >= cvector_size(pt)) {
        return 0;
    }

    elementsToShift = cvector_size(pt) - MIN(cvector_size(pt), start_index + positions_to_shift);
    newSize = start_index + elementsToShift;
    memmove(cvector_at(pt, start_index), cvector_at(pt, start_index + positions_to_shift), elementsToShift);

    cvector_resize(pt, newSize);
    return elementsToShift;
}

int cvector_isempty(cvector* _cvector) {
    if (cvector_size(_cvector) <= (size_t)0) {
        return 1;
    }
    return 0;
}

int cvector_isvalid(cvector* _cvector) {
    if (cvector_capacity(_cvector) > (size_t)0 && cvector_data(_cvector) == NULL) {
        return 0;
    }
    if (cvector_size_type(_cvector) <= (size_t)0) {
        return 0;
    }
    if (cvector_size(_cvector) > cvector_capacity(_cvector)) {
        return 0;
    }
    return 1;
}

int cvector_clone(cvector* _dst_Vector, cvector* _src_Vector) {
    int tempResponse;
    if (_dst_Vector == NULL || _src_Vector == NULL) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }

    tempResponse = cvector_init(_dst_Vector, (size_t)0, cvector_size_type(_src_Vector));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    tempResponse = cvector_insert(_dst_Vector, cvector_data(_src_Vector), (size_t)0, cvector_size(_src_Vector));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }
    return CVECTOR_SUCCESS;
}

void cvector_free(cvector* pt) {
    free(pt->data);
    pt->data = NULL;
    pt->capacity = (size_t)0;
    pt->size = (size_t)0;
    pt->size_type = (size_t)0;
}

void cvector_free_array(cvector* pt) {
    size_t i;
    for (i = (size_t)0; i < cvector_size(pt); i++) {
        cvector_free((cvector*)cvector_at(pt, i));
    }
}