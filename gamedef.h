#ifndef _GAMEDEF_H_
#define _GAMEDEF_H_
#include "board.h"

class Gamedef {
  public:
    int gameid;
    int movesallowed;
    int colors;
    Board* extensioncolor;
    Board* boardstate;

    Gamedef() {
      extensioncolor = boardstate = NULL;
    }

    ~Gamedef() {
      if (extensioncolor) delete(extensioncolor);
      if (boardstate) delete(boardstate);
    }
};


#endif  // _GAMEDEF_H_
