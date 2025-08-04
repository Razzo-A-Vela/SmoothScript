#include "Parser.hpp"

namespace Parser {
  void Parser::process() {
    syntaxChecker.process();
  }

  void Parser::print(std::ostream& out) {
    out << "Syntax Checker:\n\n";
    syntaxChecker.print(out);
  }
}
