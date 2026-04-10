#include "Token.hpp"

namespace Tokenizer {
  void Token::print(std::ostream& out) {
    switch (type) {
      case TokenType::FUN :
        out << "FUN";
        break;
      
      case TokenType::RETURN :
        out << "RETURN";
        break;
      
      case TokenType::IF :
        out << "IF";
        break;
      
      case TokenType::ELSE :
        out << "ELSE";
        break;

      
      case TokenType::INT :
        out << "INT";
        break;
      
      case TokenType::FLOAT :
        out << "FLOAT";
        break;
      
      case TokenType::BOOL :
        out << "BOOL";
        break;
      
      case TokenType::CHAR :
        out << "CHAR";
        break;
      
      case TokenType::CSTR :
        out << "CSTR";
        break;
      
      case TokenType::SIZE_T :
        out << "SIZE_T";
        break;
      
      case TokenType::VOID :
        out << "VOID";
        break;
      

      case TokenType::SEMI :
        out << "SEMI";
        break;
      
      case TokenType::COLON :
        out << "COLON";
        break;

      case TokenType::MINUS :
        out << "MINUS";
        break;

      case TokenType::EQUALS :
        out << "EQUALS";
        break;
      

      case TokenType::PARENTS :
        out << "( ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i).print(out);
          if (i != u.tokens->size() - 1)
            out << ", ";
        }
        out << " )";
        break;
      
      case TokenType::SQUARES :
        out << "[ ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i).print(out);
          if (i != u.tokens->size() - 1)
            out << ", ";
        }
        out << " ]";
        break;
      
      case TokenType::BRACKETS :
        out << "{ ";
        for (int i = 0; i < u.tokens->size(); i++) {
          u.tokens->at(i).print(out);
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
