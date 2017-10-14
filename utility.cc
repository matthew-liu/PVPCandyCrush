#include "utility.h"

void Utility::json_integer_set(json_t * object, const char* name, int input) {
	json_t *value = json_integer(input);
	json_object_set(object, name, value);
	json_decref(value);
}

void Utility::json_string_set(json_t * object, const char* name, const char* input) {
	json_t *value = json_string(input);
	json_object_set(object, name, value);
	json_decref(value);
}

// if (tag == 1) -> json_candy_board_set; else json_board_set
void Utility::json_board_set(json_t * object, const char* name, Board* board, int tag) {
	json_t * json_board = json_object();
	json_integer_set(json_board, "rows", board->rows);
	json_integer_set(json_board, "columns", board->columns);

	json_t * json_arr = json_array();
	json_t * value;
	int* payload;

	for (int i = 0; i < board->rows; i++) {
		for (int j = 0; j < board->columns; j++) {
			Get(board->data, i, j, (Payload*)&payload);
			if (tag == 1) {
				value = json_object();
				json_integer_set(value, "color", payload[0]);
				json_integer_set(value, "type", payload[1]);
			} else {
				value = json_integer(*payload);
			}
			json_array_append(json_arr, value);
			json_decref(value);
		}
	}
	json_object_set(json_board, "data", json_arr);
	json_object_set(object, name, json_board);
	json_decref(json_arr);
	json_decref(json_board);
}

void Utility::outputToFile(Gamedef &gamedef, Gamestate &gamestate) {
	json_t * output = json_object();
	json_t * json_gamedef = json_object();
	json_t * json_gamestate = json_object();

	json_object_set(output, "gamedef", json_gamedef);
	json_object_set(output, "gamestate", json_gamestate);

	// set json_gamedef
	json_integer_set(json_gamedef, "gameid", gamedef.gameid);
	json_board_set(json_gamedef, "extensioncolor", gamedef.extensioncolor, 0);
	json_board_set(json_gamedef, "boardstate", gamedef.boardstate, 0);
	json_integer_set(json_gamedef, "movesallowed", gamedef.movesallowed);
	json_integer_set(json_gamedef, "colors", gamedef.colors);

	// set json_gamestate
	json_board_set(json_gamestate, "boardcandies", gamestate.boardcandies, 1);
	json_board_set(json_gamestate, "boardstate", gamestate.boardstate, 0);
	json_integer_set(json_gamestate, "movesmade", gamestate.movesmade);
	json_integer_set(json_gamestate, "currentscore", gamestate.currentscore);

	json_t * json_arr = json_array();
	json_t * value;
	for (int i = 0; i < gamestate.boardstate->rows; i++) {
		value = json_integer(gamestate.extensionoffset[i]);
		json_array_append(json_arr, value);
		json_decref(value);
	}
	json_object_set(json_gamestate, "extensionoffset", json_arr);
	json_decref(json_arr);


	json_dump_file(output, "./record.out",0);
	json_decref(json_gamedef);
	json_decref(json_gamestate);
	json_decref(output);
}

void Utility::serialize(char** result, GameInstance &gameInstance) {
	Gamedef &gamedef = gameInstance.gamedef;
	Gamestate &gamestate = gameInstance.gamestate;

	json_t * message = json_object();
	json_t * output = json_object();
	json_string_set(message, "action", "update");
	json_object_set(message, "gameinstance", output);

	json_t * json_gamedef = json_object();
	json_t * json_gamestate = json_object();

	json_object_set(output, "gamedef", json_gamedef);
	json_object_set(output, "gamestate", json_gamestate);

	// set json_gamedef
	json_integer_set(json_gamedef, "gameid", gamedef.gameid);
	json_board_set(json_gamedef, "extensioncolor", gamedef.extensioncolor, 0);
	json_board_set(json_gamedef, "boardstate", gamedef.boardstate, 0);
	json_integer_set(json_gamedef, "movesallowed", gamedef.movesallowed);
	json_integer_set(json_gamedef, "colors", gamedef.colors);

	// set json_gamestate
	json_board_set(json_gamestate, "boardcandies", gamestate.boardcandies, 1);
	json_board_set(json_gamestate, "boardstate", gamestate.boardstate, 0);
	json_integer_set(json_gamestate, "movesmade", gamestate.movesmade);
	json_integer_set(json_gamestate, "currentscore", gamestate.currentscore);

	json_t * json_arr = json_array();
	json_t * value;
	for (int i = 0; i < gamestate.boardstate->rows; i++) {
		value = json_integer(gamestate.extensionoffset[i]);
		json_array_append(json_arr, value);
		json_decref(value);
	}
	json_object_set(json_gamestate, "extensionoffset", json_arr);
	json_decref(json_arr);


	*result = json_dumps(message, JSON_ENCODE_ANY);
	json_decref(json_gamedef);
	json_decref(json_gamestate);
	json_decref(output);
	json_decref(message);
}


void Utility::freeFunction(Payload payload) {
	free(payload);
}

// if (tag == 1) -> getBoardCandies; else getBoard
Board* Utility::getBoard(json_t* parent, int tag) {

	Board* board = new Board();

	board->rows = json_integer_value(json_object_get(parent, "rows"));
	board->columns = json_integer_value(json_object_get(parent, "columns"));

	Array_2d arr2d = ConstructArray2d(board->rows, board->columns, &freeFunction);
	int* payload;
	int row;
	int column;

	json_t * json_arr = json_object_get(parent, "data");
	size_t index;
	json_t *value;

	json_array_foreach(json_arr, index, value) {
		// calculate row & column
		row = index / board->columns;
		column = index % board->columns;
		// assign value to payload
		if (tag) {
			payload = (int*) malloc(sizeof(int) * 2);
			payload[0] = json_integer_value(json_object_get(value, "color"));
			payload[1] = json_integer_value(json_object_get(value, "type"));			
		} else {
			payload = (int*) malloc(sizeof(int));
			*payload = json_integer_value(value);
		}

		if (!Set(arr2d, row, column, payload)) {
			delete(board);
			FreeArray2d(arr2d);
	    	fprintf(stderr, "error: can not set row: %d column: %d\n", row, column);
	    	return NULL;
		}
	}

	board->data = arr2d;
	return board;
}
int Utility::constructFromFile_Client(char* filename, Gamedef &gamedef, Gamestate &gamestate) {


	FILE* input = fopen(filename, "r");

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

	//**************************************************//
	json_t * json_gameinstance = json_object_get(root, "gameinstance");
	json_t * json_gamedef = json_object_get(json_gameinstance, "gamedef");
	json_t * json_gamestate = json_object_get(json_gameinstance, "gamestate");

	// assign values to gamedef

	gamedef.gameid = json_integer_value(json_object_get(json_gamedef, "gameid"));
	gamedef.movesallowed = json_integer_value(json_object_get(json_gamedef, "movesallowed"));
	gamedef.colors = json_integer_value(json_object_get(json_gamedef, "colors"));

	json_t * json_extensioncolor = json_object_get(json_gamedef, "extensioncolor");
	json_t * json_boardstate = json_object_get(json_gamedef, "boardstate");

	gamedef.extensioncolor = getBoard(json_extensioncolor, 0);
	gamedef.boardstate = getBoard(json_boardstate, 0);


	if (!gamedef.extensioncolor || !gamedef.boardstate) {
		fprintf(stderr, "error: loading board for gamedef failed\n");
		json_decref(root);
		return 1;
	}

	// assign values to gamestate

	if (json_gamestate == NULL) {
		// construct gamestate
		gamestate.movesmade = 0;
		gamestate.currentscore = 0;
		// construct boardstate
		gamestate.boardstate = getBoard(json_boardstate, 0);
		// construct extensionoffset
		int* extensionoffset = new int[gamestate.boardstate->columns];
		for (int i = 0; i < gamestate.boardstate->columns; i++) {
			extensionoffset[i] = gamestate.boardstate->rows;
		}
		gamestate.extensionoffset = extensionoffset;
		// construct boardcandies
		Board* boardcandies = new Board();

		boardcandies->rows = gamestate.boardstate->rows;
		boardcandies->columns = gamestate.boardstate->columns;

		Array_2d arr2d = ConstructArray2d(gamestate.boardstate->rows, gamestate.boardstate->columns, &freeFunction);
		int* payload;
		int* input;

		for (int i = 0; i < boardcandies->rows; i++) {
			for (int j = 0; j < boardcandies->columns; j++) {
				payload = (int*) malloc(sizeof(int) * 2);
				Get(gamedef.extensioncolor->data, i, j, (Payload*)&input);
				payload[0] = *input;
				payload[1] = 0;

				if (!Set(arr2d, i, j, payload)) {
					delete(boardcandies);
					FreeArray2d(arr2d);
			    	fprintf(stderr, "error: can not set row: %d column: %d\n", i, j);
			    	return 1;
				}
			}
		}		
		boardcandies->data = arr2d;
		gamestate.boardcandies = boardcandies;
	} else {
		gamestate.movesmade = json_integer_value(json_object_get(json_gamestate, "movesmade"));
		gamestate.currentscore = json_integer_value(json_object_get(json_gamestate, "currentscore"));
		
		// get boardstate
		json_boardstate = json_object_get(json_gamestate, "boardstate");
		gamestate.boardstate = getBoard(json_boardstate, 0);

		if (!gamestate.boardstate) {
			fprintf(stderr, "error: loading board for gamestate failed\n");
			json_decref(root);
			return 1;
		}

		// get extensionoffset
		int* extensionoffset = new int[gamestate.boardstate->columns];

		json_t * json_arr = json_object_get(json_gamestate, "extensionoffset");
		size_t index;
		json_t *value;

		json_array_foreach(json_arr, index, value) {
			extensionoffset[index] = json_integer_value(value);
		}		
		gamestate.extensionoffset = extensionoffset;

		// get boardcandies
		json_t * json_boardcandies = json_object_get(json_gamestate, "boardcandies");
		gamestate.boardcandies = getBoard(json_boardcandies, 1);

		if (!gamestate.boardcandies) {
			fprintf(stderr, "error: loading boardcandies for gamestate failed\n");
			json_decref(root);
			return 1;
		}
	}

	json_decref(root);

	return 0;	
}

int Utility::constructFromFile(char* filename, Gamedef &gamedef, Gamestate &gamestate) {


	FILE* input = fopen(filename, "r");

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

	//**************************************************//

	json_t * json_gamedef = json_object_get(root, "gamedef");
	json_t * json_gamestate = json_object_get(root, "gamestate");

	// assign values to gamedef

	gamedef.gameid = json_integer_value(json_object_get(json_gamedef, "gameid"));
	gamedef.movesallowed = json_integer_value(json_object_get(json_gamedef, "movesallowed"));
	gamedef.colors = json_integer_value(json_object_get(json_gamedef, "colors"));

	json_t * json_extensioncolor = json_object_get(json_gamedef, "extensioncolor");
	json_t * json_boardstate = json_object_get(json_gamedef, "boardstate");

	gamedef.extensioncolor = getBoard(json_extensioncolor, 0);
	gamedef.boardstate = getBoard(json_boardstate, 0);


	if (!gamedef.extensioncolor || !gamedef.boardstate) {
		fprintf(stderr, "error: loading board for gamedef failed\n");
		json_decref(root);
		return 1;
	}

	// assign values to gamestate

	if (json_gamestate == NULL) {
		// construct gamestate
		gamestate.movesmade = 0;
		gamestate.currentscore = 0;
		// construct boardstate
		gamestate.boardstate = getBoard(json_boardstate, 0);
		// construct extensionoffset
		int* extensionoffset = new int[gamestate.boardstate->columns];
		for (int i = 0; i < gamestate.boardstate->columns; i++) {
			extensionoffset[i] = gamestate.boardstate->rows;
		}
		gamestate.extensionoffset = extensionoffset;
		// construct boardcandies
		Board* boardcandies = new Board();

		boardcandies->rows = gamestate.boardstate->rows;
		boardcandies->columns = gamestate.boardstate->columns;

		Array_2d arr2d = ConstructArray2d(gamestate.boardstate->rows, gamestate.boardstate->columns, &freeFunction);
		int* payload;
		int* input;

		for (int i = 0; i < boardcandies->rows; i++) {
			for (int j = 0; j < boardcandies->columns; j++) {
				payload = (int*) malloc(sizeof(int) * 2);
				Get(gamedef.extensioncolor->data, i, j, (Payload*)&input);
				payload[0] = *input;
				payload[1] = 0;

				if (!Set(arr2d, i, j, payload)) {
					delete(boardcandies);
					FreeArray2d(arr2d);
			    	fprintf(stderr, "error: can not set row: %d column: %d\n", i, j);
			    	return 1;
				}
			}
		}		
		boardcandies->data = arr2d;
		gamestate.boardcandies = boardcandies;
	} else {
		gamestate.movesmade = json_integer_value(json_object_get(json_gamestate, "movesmade"));
		gamestate.currentscore = json_integer_value(json_object_get(json_gamestate, "currentscore"));
		
		// get boardstate
		json_boardstate = json_object_get(json_gamestate, "boardstate");
		gamestate.boardstate = getBoard(json_boardstate, 0);

		if (!gamestate.boardstate) {
			fprintf(stderr, "error: loading board for gamestate failed\n");
			json_decref(root);
			return 1;
		}

		// get extensionoffset
		int* extensionoffset = new int[gamestate.boardstate->columns];

		json_t * json_arr = json_object_get(json_gamestate, "extensionoffset");
		size_t index;
		json_t *value;

		json_array_foreach(json_arr, index, value) {
			extensionoffset[index] = json_integer_value(value);
		}		
		gamestate.extensionoffset = extensionoffset;

		// get boardcandies
		json_t * json_boardcandies = json_object_get(json_gamestate, "boardcandies");
		gamestate.boardcandies = getBoard(json_boardcandies, 1);

		if (!gamestate.boardcandies) {
			fprintf(stderr, "error: loading boardcandies for gamestate failed\n");
			json_decref(root);
			return 1;
		}
	}

	json_decref(root);

	return 0;	
}