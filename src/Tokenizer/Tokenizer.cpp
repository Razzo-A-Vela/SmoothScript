#include "Tokenizer.hpp"

namespace Tokenizer {
  void Tokenizer::processLiteral(PreToken preToken) {
    Literal literal;
    
    if (preToken.type == PreTokenType::NUMBER) {
      literal.type = LiteralType::INTEGER;
      literal.u.integer = std::stoi(std::string(preToken.u.string));
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

        if (str == "function")
          token.type = TokenType::FUNCTION;
        
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
      
      } else if (preToken.type == PreTokenType::NUMBER) {
        processLiteral(preToken);
        continue;
      }

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
