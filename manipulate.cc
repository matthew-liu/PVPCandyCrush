#include "manipulate.h"

int Manipulate::update(Gamedef &gamedef, Gamestate &gamestate) {
	int limit = 1000;
	int everFired = 0;
	int fired = 0; // by default no template fired
	do {
		fired = vFour(gamestate) | hFour(gamestate) | vThree(gamestate) | hThree(gamestate);
		if (fired) {
			everFired = 1;
			limit--;
		}
		scan_drop(gamedef, gamestate);
	} while (fired && limit > 0);

	return everFired;
}

void Manipulate::scan_drop(Gamedef &gamedef, Gamestate &gamestate) {
	Board* extension = gamedef.extensioncolor;
	int* offset = gamestate.extensionoffset;
	Board* board = gamestate.boardcandies;

	int index;
	int* payload;
	int value;

	for (int j = 0; j < board->columns; j++) {
		index = 0;
		for (int i = 0; i < board->rows; i++) {
			Get(board->data, i, j, (Payload*)&payload);
			if (payload[0] != -1) {
				if (index != i) Swap(board->data, i, j, index, j);
				index++;
			}
			// if payload[0] == -1, don't update index
		}
		// drop here
		for (int k = index; k < board->rows; k++) {
			// get value from extension board
			Get(extension->data, (offset[j] % extension->rows), j, (Payload*)&payload);
			value = *payload;
			// update offset[j]
			offset[j]++;
			// update value on the board
			Get(board->data, k, j, (Payload*)&payload);
			payload[0] = value;
		}
	}	
}

int Manipulate::vertical(Gamestate &gamestate, int bound) {
	int fired = 0; // by default no template fired

	Board* board = gamestate.boardcandies;
	int* payload;
	int* curr;
	int* next;

	for (int i = 0; i < board->rows - (bound - 1); i++) {
		for (int j = 0; j < board->columns; j++) {
			Get(board->data, i, j, (Payload*)&curr);
			for (int k = 1; k < bound; k++) {
				Get(board->data, i + k, j, (Payload*)&next);
				if (curr[0] != next[0] || curr[0] == -1) break;
				if (k + 1 == bound) { // last iteration of the loop
					fired = 1;
					for (int l = 0; l < bound; l++) { // set the matched blocks to -1
						Get(board->data, i + l, j, (Payload*)&payload);
						payload[0] = -1;
						// update the score
						gamestate.currentscore++;
					}
				}
			}
		}
	}
	return fired; // if fired == 1, then at least 1 template fired	
}

int Manipulate::horizontal(Gamestate &gamestate, int bound) {
	int fired = 0; // by default no template fired

	Board* board = gamestate.boardcandies;
	int* payload;
	int* curr;
	int* next;

	for (int i = 0; i < board->rows; i++) {
		for (int j = 0; j < board->columns  - (bound - 1); j++) {
			Get(board->data, i, j, (Payload*)&curr);
			for (int k = 1; k < bound; k++) {
				Get(board->data, i, j + k, (Payload*)&next);
				if (curr[0] != next[0] || curr[0] == -1) break;
				if (k + 1 == bound) { // last iteration of the loop
					fired = 1;
					for (int l = 0; l < bound; l++) { // set the matched blocks to -1
						Get(board->data, i, j + l, (Payload*)&payload);
						payload[0] = -1;
						// update the score
						gamestate.currentscore++;
					}
				}
			}
		}
	}
	return fired; // if fired == 1, then at least 1 template fired	
}

int Manipulate::vFour(Gamestate &gamestate) {
	return vertical(gamestate, 4);
}

int Manipulate::vThree(Gamestate &gamestate) {
	return vertical(gamestate, 3);
}

int Manipulate::hFour(Gamestate &gamestate) {
	return horizontal(gamestate, 4);
}

int Manipulate::hThree(Gamestate &gamestate) {
	return horizontal(gamestate, 3);
}