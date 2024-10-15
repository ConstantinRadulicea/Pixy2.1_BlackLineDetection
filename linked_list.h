#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**************************************************************************************************************/
// Double linked list
typedef struct d_linked_list
{
	void* data;
	//int value;
	struct d_linked_list* next, * previous;
}d_ll;

static d_ll* d_ll_find_s(d_ll* _front, d_ll* _back, int(*compar)(d_ll* node, const void* context), void* _context) {
	while (_front && _front != _back)
	{
		if (compar(_front, _context) == 0) {
			return _front;
		}
		_front = _front->next;
	}
	return NULL;
}

static d_ll* d_ll_malloc() {
	d_ll* node;
	node = (d_ll*)malloc(sizeof(d_ll));
	if (!node) {
		printf("malloc failed!\n");
		return NULL;
	}
	memset(node, 0, sizeof(d_ll));
	return node;
}

static size_t d_ll_size(d_ll* _list)
{
	size_t count = 0;

	while (_list)
	{
		_list = _list->next;
		count++;
	}
	return count;
}

static d_ll* d_ll_front(d_ll* _list)
{
	if (!_list) return NULL;
	d_ll* temp = _list;
	while (temp->previous)
	{
		temp = temp->previous;
	}
	return temp;
}

static d_ll* d_ll_back(d_ll* _list)
{
	if (!_list) return NULL;
	d_ll* temp = _list;
	while (temp->next)
	{
		temp = temp->next;
	}
	return temp;
}

static void d_ll_append(d_ll* _list, d_ll* _new)
{
	d_ll* back = d_ll_back(_list);
	_new->next = NULL;
	_new->previous = back;
	if (back) back->next = _new;
}

static d_ll* d_ll_at(d_ll* _list, size_t _index)
{
	size_t count = 0;

	d_ll* temp = d_ll_front(_list);
	while (count < _index)
	{
		if (!temp) return NULL; //no more elements, intex too high
		temp = temp->next;
		count++;
	}
	return temp;
}

//returns ((size_t)-1) on fail
static size_t d_ll_index(d_ll* _elem)
{
	size_t count = 0;

	d_ll* temp = d_ll_front(_elem);
	while (_elem != temp)
	{
		if (!(temp->next)) return (size_t)-1; //no more elements, intex too high (all bits to 1)
		temp = temp->next;
		count++;
	}
	return count;
}

//insert a new element in before another one
static void d_ll_insert(d_ll* _old, d_ll* _new)
{
	if (_new) _new->next = _old;
	if (_old) {
		if (_old->previous)_old->previous->next = _new;
	}
	if (_new && _old)_new->previous = _old->previous;
	else if (_new) _new->previous = NULL;
	if (_old) _old->previous = _new;
}

//insert a new element in before another one, return 0 on success
static int d_ll_insert_at(d_ll* _list, d_ll* _new, size_t _index)
{
	d_ll* temp = d_ll_at(_list, _index);
	if (!temp) return -1;
	d_ll_insert(temp, _new);
	return 0;
}

static d_ll* d_ll_push_front(d_ll* _list, d_ll* _new)
{
	d_ll* front = d_ll_front(_list);
	_new->next = front;
	_new->previous = NULL;
	if (front) front->previous = _new;
	return _new;
}

static void d_ll_push_back(d_ll* _list, d_ll* _new)
{
	d_ll_append(_list, _new);
}

static void d_ll_remove(d_ll* _elem)
{
	if (!_elem) return;
	if (_elem->previous) _elem->previous->next = _elem->next;
	if (_elem->next) _elem->next->previous = _elem->previous;
}

static d_ll* d_ll_pop_front(d_ll* _list) {
	d_ll* element;
	element = d_ll_front(_list);
	d_ll_remove(element);
	return element;
}

// if the size of the list is 1 then s_ll_pop_back will do nothing
static d_ll* d_ll_pop_back(d_ll* _list) {
	d_ll* element;
	element = d_ll_back(_list);
	d_ll_remove(element);
	return element;
}

//return 0 on success or -1 if the element doesen't exist
static int d_ll_remove_at(d_ll* _list, size_t _index)
{
	d_ll* temp = d_ll_at(_list, _index);
	if (temp)
	{
		d_ll_remove(temp);
		return 0;
	}
	return -1;
}

// iterates the list executing an arbitrary function while the function return NULL
static void d_ll_iterator(d_ll* _start, int _direction, int (*func)(d_ll*, void*), void* _context)
{
	d_ll* temp = _start;
	if (_direction >= 0)
	{
		while (temp)
		{
			if (func(temp, _context)) return;
			temp = temp->next;
		}
	}
	else
	{
		while (temp)
		{
			if (func(temp, _context)) return;
			temp = temp->previous;
		}
	}
}

static d_ll* d_ll_reverse(d_ll* _front)
{
	d_ll* prev_elem, * elem, * next_elem;
	if (!_front) return NULL;

	prev_elem = _front;
	elem = prev_elem->next;
	prev_elem->next = NULL;
	while (elem)
	{
		next_elem = elem->next;
		elem->next = prev_elem;
		prev_elem = elem;
		elem = next_elem;
	}
	return prev_elem;
}

/**********************************************************************************************************************************************/
//Single linked list

typedef struct s_linked_list
{
	void* data;
	struct s_linked_list* next;
}s_ll;

static size_t s_ll_size(s_ll* _list)
{
	size_t count = 0;

	while (_list)
	{
		_list = _list->next;
		count++;
	}
	return count;
}

static s_ll* s_ll_prev(s_ll* _list, s_ll* _elem)
{
	if (_list == _elem) return NULL;
	s_ll* temp = _list;
	while (temp)
	{
		if (temp->next == _elem) return temp;
		temp = temp->next;
	}
	return NULL;
}

static s_ll* s_ll_prev_back(s_ll* _list)
{
	if (!_list) return NULL;
	else if (!_list->next) return NULL;
	s_ll* temp = _list;
	while (temp->next->next)
	{
		temp = temp->next;
	}
	return temp;
}

static s_ll* s_ll_back(s_ll* _list)
{
	if (!_list) return NULL;
	s_ll* temp = _list;
	while (temp->next)
	{
		temp = temp->next;
	}
	return temp;
}

static void s_ll_append(s_ll* _list, s_ll* _elem)
{
	s_ll* temp = s_ll_back(_list);
	if (temp) temp->next = _elem;
	if (_elem) _elem->next = NULL;
}

//returns NULL on fail
static s_ll* s_ll_at(s_ll* _front, size_t _index)
{
	size_t count = 0;
	s_ll* temp = _front;
	while (count < _index)
	{
		if (!temp) return NULL; //no more elements, intex too high
		temp = temp->next;
		count++;
	}
	return temp;
}

//returns ((size_t)-1) on fail
static size_t s_ll_index(s_ll* _front, s_ll* _elem)
{
	size_t count = 0;
	s_ll* temp = _front;
	while (_elem != temp)
	{
		if (!temp) return -1; //no more elements, intex too high (all bits to 1)
		temp = temp->next;
		count++;
	}
	return count;
}

//insert a new element in before another one, returns 0 on success, returns -1 on fail
static int s_ll_insert(s_ll* _front, s_ll* _old, s_ll* _new)
{
	size_t index_old = s_ll_index(_front, _old);
	if (index_old == ((size_t)-1))
	{
		return -1; //index of _old not found
	}

	if (index_old != 0) // If _old is not the elemnt at index 0
	{
		_front = s_ll_at(_front, index_old - 1);
		_front->next = _new;
	}
	_new->next = _old;
	return 0;
}

static int s_ll_insert_at(s_ll* _front, s_ll* _new, size_t _index) //insert a new element in before another one, return 0 on success
{
	s_ll* temp = s_ll_at(_front, _index);
	if (!temp)
	{
		return -1;
	}
	if (s_ll_insert(_front, temp, _new) == 0)
	{
		return 0;
	}
	return 1;
}

static s_ll* s_ll_push_front(s_ll* _front, s_ll* _new)
{
	if (_new) _new->next = _front;
	return _new;
}

static void s_ll_push_back(s_ll* _front, s_ll* _new) {
	s_ll_append(_front, _new);
}

static s_ll* s_ll_pop_front(s_ll* _front)
{
	if (_front) return _front->next;
	return NULL;
}

// if the size of the list is 1 then s_ll_pop_back will do nothing
static void s_ll_pop_back(s_ll* _front)
{
	s_ll* temp = s_ll_prev_back(_front);
	if (temp) temp->next = NULL;
}

//return 0 on success or -1 if the element doesent exist
static int s_ll_remove(s_ll* _front, s_ll* _elem)
{
	size_t index_elem = s_ll_index(_front, _elem);
	if (index_elem == ((size_t)-1))
	{
		return -1; //index of _elem not found
	}
	if (index_elem != 0) // If _elem is not the elemnt at index 0
	{
		_front = s_ll_at(_front, index_elem - 1);
		_front->next = _elem->next;
	}
	return 0;
}

//return 0 on success or -1 if the element doesen't exist
static int s_ll_remove_at(s_ll* _front, size_t _index)
{
	s_ll* temp = s_ll_at(_front, _index);
	if (!temp)
	{
		return -1;
	}
	if (s_ll_remove(_front, temp) == -1)
	{
		return -1;
	}
	return 0;
}

// iterates the list executing an arbitrary function while the function return NULL
static void s_ll_iterator(s_ll* _start, int (*func)(s_ll*, void*), void* _context)
{
	s_ll* temp = _start;
	while (temp)
	{
		if (func(temp, _context)) return;
		temp = temp->next;
	}
}

static void s_ll_swap(s_ll* _prev_a, s_ll* _a, s_ll* _prev_b, s_ll* _b)
{
	if (!_a || !_b) return;
	s_ll* temp;

	if (_prev_a) _prev_a->next = _b;
	if (_prev_b) _prev_b->next = _a;

	temp = _a->next;
	_a->next = _b->next;
	_b->next = temp;
}

//move _a after _b
static void s_ll_move_elem_after(s_ll* _prev_a, s_ll* _a, s_ll* _b)
{
	if (!_a || !_b) return;
	if (_prev_a) _prev_a->next = _a->next;

	if (_b)
	{
		_a->next = _b->next;
		_b->next = _a;
	}
}

static void s_ll_swap_next(s_ll* _prev, s_ll* _a)
{
	s_ll_swap(_prev, _a, _a, _a->next);
}

static s_ll* s_ll_reverse(s_ll* _front)
{
	s_ll* prev_elem, * elem, * next_elem;
	if (!_front) return NULL;

	prev_elem = _front;
	elem = prev_elem->next;
	prev_elem->next = NULL;
	while (elem)
	{
		next_elem = elem->next;
		elem->next = prev_elem;
		prev_elem = elem;
		elem = next_elem;
	}
	return prev_elem;
}

static s_ll* s_ll_bubbleSort_s(s_ll* _front, int(*compar)(void*, const void*, const void*), void* _context)
{
	if (!_front) return _front;
	else if (!_front->next) return _front;

	s_ll* pivot_1, * prev_pivot_1;
	s_ll* pivot_2, * prev_pivot_2, * next_pivot_2;
	s_ll* first_at_right_place, * last_swap_right_pos;

	pivot_1 = _front;
	pivot_2 = NULL;
	first_at_right_place = NULL;
	last_swap_right_pos = NULL;

	while (_front != first_at_right_place)
	{
		pivot_1 = _front;
		pivot_2 = pivot_1;
		prev_pivot_1 = NULL;
		prev_pivot_2 = NULL;

		while (pivot_2->next != first_at_right_place)
		{
			next_pivot_2 = pivot_2->next;
			if (compar(_context, pivot_1->data, next_pivot_2->data) < 0)
			{
				if (pivot_1 != pivot_2)
				{
					last_swap_right_pos = pivot_1;
					if (pivot_1 == _front) _front = pivot_2;
					s_ll_swap(prev_pivot_1, pivot_1, prev_pivot_2, pivot_2);
					prev_pivot_1 = pivot_1;
					pivot_2 = pivot_1;
				}
				prev_pivot_1 = pivot_1;
				pivot_1 = next_pivot_2;
			}
			prev_pivot_2 = pivot_2;
			pivot_2 = pivot_2->next;
		}
		if (pivot_1 != pivot_2)
		{
			last_swap_right_pos = pivot_1;
			if (pivot_1 == _front) _front = pivot_2;
			s_ll_swap(prev_pivot_1, pivot_1, prev_pivot_2, pivot_2);
		}
		else if (last_swap_right_pos == NULL) last_swap_right_pos = pivot_1;
		first_at_right_place = last_swap_right_pos;
		last_swap_right_pos = NULL;
	}
	return _front;
}

static s_ll* s_ll_orderedInsert_s(s_ll* _front, s_ll* _elem, int(*compar)(void*, const void*, const void*), void* _context)
{
	s_ll* pivot, * prev_pivot;
	pivot = _front;
	prev_pivot = NULL;

	if (!_front) return NULL;
	else if (!_elem) return _front;
	if (compar(_context, _elem, _front) < 0)
	{
		_elem->next = _front;
		return _elem;
	}
	prev_pivot = pivot;
	pivot = pivot->next;
	while (pivot)
	{
		if (compar(_context, _elem, pivot) < 0)
		{
			_elem->next = pivot;
			prev_pivot->next = _elem;
			return _front;
		}
		prev_pivot = pivot;
		pivot = pivot->next;
	}
	_elem->next = pivot;
	prev_pivot->next = _elem;
	return _front;
}

//return NULL If is sorted Or a ptr to the first element not sorted
static s_ll* s_ll_isSorted_s(s_ll* _front, s_ll* _back, int(*compar)(void*, const void*, const void*), void* _context)
{
	if (!_front) return NULL;
	while (_front->next != NULL && _front->next != _back)
	{
		if (compar(_context, _front->data, _front->next->data))
		{
			return _front;
		}
		_front = _front->next;
	}
	return NULL;
}

static s_ll* s_ll_find_s(s_ll* _front, s_ll* _back, s_ll* _match, int(*compar)(void*, const void*, const void*), void* _context)
{
	s_ll* pivot;
	pivot = _front;
	while (pivot != _back)
	{
		if (compar(_context, _match, pivot) == 0) return pivot;
		pivot = pivot->next;
	}
	return NULL;
}

#endif // !__LINKED_LIST_H__