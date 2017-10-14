#ifndef UTILITY_H
#define UTILITY_H

extern "C" {
	#include <stdio.h>
	#include <stdlib.h>
	#include <jansson.h>
}

#include "board.h"
#include "gamedef.h"
#include "gamestate.h"
#include "gameInstance.h"

class Utility {
  public:
    static int constructFromFile(char* filename, Gamedef &gamedef, Gamestate &gamestate);
    static int constructFromFile_Client(char* filename, Gamedef &gamedef, Gamestate &gamestate);
    static void outputToFile(Gamedef &gamedef, Gamestate &gamestate);
    static void json_string_set(json_t * object, const char* name, const char* input);
    static void json_integer_set(json_t * object, const char* name, int input);
    static void serialize(char** result, GameInstance &gameInstance);
  private:
  	static void freeFunction(Payload payload);
    static Board* getBoard(json_t* parent, int tag);
    static void json_board_set(json_t * object, const char* name, Board* board, int tag);
};

#endif // UTILITY_H