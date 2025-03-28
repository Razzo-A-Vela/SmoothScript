#include "Literal.hpp"

void Literal::print(std::ostream& out) {
  switch (type) {
    case LiteralType::INTEGER :
      out << "INTEGER(" << u.integer << ')';
      break;
    
    case LiteralType::STRING :
      out << "STRING(" << u.string << ')';
      break;
    
    case LiteralType::CHAR :
      out << "CHAR(" << u.character << ')';
      break;
  }
}

bool Literal::operator==(const Literal& right) const {
  if (type != right.type)
    return false;
  
  switch (type) {
    case LiteralType::INTEGER :
      return u.integer == right.u.integer;

    case LiteralType::STRING :
      return std::string(u.string) == right.u.string;

    case LiteralType::CHAR :
      return u.character == right.u.character;
  }

  return true;
}
