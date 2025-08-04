#pragma once
#include <ostream>
#include <vector>

#include "SyntaxChecker.hpp"

namespace Parser {
  class Parser {
  public:
    Parser(std::vector<Token> tokens) : syntaxChecker(tokens) {}
    void process();
    void print(std::ostream& out);

  protected:

  private:
    SyntaxChecker syntaxChecker;
  };
}
