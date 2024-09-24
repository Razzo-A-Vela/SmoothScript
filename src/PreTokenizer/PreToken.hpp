#pragma once
#include <iostream>

namespace PreTokenizer {
  enum class PreTokenType {
    symbol, identifier, literal, numLiteral, preProcessor
  };

  struct PreToken {
    PreTokenType type;
    union {
      const char* string;
      char character;
    } u;
    int line;

    void print();
  };
}
