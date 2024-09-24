#include "Token.hpp"

namespace Tokenizer {
  void Token::print() {
    switch (type) {
      case TokenType::func :
        std::cout << "FUNC";
        break;
      
      case TokenType::proc :
        std::cout << "PROC";
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
        std::cout << "BYTE_TYPE(" << u.integer << ')';
        break;

      case TokenType::literal :
        std::cout << "LITERAL(";
        u.literal.print();
        std::cout << ')';
        break;

      case TokenType::symbol :
        std::cout << "SYMBOL(" << u.character << ')';
        break;
    }
  }
}
