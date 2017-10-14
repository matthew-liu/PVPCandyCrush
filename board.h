#ifndef _BOARD_H_
#define _BOARD_H_

extern "C" {
  #include "array2d.h"
}

class Board {
  public:
    int rows;
    int columns;
    Array_2d data;

    ~Board() {
      FreeArray2d(data);
    }
};


#endif  // _BOARD_H_

