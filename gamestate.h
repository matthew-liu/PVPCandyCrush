#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_
#include "board.h"

class Gamestate {
  public:
    int movesmade;
    int currentscore;
    int* extensionoffset;
    Board* boardcandies;
    Board* boardstate;

    Gamestate() {
    	extensionoffset = NULL;
    	boardcandies = boardstate = NULL;
    }

    ~Gamestate() {
      if (extensionoffset) delete[] extensionoffset;
      if (boardcandies) delete(boardcandies);
      if (boardstate) delete(boardstate);
    }
};


#endif  // _GAMEDEF_H_
