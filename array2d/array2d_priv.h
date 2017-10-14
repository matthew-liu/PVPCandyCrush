#ifndef PRIV_H
#define PRIV_H

#include "array2d.h"

// define the head
typedef struct array2d_head {
	int arr_len;   // len of each array
	int arr_num;   // number of arrays
	Payload* start; // the starting address of array_2d
	PayloadFreeFunction freefunc; // the freefunction used to free any necessary payload
} Array2d_Head;

#endif 