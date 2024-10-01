#include "Tokenizer.hpp"

namespace Tokenizer {
  Literal Tokenizer::parseLiteral(std::string str, int line) {
    LiteralParser literalParser(str, line, false);
    literalParser.process();
    return literalParser.getOutput().at(0);
  }

  Literal Tokenizer::parseNumLiteral(std::string str, int line) {
    LiteralParser literalParser(str, line, true);
    literalParser.process();
    return literalParser.getOutput().at(0);
  }

  void Tokenizer::processSymbol(PreToken preToken) {
    Token token;
    token.line = preToken.line;

    if (preToken.u.character == '<' &&
          peekEqual({ PreTokenType::numLiteral }, PreToken::typeEqual) &&
          peekEqual({ PreTokenType::symbol, { .character = '>' } }, PreToken::typeCharEqual, 1)) {
      PreToken literalToken = consume().value();
      consume();
      
      token.type = TokenType::byte_type;
      Literal literal = parseNumLiteral(std::string(literalToken.u.string), token.line);
      if (literal.type != LiteralType::integer)
        Utils::error("Syntax Error", "Invalid byte type", token.line);
      token.u.integer = literal.u.integer;

    } else {
      token.u.character = preToken.u.character;
      switch (preToken.u.character) {
        case ':' :
          if (peekEqual({ PreTokenType::symbol, { .character = ':' } }, PreToken::typeCharEqual)) {
            consume();
            token.type = TokenType::double_colon;
          } else
            token.type = TokenType::colon;
          break;
        
        case ';' :
          token.type = TokenType::semi_colon;
          break;

        case '?' :
          if (peekEqual({ PreTokenType::symbol, { .character = '?' } }, PreToken::typeCharEqual)) {
            consume();
            token.type = TokenType::double_question;
          } else
            token.type = TokenType::question;
          break;
        
        case '!' :
          token.type = TokenType::exclamation;
          break;

        case '=' :
          token.type = TokenType::equals;
          break;
        
        case '-' :
          token.type = TokenType::minus;
          break;
        
        case ',' :
          token.type = TokenType::comma;
          break;
        
        case '*' :
          token.type = TokenType::star;
          break;
        
        
        case '(' :
          token.type = TokenType::open_paren;
          break;
        
        case ')' :
          token.type = TokenType::closed_paren;
          break;
          
        case '[' :
          token.type = TokenType::open_bracket;
          break;
        
        case ']' :
          token.type = TokenType::closed_bracket;
          break;
          
        case '{' :
          token.type = TokenType::open_brace;
          break;
        
        case '}' :
          token.type = TokenType::closed_brace;
          break;

        default :
          token.type = TokenType::symbol;
          break;
      }
    }

    addToOutput(token);
  }

  void Tokenizer::process() {
    while (hasPeek()) {
      PreToken preToken = consume().value();
      Token token;
      token.line = preToken.line;

      if (PreToken::typeCharEqual(preToken, { PreTokenType::symbol, { .character = '#' } })) {
        if (peekNotEqual({ PreTokenType::identifier }, PreToken::typeEqual))
          Utils::error("Syntax Error", "Expected PreProcessor Identifier", token.line);
        
        token.type = TokenType::preProcessor;
        token.u.string = consume().value().u.string;
        addToOutput(token);
        continue;

      } else if (PreToken::typeEqual(preToken, { PreTokenType::symbol }) ) {
        processSymbol(preToken);
        continue;

      } else if (PreToken::typeEqual(preToken, { PreTokenType::literal })) {
        token.type = TokenType::literal;
        token.u.literal = parseLiteral(std::string(preToken.u.string), token.line);
        
        addToOutput(token);
        continue;
        
      } else if (PreToken::typeEqual(preToken, { PreTokenType::numLiteral })) {
        token.type = TokenType::literal;
        token.u.literal = parseNumLiteral(std::string(preToken.u.string), token.line);

        addToOutput(token);
        continue;

      } else if (PreToken::typeEqual(preToken, { PreTokenType::endPreProcessor })) {
        token.type = TokenType::endPreProcessor;
        addToOutput(token);
        continue;
      }

      std::string str = std::string(preToken.u.string);
      if (str == "func")
        token.type = TokenType::func;
      else if (str == "proc")
        token.type = TokenType::proc;
      else if (str == "return")
        token.type = TokenType::ret;
      else if (str == "struct")
        token.type = TokenType::structu;
      else if (str == "union")
        token.type = TokenType::unioni;
      else {
        token.type = TokenType::identifier;
        token.u.string = preToken.u.string;
      }
      addToOutput(token);
    }
  }

  void Tokenizer::print() {
    std::vector<Token> output = getOutput();
    for (Token token : output) {
      token.print();
      std::cout << '\n';
    }
  }
}
