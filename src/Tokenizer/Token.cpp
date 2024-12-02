#include "Token.hpp"

namespace Tokenizer {
  void Token::print() {
    switch (type) {
      case TokenType::func :
        std::cout << "FUNC";
        break;

      case TokenType::ret :
        std::cout << "RETURN";
        break;


      case TokenType::colon :
        std::cout << "COLON";
        break;
      
      case TokenType::semi_colon :
        std::cout << "SEMI_COLON";
        break;
        
      case TokenType::question :
        std::cout << "QUESTION";
        break;
        
      case TokenType::exclamation :
        std::cout << "EXCLAMATION";
        break;
      
      case TokenType::equals :
        std::cout << "EQUALS";
        break;
      
      case TokenType::minus :
        std::cout << "MINUS";
        break;
      
      case TokenType::comma :
        std::cout << "COMMA";
        break;
      
      case TokenType::star :
        std::cout << "STAR";
        break;

      
      case TokenType::open_paren :
        std::cout << "OPEN_PAREN";
        break;
      
      case TokenType::closed_paren :
        std::cout << "CLOSED_PAREN";
        break;
      
      case TokenType::open_bracket :
        std::cout << "OPEN_BRACKET";
        break;
      
      case TokenType::closed_bracket :
        std::cout << "CLOSED_BRACKET";
        break;
        
      case TokenType::open_brace :
        std::cout << "OPEN_BRACE";
        break;
        
      case TokenType::closed_brace :
        std::cout << "CLOSED_BRACE";
        break;

        

      case TokenType::identifier :
        std::cout << "IDENTIFIER(" << std::string(u.string) << ')';
        break;

      case TokenType::byte_type :
        std::cout << "BYTE_TYPE(";
        if (u.integer == 0)
          std::cout << "VOID";
        else
          std::cout << u.integer;
        std::cout << ')';
        break;

      case TokenType::literal :
        std::cout << "LITERAL(";
        u.literal.print();
        std::cout << ')';
        break;

      case TokenType::symbol :
        std::cout << "SYMBOL(" << u.character << ')';
        break;

      case TokenType::preProcessor :
        std::cout << "PRE_PROCESSOR(" << std::string(u.string) << ')';
        break;

      case TokenType::parserParameter :
        std::cout << "PARSER_PARAMETER(" << std::string(u.string) << ')';
        break;

      case TokenType::asmRegister :
        std::cout << "ASM_REGISTER(" << std::string(u.string) << ')';
        break;

      
      case TokenType::endPreProcessor :
        std::cout << "END_PRE_PROCESSOR";
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
