#pragma once
#include <ostream>

#include <Literal/Literal.hpp>

namespace Tokenizer {
  enum class TokenType {
    FUNCTION, IMPL, RETURN,

    
    SEMI, MINUS,
    OPEN_PAREN, CLOSED_PAREN, OPEN_SQUARE, CLOSED_SQUARE, OPEN_BRACKET, CLOSED_BRACKET,
    ARROW,
    

    IDENTIFIER, SYMBOL, LITERAL
  };
  
  struct Token {
    TokenType type;
    union {
      const char* string;
      char character;
      Literal literal;
    } u;
    int line;

    void print(std::ostream& out);
    bool operator==(Token& right);

    bool typeStringEqual(Token& left, Token& right);
    bool typeCharEqual(Token& left, Token& right);
    bool typeLiteralEqual(Token& left, Token& right);
  };
}
