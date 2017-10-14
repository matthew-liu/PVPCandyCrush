#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "array2d.h"

void FreeFunction(Payload payload) {
	free(payload);
}

void print2dArr(Array_2d arr2d) {
	int* payload;
	int rows = GetRowCount(arr2d);
	int columns = GetColumnCount(arr2d);

	printf("Array is %d x %d\n", rows, columns);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if(Get(arr2d, i, j, (Payload*)&payload)) {
				printf("%d ", *payload);
			}
		}
	}
	printf("\n\n");
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		fprintf(stderr, "error: invalid argument count!\n");
		return 1;
	}

	FILE* input = fopen(argv[1], "r");

	if (input == NULL) {
		fprintf(stderr, "error: can not open file!\n");
		return 1;
	}

	json_t *root;
    json_error_t error;

    root = json_loadf(input, 0, &error);
    fclose(input);

    if(!root) {
    	fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    	return 1;
    }

    if(!json_is_object(root)) {
	    fprintf(stderr, "error: root is not an object\n");
	    json_decref(root);
	    return 1;
	}

	int rows = json_integer_value(json_object_get(root, "rows"));
	int columns = json_integer_value(json_object_get(root, "columns"));


	// [1] Deserialize test.json

	printf("[1] Deserialize %s\n", argv[1]);

	Array_2d arr2d = ConstructArray2d(rows, columns, &FreeFunction);
	int* payload;
	int row;
	int column;

	json_t * json_arr = json_object_get(root, "data");
	size_t index;
	json_t *value;

	json_array_foreach(json_arr, index, value) {
		// calculate row & column
		row = index / columns;
		column = index % columns;
		// assign value to payload
		payload = (int*) malloc(sizeof(int));
		*payload = json_integer_value(value);

		if (Set(arr2d, row, column, payload)) {
			printf("\t\tdeserialized %d\n", *payload);
		}
	}	

	print2dArr(arr2d);

	// [2] Set [1][1] to 100
	printf("[2] Set [1][1] to 100\n");
	payload = (int*) malloc(sizeof(int));
	*payload = 100;
	Set(arr2d, 1, 1, payload);
	print2dArr(arr2d);

	// [3] swap [1][1] and [0][0]
	printf("[3] swap [1][1] and [0][0]\n");
	Swap(arr2d, 1, 1, 0, 0);
	print2dArr(arr2d);

	// [4] swap [1][1] and [10][10]
	printf("[4] swap [1][1] and [10][10]\n");
	Swap(arr2d, 1, 1, 10, 10);
	print2dArr(arr2d);

	// [5] serialize array to file json.out
	printf("[5] serialize array to file json.out\n");
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			Get(arr2d, i, j, (Payload*)&payload);
			value = json_integer(*payload);
			if(json_array_set(json_arr, i * columns + j, value) == 0) {
				printf("\t\tserialized %d\n", *payload);
			}
			json_decref(value);
		}
	}
	printf("\n");

	json_t * output = json_object();
	// set rows 
	value = json_integer(rows);
	json_object_set(output, "rows", value);
	json_decref(value);
	// set columns
	value = json_integer(columns);
	json_object_set(output, "columns", value);
	json_decref(value);
	// set data
	json_object_set(output, "data", json_arr);

	json_dump_file(output, "./json.out",0);

	// [6] destroy array
	printf("[6] destroy array\n");
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			Get(arr2d, i, j, (Payload*)&payload);
			int value = *payload;
			if(FreePayload(arr2d, i, j)) {
				printf("\t\tdestroyed %d\n", value);
			}
		}
	}
	printf("\n");

	// destroy the references
	FreeArray2d(arr2d);
	json_decref(root);
	json_decref(output);
  	return 0;
}