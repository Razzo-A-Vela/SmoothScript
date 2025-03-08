#pragma once
#include <ostream>


namespace PreTokenizer {
  enum class PreTokenType {
    IDENTIFIER, SYMBOL, NUMBER
  };

  struct PreToken {
    PreTokenType type;
    union {
      const char* string;
      char character;
    } u;
    int line; 

    void print(std::ostream& out);
    bool operator==(const PreToken& right) const;

    static bool typeCharEqual(PreToken left, PreToken right);
    static bool typeStringEqual(PreToken left, PreToken right);
  };
}
