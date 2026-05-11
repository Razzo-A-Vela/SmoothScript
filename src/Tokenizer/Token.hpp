#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>

namespace Tokenizer {
  enum class TokenType {
    FUNC, RETURN, IF, ELSE, WHILE, DO, LOOP, BREAK, CONTINUE, FOR,
    
    INT, FLOAT, BOOL, CHAR, CSTR, SIZE_T, CONST, SIGNED, UNSIGNED, VOID,
    
    SEMI, COLON, MINUS, PLUS, EQUALS, COMMA, EXCLAMATION,
    ASTERISK, SLASH, LESS, GREATER, AMPERSAND, PIPE, TILDE,
    
    PARENTS, SQUARES, BRACKETS,
    
    ARROW, PLUSPLUS, MINUSMINUS, DOUBLE_EQUALS, NOT_EQUAL,
    LESS_EQ, GREATER_EQ, SHIFT_RIGHT, SHIFT_LEFT, AND, OR,

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
