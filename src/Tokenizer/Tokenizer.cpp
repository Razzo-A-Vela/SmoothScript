#include "Tokenizer.hpp"

namespace Tokenizer {
  Token* Tokenizer::processLiteral(PreToken preToken) {
    Token* ret = new Token();
    ret->type = TokenType::LITERAL;
    ret->line = preToken.line;
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
          
          else if (c == '\\')
            toModify << '\\';
          
          else if (c == '\"')
            toModify << '\"';
          
          else if (c == '\'')
            toModify << '\'';
          
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
        
      else if (toCheck == "\\\"")
        character = '\"';
        
      else if (toCheck == "\\\'")
        character = '\'';
      
      else
        Utils::error("Syntax Error", "Invalid charLiteral", preToken.line);
      
      literal.u.character = character;
    }

    ret->u.literal = literal;
    return ret;
  }

  Token* Tokenizer::processToken(PreToken preToken) {
    Token* ret = new Token();
    ret->line = preToken.line;
    
    if (preToken.type == PreTokenType::SPACES)
      return NULL;
    
    else if (preToken.type == PreTokenType::IDENTIFIER) {
      std::string str = std::string(preToken.u.string);

      if (str == "func")
        ret->type = TokenType::FUNC;
      
      else if (str == "return")
        ret->type = TokenType::RETURN;
      
      else if (str == "if")
        ret->type = TokenType::IF;
      
      else if (str == "else")
        ret->type = TokenType::ELSE;
      
      else if (str == "while")
        ret->type = TokenType::WHILE;
      
      else if (str == "do")
        ret->type = TokenType::DO;
      
      else if (str == "loop")
        ret->type = TokenType::LOOP;
      
      else if (str == "break")
        ret->type = TokenType::BREAK;
      
      else if (str == "continue")
        ret->type = TokenType::CONTINUE;
      
      else if (str == "for")
        ret->type = TokenType::FOR;
      
      else if (str == "goto")
        ret->type = TokenType::GOTO;
      
      
      else if (str == "int")
        ret->type = TokenType::INT;
      
      else if (str == "float")
        ret->type = TokenType::FLOAT;
      
      else if (str == "bool")
        ret->type = TokenType::BOOL;
      
      else if (str == "cstr")
        ret->type = TokenType::CSTR;
      
      else if (str == "char")
        ret->type = TokenType::CHAR;
      
      else if (str == "size_t")
        ret->type = TokenType::SIZE_T;

      else if (str == "const")
        ret->type = TokenType::CONST;

      else if (str == "signed")
        ret->type = TokenType::SIGNED;

      else if (str == "unsigned")
        ret->type = TokenType::UNSIGNED;

      else if (str == "void")
        ret->type = TokenType::VOID;


      else {
        ret->type = TokenType::IDENTIFIER;
        ret->u.string = preToken.u.string;
      }
    
    } else if (preToken.type == PreTokenType::SYMBOL) {
      char c = preToken.u.character;

      if (c == '(') {
        ret->type = TokenType::PARENTS;
        ret->u.tokens = new std::vector<Token>();
        while (!tryConsume({ PreTokenType::SYMBOL, { .character = ')' } }, PreToken::typeCharEqual)) {
          if (!hasPeek())
            Utils::error("Expected ')' after '('", preToken.line);
          
          Token* token = processToken(consume().value());
          if (token != NULL)
            ret->u.tokens->push_back(*token);
        }

      } else if (c == '[') {
        ret->type = TokenType::SQUARES;
        ret->u.tokens = new std::vector<Token>();
        while (!tryConsume({ PreTokenType::SYMBOL, { .character = ']' } }, PreToken::typeCharEqual)) {
          if (!hasPeek())
            Utils::error("Expected ']' after '['", preToken.line);
          
          Token* token = processToken(consume().value());
          if (token != NULL)
            ret->u.tokens->push_back(*token);
        }

      } else if (c == '{') {
        ret->type = TokenType::BRACKETS;
        ret->u.tokens = new std::vector<Token>();
        while (!tryConsume({ PreTokenType::SYMBOL, { .character = '}' } }, PreToken::typeCharEqual)) {
          if (!hasPeek())
            Utils::error("Expected '}' after '{'", preToken.line);
          
          Token* token = processToken(consume().value());
          if (token != NULL)
            ret->u.tokens->push_back(*token);
        }

      } else switch (c) {
        case ';' :
          ret->type = TokenType::SEMI;
          break;
        
        case ':' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = ':' } }, PreToken::typeCharEqual))
            ret->type = TokenType::DOUBLE_COLON;
          else
            ret->type = TokenType::COLON;
          break;
        
        case '+' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '+' } }, PreToken::typeCharEqual))
            ret->type = TokenType::PLUSPLUS;
          else
            ret->type = TokenType::PLUS;
          break;

        case '-' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '>' } }, PreToken::typeCharEqual))
            ret->type = TokenType::ARROW;
          else if (tryConsume({ PreTokenType::SYMBOL, { .character = '-' } }, PreToken::typeCharEqual))
            ret->type = TokenType::MINUSMINUS;
          else
            ret->type = TokenType::MINUS;
          break;
        
        case '=' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '=' } }, PreToken::typeCharEqual))
            ret->type = TokenType::DOUBLE_EQUALS;
          else
            ret->type = TokenType::EQUALS;
          break;
        
        case ',' :
          ret->type = TokenType::COMMA;
          break;
        
        case '!' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '=' } }, PreToken::typeCharEqual))
            ret->type = TokenType::NOT_EQUAL;
          else
            ret->type = TokenType::EXCLAMATION;
          break;

        case '*' :
          ret->type = TokenType::ASTERISK;
          break;

        case '/' :
          ret->type = TokenType::SLASH;
          break;
        
        case '<' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '=' } }, PreToken::typeCharEqual))
            ret->type = TokenType::LESS_EQ;
          else if (tryConsume({ PreTokenType::SYMBOL, { .character = '<' } }, PreToken::typeCharEqual))
            ret->type = TokenType::SHIFT_LEFT;
          else
            ret->type = TokenType::LESS;
          break;
        
        case '>' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '=' } }, PreToken::typeCharEqual))
            ret->type = TokenType::GREATER_EQ;
          else if (tryConsume({ PreTokenType::SYMBOL, { .character = '>' } }, PreToken::typeCharEqual))
            ret->type = TokenType::SHIFT_RIGHT;
          else
            ret->type = TokenType::GREATER;
          break;
        
        case '&' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '&' } }, PreToken::typeCharEqual))
            ret->type = TokenType::AND;
          else
            ret->type = TokenType::AMPERSAND;
          break;
        
        case '|' :
          if (tryConsume({ PreTokenType::SYMBOL, { .character = '|' } }, PreToken::typeCharEqual))
            ret->type = TokenType::OR;
          else
            ret->type = TokenType::PIPE;
          break;
        
        case '~' :
          ret->type = TokenType::TILDE;
          break;
        

        default :
          ret->type = TokenType::SYMBOL;
          ret->u.character = c;
          break;
      }
    
    } else if (preToken.type == PreTokenType::NUMBER || preToken.type == PreTokenType::STRING_LITERAL || preToken.type == PreTokenType::CHAR_LITERAL)
      return processLiteral(preToken);
    
    return ret;
  }

  void Tokenizer::process() {
    while (hasPeek()) {
      Token* token = processToken(consume().value());
      if (token != NULL)
        addToOutput(*token);
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
