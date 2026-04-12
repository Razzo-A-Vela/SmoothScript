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
    
    InitExpression* initExpr = NULL;
    if (wakeup(TokenType::EQUALS))
      expectError(Variable, InitExpression, initExpr, processInitExpression());
    return Result::success(new Variable{ type, name, initExpr });
  }

  bool SyntaxChecker::identifierPeek() {
    // HACK: I should make a better system to return the already processed identifier instead of processing it again
    int prevIndex = index;
    bool ret = processIdentifier().hasValue();
    index = prevIndex;
    return ret;
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

  Result::inst<InitExpression> SyntaxChecker::processInitExpression() {
    Expression* expr;
    expectError(InitExpression, Expression, expr, processExpression());
    return Result::success(new InitExpression{ InitExpression::Type::EXPRESSION, { .expr = expr } });
  }

  Result::inst<Expression> SyntaxChecker::processExpression() {
    if (peekEqual({ TokenType::LITERAL }))
      return processLiteralExpression();
    
    else if (identifierPeek()) {
      Identifier* var = processIdentifier().expect();

      if (wakeup(TokenType::EQUALS)) {
        Expression* expr;
        expectError(Expression, Expression, expr, processExpression());
        return Result::success(new Expression{ Expression::Type::VAR_ASSIGN, { .varAssign = new VarAssign{ var, expr } }, expr->returnType });
      }

      return Result::success(new Expression{ Expression::Type::VAR, { .var = var }, ReturnType::unknown() });
    }

    return Result::ignore<Expression>(syntaxError("Expected expression"));
  }

  Result::inst<Expression> SyntaxChecker::processLiteralExpression() {
    Token token = consume().value();
    Literal literal = token.u.literal;

    if (literal.type != LiteralType::INTEGER)
      return Result::error<Expression>(Parser::syntaxError("Invalid expression", token.line));
    return Result::success(new Expression{ Expression::Type::LITERAL, { .literal = literal }, ReturnType::fromType(new Type{ Type::TypeT::INT }) });
  }


  #undef expectError
  #undef expectIgnore


  void SyntaxChecker::process() {
    while (hasPeek()) {
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
