#ifndef MANIPULATE_H
#define MANIPULATE_H

#include "utility.h"

class Manipulate {
  public:
    static int update(Gamedef &gamedef, Gamestate &gamestate);

  private:
    static int vertical(Gamestate &gamestate, int bound);
  	static int vFour(Gamestate &gamestate);
    static int vThree(Gamestate &gamestate);

    static int horizontal(Gamestate &gamestate, int bound);
    static int hFour(Gamestate &gamestate);
    static int hThree(Gamestate &gamestate);

    static void scan_drop(Gamedef &gamedef, Gamestate &gamestate);
};

#endif // MANIPULATE_H