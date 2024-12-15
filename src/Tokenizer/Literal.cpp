#include "Literal.hpp"

namespace Tokenizer {
  void Literal::print(std::ostream& out) {
    switch (type) {
      case LiteralType::integer :
        out << "INT(" << u.integer << ')';
        break;

      case LiteralType::floating :
        out << "FLOAT(" << u.floating << ')';
        break;

      case LiteralType::byte :
        out << "BYTE(" << (unsigned int) u.byte << ')';
        break;

      case LiteralType::character :
        out << "CHAR(" << u.character << ')';
        break;

      case LiteralType::string :
        out << "STRING(" << std::string(u.string) << ')';
        break;

      case LiteralType::longint :
        out << "LONG(" << u.longint << ')';
        break;

      case LiteralType::doubled :
        out << "DOUBLE(" << u.doubled << ')';
        break;

      case LiteralType::shorted :
        out << "SHORT(" << u.shorted << ')';
        break;
    }
  }


  void LiteralParser::processNumLiteral() {
    Literal literal;
    char prefix = 0;
    bool hasDot = false;
    char suffix = 0;

    if (peekEqual('0') && toParse.size() > 2) {
      consume();
      prefix = Utils::charLower(consume().value());
    }

    int base = 10;
    if (prefix == 'x')
      base = 16;
    else if (prefix == 'b')
      base = 2;
    else if (prefix != 0)
      Utils::error("Syntax Error", "Invalid literal prefix", line);

    std::stringstream stream;
    while (hasPeek()) {
      char c = consume().value();
      if (c == '_')
        continue;

      if (c == '.') {
        if (hasDot || prefix != 0)
          Utils::error("Syntax Error", "Invalid literal", line);
        hasDot = true;

      } else {
        if ((prefix == 'b' && (c != '0' && c != '1')) ||
            (prefix == 'x' && !isxdigit(c)) ||
            (prefix == 0 && !isdigit(c))) {
          if (hasPeek())
            Utils::error("Syntax Error", "Invalid literal", line);
          suffix = Utils::charLower(c);
          break;
        }
      }

      stream << c;
    }
    
    if (hasDot) {
      if (suffix == 'f')
        literal.type = LiteralType::floating;
      else if (suffix == 0)
        literal.type = LiteralType::doubled;
      else
        Utils::error("Syntax Error", "Invalid literal suffix", line);
    } else {
      if (suffix == 'y')
        literal.type = LiteralType::byte;
      else if (suffix == 'l')
        literal.type = LiteralType::longint;
      else if (suffix == 's')
        literal.type = LiteralType::shorted;
      else if (suffix == 0)
        literal.type = LiteralType::integer;
      else
        Utils::error("Syntax Error", "Invalid literal suffix", line);
    }

    //TODO: Fix out of range and truncate to max
    std::string str = stream.str();
    if (literal.type == LiteralType::floating)
      literal.u.floating = std::stof(str);

    else if (literal.type == LiteralType::doubled)
      literal.u.doubled = std::stold(str);
    
    else if (literal.type == LiteralType::byte)
      literal.u.byte = std::stoul(str, NULL, base) & 0xFF;

    else if (literal.type == LiteralType::longint)
      literal.u.longint = std::stoull(str, 0, base);

    else if (literal.type == LiteralType::integer)
      literal.u.integer = std::stoul(str, 0, base);

    else if (literal.type == LiteralType::shorted)
      literal.u.shorted = std::stoul(str, NULL, base) & 0xFFFF;

    addToOutput(literal);
  }

  void LiteralParser::process() {
    if (isNumLiteral) {
      processNumLiteral();
      return;
    }

    Literal literal;
    char quote = consume().value();
    literal.type = quote == '\"' ? LiteralType::string : LiteralType::character;
    
    std::stringstream stream;
    while (hasPeek()) {
      char c = consume().value();
      if (c != '\\') {
        stream << c;
        continue;
      }

      if (!hasPeek()) // Tests for: "... \" -> ("... \) -> removes the ending " so it's invalid
        Utils::error("Syntax Error", "Invalid literal", line);
      char escape = consume().value();
      
      if (escape == '\"' || escape == '\'' || escape == '\\')
        stream << escape;
        
      else if (escape == 'n')
        stream << '\n';

      else if (escape == '0')
        stream << '\0';

      else
        Utils::error("Syntax Error", "Invalid literal escape", line);
    }

    std::string str = stream.str();
    if (literal.type == LiteralType::character) {
      if (str.size() != 1)
        Utils::error("Syntax Error", "Invald character literal", line);

      literal.u.character = str.at(0);
    } else
      literal.u.string = Utils::stringToCString(str);

    addToOutput(literal);
  }

  void LiteralParser::print(std::ostream& out) {
    out << "LITERAL(";
    getSingleOutput().print(out);
    out << ")\n";
  }
}
