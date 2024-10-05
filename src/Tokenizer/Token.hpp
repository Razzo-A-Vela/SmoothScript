#pragma once
#include <iostream>

#include "Literal.hpp"

namespace Tokenizer {
  enum class TokenType {
    func, ret,
    colon, double_colon, semi_colon, question, double_question, exclamation, equals, minus, comma, star,
    open_paren, closed_paren, open_bracket, closed_bracket, open_brace, closed_brace, // all symbols -> character;

    identifier, byte_type, literal, symbol, preProcessor, // identifier -> string; byte_type -> integer; literal -> literal; symbol -> character; preProcessor -> string;

    endPreProcessor
  };

  struct Token {
    TokenType type;
    union {
      const char* string;
      char character;
      int integer;
      Literal literal;
    } u;
    int line;

    void print();

    static const std::function<bool(Token, Token)> typeEqual;
    static const std::function<bool(Token, Token)> typeIntEqual;
    static const std::function<bool(Token, Token)> typeCharEqual;
    static const std::function<bool(Token, Token)> typeStringEqual;
  };
}
