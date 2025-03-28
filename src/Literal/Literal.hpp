#pragma once
#include <ostream>


enum class LiteralType {
  INTEGER, STRING, CHAR
};

struct Literal {
  LiteralType type;
  union {
    int integer;
    const char* string;
    char character;
  } u;

  void print(std::ostream& out);
  bool operator==(const Literal& right) const;
};
