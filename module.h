#ifndef _MODULE_H_
#define _MODULE_H_

#include <string>
#include <vector>
#include "../socketCode/ClientSocket.h"

class Module {
  public:
	static string readMessage(hw5_net::ClientSocket &clientSocket) {
	    std::vector<char> buf;
	    int parens = 0;

	    char one_char;
	    int readCount;

	    while (true) {
	      readCount = clientSocket.WrappedRead(&one_char, 1);
	      if (readCount == 0) return ""; // broken pipe!!!
	      buf.push_back(one_char);
	      if (one_char == '{') parens++;
	      if (one_char == '}') parens--;
	      if (parens == 0) break;
	    }

	    string buf_str(buf.begin(), buf.end());
	    return buf_str;
	}
};


#endif  // _MODULE_H_