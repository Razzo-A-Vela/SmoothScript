#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>

namespace Tokenizer {
  enum class TokenType {
    FUNC, RETURN, IF, ELSE,

    INT8, INT16, INT32, INT64,
    UINT8, UINT16, UINT32, UINT64,
    FLOAT, DOUBLE, BOOL, CSTR, CHAR,
    SIZE_T, VOID,
    
    SEMI, COLON, MINUS, EQUALS,
    PARENTS, SQUARES, BRACKETS,
    ARROW,
    

    IDENTIFIER, SYMBOL, LITERAL, PARSER_PARAM
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
