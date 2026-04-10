#include "SyntaxChecker.hpp"

namespace Parser {
  Utils::Error SyntaxChecker::syntaxError(const char* msg) {
    return Parser::syntaxError(msg, getErrorLine());
  }

  int SyntaxChecker::getErrorLine() {
    if (hasPeek())
      return peekValue().line;
    else if (hasPeek(-1))
      return peekValue(-1).line;
    else
      return 1; //* Technically it should be unreachable
  }

  bool SyntaxChecker::wakeup(Token token, TokenType tokenType) {
    return token == Token{ tokenType };
  }

  bool SyntaxChecker::wakeup(TokenType tokenType) {
    return tryConsume({ tokenType });
  }


  #define expectError(retType, type, result, function) \
    { Result::inst<type> result##_result = function; \
    if (!result##_result.hasValue()) \
      return Result::error<retType>(result##_result.error); \
    result = result##_result.value; } 0
  
  #define expectIgnore(retType, type, result, function) \
    { Result::inst<type> result##_result = function; \
    if (!result##_result.hasValue()) \
      return Result::ignore<retType>(result##_result.error); \
    result = result##_result.value; } 0


  Result::inst<Variable> SyntaxChecker::processVariable() {
    Type* type;
    Identifier* name;
    expectError(Variable, Type, type, processType());
    expectError(Variable, Identifier, name, processIdentifier());
    return Result::success(new Variable{ type, name, NULL });
  }

  Result::inst<Identifier> SyntaxChecker::processIdentifier() {
    if (peekEqual({ TokenType::IDENTIFIER }))
      return Result::success(new Identifier{ consume().value().u.string });
    return Result::ignore<Identifier>(syntaxError("Expected identifier"));
  }

  Result::inst<Type> SyntaxChecker::processType() {
    if (tryConsume({ TokenType::INT }))
      return Result::success(new Type{ Type::TypeT::INT });
    return Result::ignore<Type>(syntaxError("Expected type"));
  }


  #undef expectError
  #undef expectIgnore


  void SyntaxChecker::process() {
    while(hasPeek()) {
      Token token = consume().value();
      
      if (wakeup(token, TokenType::SEMI))
        continue; //* Technically not needed (better than ';')
      else if (wakeup(token, TokenType::COLON))
        addToOutput({ GlobalNode::Type::VAR, { .var = expectSemi(processVariable()).expect() } });
      else
        Utils::error(syntaxError("Unexpected token"));
    }
  }

  void SyntaxChecker::print(std::ostream& out) {
    std::vector<GlobalNode> output = getOutput();

    for (GlobalNode globalNode : output) {
      globalNode.print(out);
      out << '\n';
    }
  }
}
