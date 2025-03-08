#include "Literal.hpp"

void Literal::print(std::ostream& out) {
  switch (type) {
    case LiteralType::INTEGER :
      out << "INTEGER(" << u.integer << ')';
      break;
  }
}

bool Literal::operator==(const Literal& right) const {
  if (type != right.type)
    return false;
  
  switch (type) {
    case LiteralType::INTEGER :
      return u.integer == right.u.integer;
  }

  return true;
}
