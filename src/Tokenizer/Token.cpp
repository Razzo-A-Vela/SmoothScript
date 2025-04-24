#include "Token.hpp"

namespace Tokenizer {
  void Token::print(std::ostream& out) {
    switch (type) {
      case TokenType::FUNC :
        out << "FUNC";
        break;
      
      case TokenType::RETURN :
        out << "RETURN";
        break;
      

      case TokenType::SEMI :
        out << "SEMI";
        break;

      case TokenType::MINUS :
        out << "MINUS";
        break;
      
      
      case TokenType::PARENTS :
        out << "( ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i)->print(out);
          if (i != u.tokens->size() - 1)
            out << ", ";
        }
        out << " )";
        break;
      
      case TokenType::SQUARES :
        out << "[ ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i)->print(out);
          if (i != u.tokens->size() - 1)
            out << ", ";
        }
        out << " ]";
        break;
      
      case TokenType::BRACKETS :
        out << "{ ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i)->print(out);
          if (i != u.tokens->size() - 1)
            out << ", ";
        }
        out << " }";
        break;
      

      case TokenType::ARROW :
        out << "ARROW";
        break;
      
      
      case TokenType::IDENTIFIER :
        out << "IDENTIFIER(" << u.string << ')';
        break;
      
      case TokenType::SYMBOL :
        out << "SYMBOL(" << u.character << ')';
        break;
      
      case TokenType::LITERAL :
        out << "LITERAL(";
        u.literal.print(out);
        out << ')';
        break;
    }

    out << " [" << line << ']';
  }

  
  bool Token::operator==(Token& right) const {
    return type == right.type;    
  }

  bool Token::typeStringEqual(Token left, Token right) {
    return left.type == right.type && std::string(left.u.string) == std::string(right.u.string);
  }

  bool Token::typeCharEqual(Token left, Token right) {
    return left.type == right.type && left.u.character == right.u.character;
  }

  bool Token::typeLiteralEqual(Token left, Token right) {
    return left.type == right.type && left.u.literal == right.u.literal;
  }
}
