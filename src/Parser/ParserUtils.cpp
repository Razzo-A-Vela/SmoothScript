#include "ParserUtils.hpp"

namespace Parser {
  void IntStack::push(int i) {
    vect.push_back(i);
  }

  int IntStack::pop() {
    int i = vect.at(vect.size() - 1);
    vect.pop_back();
    return i;
  }

  Utils::Error syntaxError(const char* msg, int line) {
    return { "Syntax Error", msg, line };
  }

  namespace Result {
    None* const NONE = new None(0);
    
    inst<None> success() { return success<None>(NONE); }
    inst<None> ignore(Error err) { return ignore<None>(err); }
    inst<None> error(Error err) { return error<None>(err); }
  }
}
