#include "ParserUtils.hpp"

namespace Parser {
  void syntaxError(std::string msg, int line) {
    Utils::error("Syntax Error", msg, line);
  }

  void IntStack::push(int i) {
    vect.push_back(i);
  }

  int IntStack::pop() {
    int i = vect.at(vect.size() - 1);
    vect.pop_back();
    return i;
  }
}
