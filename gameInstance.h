#ifndef _GAMEINSTANCE_H_
#define _GAMEINSTANCE_H_
#include "gamedef.h"
#include "gamestate.h"

class GameInstance {
  public:
    Gamedef gamedef;
    Gamestate gamestate;
};


#endif  // _GAMEINSTANCE_H_