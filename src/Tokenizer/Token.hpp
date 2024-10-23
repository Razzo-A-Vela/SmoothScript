#pragma once
#include <iostream>

#include "Literal.hpp"

namespace Tokenizer {
  enum class TokenType {
    func, ret,
    colon, semi_colon, question, exclamation, equals, minus, comma, star,
    open_paren, closed_paren, open_bracket, closed_bracket, open_brace, closed_brace, // all symbols -> character;

    identifier, byte_type, literal, symbol, preProcessor, parserParameter, // identifier -> string; byte_type -> integer (if 0 then VOID BYTE TYPE); literal -> literal; symbol -> character; preProcessor -> string; parserParameter -> string;

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
    static const std::function<bool(Token)> isSymbol;
  };
}
