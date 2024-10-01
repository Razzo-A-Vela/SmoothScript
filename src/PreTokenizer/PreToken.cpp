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

      case PreTokenType::endPreProcessor :
        std::cout << "END_PRE_PROCESSOR";
        break;
    }
  }

  const std::function<bool(PreToken, PreToken)> PreToken::typeEqual = [](PreToken preToken, PreToken toEqual) { return preToken.type == toEqual.type; };
  const std::function<bool(PreToken, PreToken)> PreToken::typeCharEqual = [](PreToken preToken, PreToken toEqual) { return preToken.type == toEqual.type && preToken.u.character == toEqual.u.character; };
  const std::function<bool(PreToken, PreToken)> PreToken::typeStringEqual = [](PreToken preToken, PreToken toEqual) { return preToken.type == toEqual.type && std::string(preToken.u.string) == std::string(toEqual.u.string); };
}
