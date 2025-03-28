#include "Tokenizer.hpp"

namespace Tokenizer {
  void Tokenizer::processLiteral(PreToken preToken) {
    Literal literal;
    
    if (preToken.type == PreTokenType::NUMBER) {
      literal.type = LiteralType::INTEGER;
      literal.u.integer = std::stoi(std::string(preToken.u.string));
    
    } else if (preToken.type == PreTokenType::STRING_LITERAL) {
      literal.type = LiteralType::STRING;
      std::string toCheck = std::string(preToken.u.string);
      std::stringstream toModify;

      bool foundBackslash = false;
      for (char c : toCheck) {
        if (c == '\\') {
          foundBackslash = true;
          continue;
        }

        if (foundBackslash) {
          foundBackslash = false;

          if (c == 'n')
            toModify << '\n';
          
          else if (c == 'r')
            toModify << '\r';
          
          else if (c == 't')
            toModify << '\t';
          
          else if (c == '0')
            toModify << '\0';
          
          else if (c == '0')
            toModify << '\0';
          
          else if (c == '\\')
            toModify << '\\';
          
          else
            Utils::error("Syntax Error", "Invalid stringLiteral", preToken.line);
        
        } else
          toModify << c;
      }

      literal.u.string = Utils::stringToCString(toModify.str());

    } else if (preToken.type == PreTokenType::CHAR_LITERAL) {
      literal.type = LiteralType::CHAR;
      std::string toCheck = std::string(preToken.u.string);
      char character;

      if (toCheck.size() == 1)
        character = toCheck.at(0);

      else if (toCheck == "\\n")
        character = '\n';
        
      else if (toCheck == "\\r")
        character = '\r';
        
      else if (toCheck == "\\t")
        character = '\t';
        
      else if (toCheck == "\\0")
        character = '\0';
        
      else if (toCheck == "\\\\")
        character = '\\';
      
      else
        Utils::error("Syntax Error", "Invalid charLiteral", preToken.line);
      
      literal.u.character = character;
    }

    addToOutput({ TokenType::LITERAL, { .literal = literal }, preToken.line });
  }

  void Tokenizer::process() {
    while (hasPeek()) {
      PreToken preToken = consume().value();
      Token token;
      token.line = preToken.line;

      if (preToken.type == PreTokenType::IDENTIFIER) {
        std::string str = std::string(preToken.u.string);

        if (str == "func")
          token.type = TokenType::FUNC;
        
        else if (str == "impl")
          token.type = TokenType::IMPL;
        
        else if (str == "return")
          token.type = TokenType::RETURN;
        
        else {
          token.type = TokenType::IDENTIFIER;
          token.u.string = preToken.u.string;
        }
      
      } else if (preToken.type == PreTokenType::SYMBOL) {
        char c = preToken.u.character;

        if (c == ';')
          token.type = TokenType::SEMI;
        
        else if (c == '(')
          token.type = TokenType::OPEN_PAREN;

        else if (c == ')')
          token.type = TokenType::CLOSED_PAREN;

        else if (c == '[')
          token.type = TokenType::OPEN_SQUARE;

        else if (c == ']')
          token.type = TokenType::CLOSED_SQUARE;

        else if (c == '{')
          token.type = TokenType::OPEN_BRACKET;

        else if (c == '}')
          token.type = TokenType::CLOSED_BRACKET;
        
        else if (c == '-') {
          if (peekEqual({ PreTokenType::SYMBOL, { .character = '>' } }, PreToken::typeCharEqual)) {
            consume();
            token.type = TokenType::ARROW;
          
          } else
            token.type = TokenType::MINUS;
        
        } else {
          token.type = TokenType::SYMBOL;
          token.u.character = c;
        }
      
      } else if (preToken.type == PreTokenType::NUMBER || preToken.type == PreTokenType::STRING_LITERAL || preToken.type == PreTokenType::CHAR_LITERAL) {
        processLiteral(preToken);
        continue;
      
      } else if (preToken.type == PreTokenType::SPACES)
        continue;

      else if (preToken.type == PreTokenType::PRE_PROCESSOR)
        Utils::error("Internal Error", "Unexpected preProcessor inside Tokenizer", preToken.line);
      
      else if (preToken.type == PreTokenType::END_PRE_PROCESSOR)
        Utils::error("Internal Error", "Unexpected endPreProcessor inside Tokenizer", preToken.line);

      addToOutput(token);
    }
  }

  void Tokenizer::print(std::ostream& out) {
    std::vector<Token> output = getOutput();

    for (Token token : output) {
      token.print(out);
      out << '\n';
    }
  }
}
