#pragma once
#include <iostream>

#include "Literal.hpp"

namespace Tokenizer {
  enum class TokenType {
    func, proc, ret,
    colon, semi_colon, question, equals, minus, comma, star,
    open_paren, closed_paren, open_bracket, closed_bracket, open_brace, closed_brace, // all symbols -> character;

    identifier, byte_type, literal, symbol // identifier -> string; byte_type -> integer; literal -> literal; symbol -> character;
  };

  struct Token {
    TokenType type;
    int line;
    union {
      const char* string;
      char character;
      int integer;
      Literal literal;
    } u;

    void print();
  };
}
