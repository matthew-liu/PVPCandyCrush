#ifndef Array2d_H
#define Array2d_H

#include <stdbool.h>  // for bool type (true, false)

// declare the head
struct array2d_head;

typedef struct array2d_head* Array_2d;

// declare arr payload, which is the address of the actual element
typedef void* Payload;

// When a client frees a 2D Array, they need to pass a pointer to a function
// that does any necessary freee-ing of the payload inside the 2D Array.
// We invoke the pointer to the free function once for each payload, before
// the 2D Array itself is freed.
typedef void(*PayloadFreeFunction)(Payload payload);

// Construct and return a new 2D Array. The caller takes responsibility for 
// eventually calling FreeArray2d to freee memory asscociated with it.
// 
// Arguments: 
// 
// 	- arr_len: the number of columns
// 
//  - arr_num: the number of rows
//
//  - payload_free_func: a pointer to a payload freeing function;
//    used to free any necessary payloads
//
// Returns: NULL on error, non-NULL on success.
Array_2d ConstructArray2d(int arr_num, int arr_len, PayloadFreeFunction freefunc);

// Free a 2D Array that was previously constructed by ConstructArray2d.
//
// Arguments: 
// 
// 	- Array_2d: the 2D Array to free
//
void FreeArray2d(Array_2d arr_2d);

// Get and return a Payload inside the 2D Array corresponds with the specific
// row & column.
// 
// Arguments: 
// 
//  - Array_2d: the 2D Array from which to get the payload 
//
// 	- row: the row number
// 
//  - column: the column number
//
//  - payload_addr: a return parameter that is set by the callee; on success,
//    the payload is returned through this parameter.
//
// Returns: false on failure, true on success.
bool Get(Array_2d arr_2d, int row, int column, Payload* payload_addr);

// Set a Payload inside the 2D Array corresponds with the specific
// row & column.
// 
// Arguments: 
// 
//  - Array_2d: the 2D Array from which to set the payload 
//
// 	- row: the row number
// 
//  - column: the column number
//
//  - new_payload: the new payload to be added
//
// Returns: false on failure, true on success.
bool Set(Array_2d arr_2d, int row, int column, Payload new_payload);

// Returns the number of Columns in the 2D Array
// 
// Arguments: 
// 
// 	- arr_2d: the 2D Array to be counted
//
// Returns: -1 on failure, the number of Columns in the 2D Array on success.
int GetColumnCount(Array_2d arr_2d);

// Returns the number of Rows in the 2D Array
// 
// Arguments: 
// 
// 	- arr_2d: the 2D Array to be counted
//
// Returns: -1 on failure, the number of Rows in the 2D Array on success.
int GetRowCount(Array_2d arr_2d);

// Free a single payload in the 2D Array with the specific location.
//
// Arguments: 
// 
//  - Array_2d: the 2D Array from which to free the payload 
//
// 	- row: the row number
// 
//  - column: the column number
//
// Returns: false on failure, true on success.
bool FreePayload(Array_2d arr_2d, int row, int column);

// Swap the payloads of 2 locations with the specific 2D Array
//
// Arguments: 
// 
//  - Array_2d: the 2D Array from which to swap the payloads 
//
// 	- row1: the row number of first payload
// 
//  - column1: the column number of first payload
//
// 	- row2: the row number of second payload
// 
//  - column2: the column number of second payload
//
// Returns: false on failure, true on success.
bool Swap(Array_2d arr_2d, int row1, int column1, int row2, int column2);



#endif 