#include <stdlib.h>

#include "array2d.h"
#include "array2d_priv.h"

bool correctIndex(Array_2d arr_2d, int row, int column) {
	return	(row >= 0 && row < arr_2d->arr_num && column >= 0 && column < arr_2d->arr_len); 
}

Array_2d ConstructArray2d(int arr_num, int arr_len, PayloadFreeFunction freefunc) {
	Array_2d arr2d = (Array_2d) malloc(sizeof(Array2d_Head));
	if (arr2d == NULL) {
		// out of memory
		return (Array_2d) NULL;
	}

	arr2d->arr_len = arr_len;
	arr2d->arr_num = arr_num;
	arr2d->start = (Payload*) malloc(sizeof(Payload*) * arr_len * arr_num);
	arr2d->freefunc = freefunc; // store the addr of the freefunc

	// initialize the 2D Array so that every element inside is NULL
	for (int i = 0; i < arr_num; i++) {
		for (int j = 0; j < arr_len; j++) {
			*(arr2d->start + i * arr2d->arr_len + j) = NULL;
		}
	}

	return arr2d;
}

bool Get(Array_2d arr_2d, int row, int column, Payload* payload_addr) {
	if (!correctIndex(arr_2d, row, column) || arr_2d == NULL || payload_addr == NULL) {
		return false;
	}

	*payload_addr = *(arr_2d->start + row * arr_2d->arr_len + column);
	return true;
}

bool Set(Array_2d arr_2d, int row, int column, Payload new_payload) {
	if (!correctIndex(arr_2d, row, column) || arr_2d == NULL) {
		return false;
	}
	Payload* location = arr_2d->start + row * arr_2d->arr_len + column;
	if (*location != NULL) { // if there is an original payload
		arr_2d->freefunc(*location);
	} 
	*location = new_payload;
	return true;
}

bool Swap(Array_2d arr_2d, int row1, int column1, int row2, int column2) {
	if (!correctIndex(arr_2d, row1, column1) || !correctIndex(arr_2d, row2, column2)) {
		return false;
	}
	Payload temp = *(arr_2d->start + row1 * arr_2d->arr_len + column1);
	*(arr_2d->start + row1 * arr_2d->arr_len + column1) = *(arr_2d->start + row2 * arr_2d->arr_len + column2);
	*(arr_2d->start + row2 * arr_2d->arr_len + column2) = temp;

	return true;
}

int GetColumnCount(Array_2d arr_2d) {
	if (arr_2d == NULL) {
		return -1;
	}

	return arr_2d->arr_len;
}

int GetRowCount(Array_2d arr_2d) {
	if (arr_2d == NULL) {
		return -1;
	}

	return arr_2d->arr_num;
}

bool FreePayload(Array_2d arr_2d, int row, int column) {
	if (!correctIndex(arr_2d, row, column) || arr_2d == NULL) {
		return false;
	}
	
	return Set(arr_2d, row, column, NULL);
}

void FreeArray2d(Array_2d arr_2d) {
	if (arr_2d == NULL) {
		// do nothing
		return;
	}

	for (int i = 0; i < arr_2d->arr_num; i++) {
		for (int j = 0; j < arr_2d->arr_len; j++) {
			// if there is a payload, delete it
			if (*(arr_2d->start + i * arr_2d->arr_len + j) != NULL) {
				FreePayload(arr_2d, i, j);
			} 
		}
	}
	// free the "actual arr" & the arr_head
	free(arr_2d->start);
	free(arr_2d);
}