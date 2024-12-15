#include "Tokenizer.hpp"

namespace Tokenizer {
  Literal Tokenizer::parseLiteral(std::string str, int line) {
    LiteralParser literalParser(str, line, false);
    literalParser.process();
    return literalParser.getSingleOutput();
  }

  Literal Tokenizer::parseNumLiteral(std::string str, int line) {
    LiteralParser literalParser(str, line, true);
    literalParser.process();
    return literalParser.getSingleOutput();
  }

  void Tokenizer::processSymbol(PreToken preToken) {
    Token token;
    token.line = preToken.line;

    if (preToken.u.character == '<') {
      if (peekEqual({ PreTokenType::symbol, { .character = '>' } }, PreToken::typeCharEqual)) {
        consume();
        token.type = TokenType::byte_type;
        token.u.integer = 0; // VOID BYTE TYPE
        addToOutput(token);
        return;

      } else if (peekEqual({ PreTokenType::numLiteral }, PreToken::typeEqual) &&
          peekEqual({ PreTokenType::symbol, { .character = '>' } }, PreToken::typeCharEqual, 1)) {
        PreToken literalToken = consume().value();
        consume();
        
        token.type = TokenType::byte_type;
        Literal literal = parseNumLiteral(std::string(literalToken.u.string), token.line);
        if (literal.type != LiteralType::integer)
          Utils::error("Syntax Error", "Invalid byte type", token.line);
        if (literal.u.integer == 0)
          Utils::error("Syntax Error", "Cannot make a byte type of size zero", token.line);
        token.u.integer = literal.u.integer;
        addToOutput(token);
        return;
      }
    }

    token.u.character = preToken.u.character;
    switch (preToken.u.character) {
      case ':' :
        token.type = TokenType::colon;
        break;
      
      case ';' :
        token.type = TokenType::semi_colon;
        break;

      case '?' :
        token.type = TokenType::question;
        break;
      
      case '!' :
        token.type = TokenType::exclamation;
        break;

      case '=' :
        token.type = TokenType::equals;
        break;
      
      case '$' :
        token.type = TokenType::dollar;
        break;
      
      case '#' :
        token.type = TokenType::hashtag;
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
      
      case '<' :
        token.type = TokenType::open_angolare;
        break;
      
      case '>' :
        token.type = TokenType::closed_angolare;
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

      } else if (PreToken::typeCharEqual(preToken, { PreTokenType::symbol, { .character = '$' } })) {
        if (peekNotEqual({ PreTokenType::identifier }, PreToken::typeEqual))
          Utils::error("Syntax Error", "Expected Parser Parameter Identifier", token.line);
        
        token.type = TokenType::parserParameter;
        token.u.string = consume().value().u.string;
        addToOutput(token);
        continue;

      } else if (PreToken::typeCharEqual(preToken, { PreTokenType::symbol, { .character = '@' } })) {
        if (peekNotEqual({ PreTokenType::identifier }, PreToken::typeEqual))
          Utils::error("Syntax Error", "Expected ASM Register Identifier", token.line);
        
        token.type = TokenType::asmRegister;
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
      else if (str == "return")
        token.type = TokenType::ret;
      else {
        token.type = TokenType::identifier;
        token.u.string = preToken.u.string;
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
