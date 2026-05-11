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
      
      case TokenType::IF :
        out << "IF";
        break;
      
      case TokenType::ELSE :
        out << "ELSE";
        break;
      
      case TokenType::WHILE :
        out << "WHILE";
        break;
      
      case TokenType::DO :
        out << "DO";
        break;
      
      case TokenType::LOOP :
        out << "LOOP";
        break;
      
      case TokenType::BREAK :
        out << "BREAK";
        break;
      
      case TokenType::CONTINUE :
        out << "CONTINUE";
        break;
      
      case TokenType::FOR :
        out << "FOR";
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
      
      case TokenType::CONST :
        out << "CONST";
        break;
      
      case TokenType::SIGNED :
        out << "SIGNED";
        break;
      
      case TokenType::UNSIGNED :
        out << "UNSIGNED";
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

      case TokenType::PLUS :
        out << "PLUS";
        break;

      case TokenType::EQUALS :
        out << "EQUALS";
        break;
      
      case TokenType::COMMA :
        out << "COMMA";
        break;
      
      case TokenType::EXCLAMATION :
        out << "EXCLAMATION";
        break;
      
      case TokenType::ASTERISK :
        out << "ASTERISK";
        break;
      
      case TokenType::SLASH :
        out << "SLASH";
        break;
      
      case TokenType::LESS :
        out << "LESS";
        break;
      
      case TokenType::GREATER :
        out << "GREATER";
        break;
      
      case TokenType::AMPERSAND :
        out << "AMPERSAND";
        break;
      
      case TokenType::PIPE :
        out << "PIPE";
        break;
      
      case TokenType::TILDE :
        out << "TILDE";
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
      
      case TokenType::PLUSPLUS :
        out << "PLUSPLUS";
        break;
      
      case TokenType::MINUSMINUS :
        out << "MINUSMINUS";
        break;
      
      case TokenType::DOUBLE_EQUALS :
        out << "DOUBLE_EQUALS";
        break;
      
      case TokenType::NOT_EQUAL :
        out << "NOT_EQUAL";
        break;
      
      case TokenType::LESS_EQ :
        out << "LESS_EQ";
        break;
      
      case TokenType::GREATER_EQ :
        out << "GREATER_EQ";
        break;
      
      case TokenType::SHIFT_LEFT :
        out << "SHIFT_LEFT";
        break;
      
      case TokenType::SHIFT_RIGHT :
        out << "SHIFT_RIGHT";
        break;
      
      case TokenType::AND :
        out << "AND";
        break;
      
      case TokenType::OR :
        out << "OR";
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
