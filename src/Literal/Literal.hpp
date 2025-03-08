#pragma once
#include <ostream>


enum class LiteralType {
  INTEGER
};

struct Literal {
  LiteralType type;
  union {
    int integer;
  } u;

  void print(std::ostream& out);
  bool operator==(const Literal& right) const;
};
