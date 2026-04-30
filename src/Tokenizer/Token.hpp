#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>

namespace Tokenizer {
  enum class TokenType {
    FUNC, RETURN, IF, ELSE,
    
    INT, FLOAT, BOOL, CHAR, CSTR, SIZE_T, VOID,
    
    SEMI, COLON, MINUS, EQUALS, COMMA,
    
    PARENTS, SQUARES, BRACKETS,
    
    ARROW,

    IDENTIFIER, SYMBOL, LITERAL
  };
  
  struct Token {
    TokenType type;
    union {
      const char* string;
      char character;
      Literal literal;
      std::vector<Token>* tokens;
    } u;
    int line;

    void print(std::ostream& out);
    bool operator==(Token& right) const;

    static bool typeStringEqual(Token left, Token right);
    static bool typeCharEqual(Token left, Token right);
    static bool typeLiteralEqual(Token left, Token right);
  };
}
