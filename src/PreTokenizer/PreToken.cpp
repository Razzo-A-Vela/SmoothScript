#include "PreToken.hpp"

namespace PreTokenizer {

  void PreToken::print(std::ostream& out) {
    switch (type) {
      case PreTokenType::IDENTIFIER :
        out << "IDENTIFIER(" << u.string << ')';
        break;
      
      case PreTokenType::SYMBOL :
        out << "SYMBOL(" << u.character << ')';
        break;
      
      case PreTokenType::NUMBER :
        out << "NUMBER(" << u.string << ')';
        break;
    }

    out << '[' << line << ']';
  }


  bool PreToken::operator==(const PreToken& right) {
    return type == right.type;
  }

  bool PreToken::typeCharEqual(PreToken left, PreToken right) {
    return left.type == right.type && left.u.character == right.u.character;
  }

  bool PreToken::typeStringEqual(PreToken left, PreToken right) {
    return left.type == right.type && std::string(left.u.string) == std::string(right.u.string);
  }
}
