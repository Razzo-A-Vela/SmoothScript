#include "PreToken.hpp"

namespace PreTokenizer {
  void PreToken::print() {
    switch (type) {
      case PreTokenType::symbol :
        std::cout << "SYMBOL(" << u.character << ')';
        break;

      case PreTokenType::identifier :
        std::cout << "IDENTIFIER(" << std::string(u.string) << ')';
        break;

      case PreTokenType::literal :
        std::cout << "LITERAL(" << std::string(u.string) << ')';
        break;

      case PreTokenType::numLiteral :
        std::cout << "NUM_LITERAL(" << std::string(u.string) << ')';
        break;

      case PreTokenType::preProcessor :
        std::cout << "PRE_PROCESSOR(" << std::string(u.string) << ')';
        break;
    }
  }
}
