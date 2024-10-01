#pragma once
#include <iostream>
#include <functional>

namespace PreTokenizer {
  enum class PreTokenType {
    symbol, identifier, literal, numLiteral, endPreProcessor
  };

  struct PreToken {
    PreTokenType type;
    union {
      const char* string;
      char character;
    } u;
    int line;

    void print();

    static const std::function<bool(PreToken, PreToken)> typeEqual;
    static const std::function<bool(PreToken, PreToken)> typeCharEqual;
    static const std::function<bool(PreToken, PreToken)> typeStringEqual;
  };
}
