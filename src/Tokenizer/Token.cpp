#include "Token.hpp"

namespace Tokenizer {
  void Token::print(std::ostream& out) {
    switch (type) {
      case TokenType::func :
        out << "FUNC";
        break;

      case TokenType::ret :
        out << "RETURN";
        break;


      case TokenType::colon :
        out << "COLON";
        break;

      case TokenType::dollar :
        out << "DOLLAR";
        break;

      case TokenType::hashtag :
        out << "HASHTAG";
        break;

      case TokenType::at :
        out << "AT(@)";
        break;
      
      case TokenType::semi_colon :
        out << "SEMI_COLON";
        break;
        
      case TokenType::question :
        out << "QUESTION";
        break;
        
      case TokenType::exclamation :
        out << "EXCLAMATION";
        break;
      
      case TokenType::equals :
        out << "EQUALS";
        break;
      
      case TokenType::minus :
        out << "MINUS";
        break;
      
      case TokenType::comma :
        out << "COMMA";
        break;
      
      case TokenType::star :
        out << "STAR";
        break;

      
      case TokenType::open_paren :
        out << "OPEN_PAREN";
        break;
      
      case TokenType::closed_paren :
        out << "CLOSED_PAREN";
        break;
      
      case TokenType::open_bracket :
        out << "OPEN_BRACKET";
        break;
      
      case TokenType::closed_bracket :
        out << "CLOSED_BRACKET";
        break;
        
      case TokenType::open_brace :
        out << "OPEN_BRACE";
        break;
        
      case TokenType::closed_brace :
        out << "CLOSED_BRACE";
        break;

        

      case TokenType::identifier :
        out << "IDENTIFIER(" << std::string(u.string) << ')';
        break;

      case TokenType::byte_type :
        out << "BYTE_TYPE(";
        if (u.integer == 0)
          out << "VOID";
        else
          out << u.integer;
        out << ')';
        break;

      case TokenType::literal :
        out << "LITERAL(";
        u.literal.print(out);
        out << ')';
        break;

      case TokenType::symbol :
        out << "SYMBOL(" << u.character << ')';
        break;

      case TokenType::preProcessor :
        out << "PRE_PROCESSOR(" << std::string(u.string) << ')';
        break;

      case TokenType::parserParameter :
        out << "PARSER_PARAMETER(" << std::string(u.string) << ')';
        break;

      case TokenType::asmRegister :
        out << "ASM_REGISTER(" << std::string(u.string) << ')';
        break;

      
      case TokenType::endPreProcessor :
        out << "END_PRE_PROCESSOR";
        break;
    }
  }

  const std::function<bool(Token, Token)> Token::typeEqual = [](Token token, Token toEqual) { return token.type == toEqual.type; };
  const std::function<bool(Token, Token)> Token::typeIntEqual = [](Token token, Token toEqual) { return token.type == toEqual.type && token.u.integer == toEqual.u.integer; };
  const std::function<bool(Token, Token)> Token::typeCharEqual = [](Token token, Token toEqual) { return token.type == toEqual.type && token.u.character == toEqual.u.character; };
  const std::function<bool(Token, Token)> Token::typeStringEqual = [](Token token, Token toEqual) { return token.type == toEqual.type && std::string(token.u.string) == std::string(toEqual.u.string); };
  const std::function<bool(Token)> Token::isSymbol = [](Token token) { return ((int) token.type >= (int) TokenType::equals && (int) token.type <= (int) TokenType::closed_brace) || Token::typeEqual(token, { TokenType::symbol }); };
  const std::function<bool(Token)> Token::isValidOperatorToken = [](Token token) { return Token::isSymbol(token) && !((int) token.type >= (int) TokenType::colon && (int) token.type <= (int) TokenType::closed_brace); };
}
