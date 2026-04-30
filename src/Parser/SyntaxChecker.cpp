#include "SyntaxChecker.hpp"

namespace Parser {
  Utils::Error SyntaxChecker::syntaxError(const char* msg) {
    return Parser::syntaxError(msg, getErrorLine());
  }

  Context SyntaxChecker::switchContext(Context newContext) {
    Context context = { tokens, index };
    tokens = newContext.tokens;
    setMaxIndex(tokens->size());
    index = newContext.index;
    return context;
  }

  int SyntaxChecker::getErrorLine() {
    if (hasPeek())
      return peekValue().line;
    else if (hasPeek(-1))
      return peekValue(-1).line;
    else
      return -2; //* Technically it SHOULD be unreachable
  }

  bool SyntaxChecker::wakeup(Token token, TokenType tokenType) {
    return token == Token{ tokenType };
  }

  bool SyntaxChecker::wakeup(TokenType tokenType) {
    return tryConsume({ tokenType });
  }

  bool SyntaxChecker::semi() {
    return wakeup(TokenType::SEMI);
  }

  bool SyntaxChecker::semi(Token token) {
    return wakeup(token, TokenType::SEMI);
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


  Result::inst<Variables> SyntaxChecker::processVariables() {
    Variable* var;
    expectError(Variables, Variable, var, processVariable());

    std::vector<InitIdentifier*>* other = NULL;
    if (wakeup(TokenType::COMMA)) {
      other = new std::vector<InitIdentifier*>();
      InitIdentifier* init;
      
      do {
        expectError(Variables, InitIdentifier, init, processInitIdentifier());
        other->push_back(init);
      } while(wakeup(TokenType::COMMA));
    }

    return Result::success(new Variables{ var, other });
  }

  Result::inst<Variable> SyntaxChecker::processVariable() {
    Type* type;
    expectError(Variable, Type, type, processType());
    
    InitIdentifier* init;
    expectError(Variable, InitIdentifier, init, processInitIdentifier());
    
    return Result::success(new Variable{ type, init });
  }

  Result::inst<Function> SyntaxChecker::processFunction() {
    Identifier* name;
    expectError(Function, Identifier, name, processIdentifier());

    //TODO: PARAMS
    if (!tryConsume({ TokenType::PARENTS }))
      return Result::error<Function>(syntaxError("Expected '('"));

    ReturnType* returnType;
    expectError(Function, ReturnType, returnType, processReturnType());

    if (semi())
      return Result::success(new Function{ name, returnType, false, NULL });

    Scope* scope;
    expectError(Function, Scope, scope, processScope());

    return Result::success(new Function{ name, returnType, true, scope });
  }

  Result::inst<ReturnType> SyntaxChecker::processReturnType() {
    if (tryConsume({ TokenType::VOID }))
      return Result::success(ReturnType::_void());
    
    Result::inst<Type> type = processType();
    if (type.isIgnored())
      return Result::ignore<ReturnType>(syntaxError("Expected return type"));
    else if (type.isError())
      return Result::error<ReturnType>(syntaxError("Expected return type"));
    
    return Result::success(ReturnType::fromType(type.value));
  }

  Result::inst<Scope> SyntaxChecker::processScope() {
    if (peekNotEqual({ TokenType::BRACKETS }))
      return Result::ignore<Scope>(syntaxError("Expected '{'"));
    
    std::vector<Token>* curlyTokens = consume().value().u.tokens;
    Context previous = switchContext(Context::fromTokens(curlyTokens));

    std::vector<Statement*>* statements = new std::vector<Statement*>();
    Result::inst<Statement> statement;
    
    #define ignoresSemi(type) ( type == Statement::Type::NOTHING || type == Statement::Type::SCOPE )

    scopeDepth++;
    while ((statement = processStatement()).hasValue() &&
            ( ignoresSemi(statement.value->type) ||
              (statement = expectSemi(statement)).hasValue() ))
      statements->push_back(statement.value);

    #undef ignoresSemi

    if (statement.isError())
      return Result::error<Scope>(statement.error);
    
    switchContext(previous);
    return Result::success(new Scope{ statements, scopeDepth-- });
  }

  Result::inst<Statement> SyntaxChecker::processStatement() {
    if (!hasPeek())
      return Result::ignore<Statement>(syntaxError("Expected statement"));
    
    if (semi())
      return Result::success(new Statement{ Statement::Type::NOTHING });

    if (wakeup(TokenType::COLON)) {
      Variables* vars;
      expectError(Statement, Variables, vars, processVariables());
      return Result::success(new Statement{ Statement::Type::VAR_DECL, { .vars = vars } });
    }

    if (wakeup(TokenType::RETURN)) {
      Expression* expr = NULL;
      if (!semi())
        expectError(Statement, Expression, expr, processExpression());
      return Result::success(new Statement{ Statement::Type::RETURN, { .expr = expr } });
    }

    Result::inst<Scope> scope;
    if ((scope = processScope()).hasValue())
      return Result::success(new Statement{ Statement::Type::SCOPE, { .scope = scope.value } });
    else if (scope.isError())
      return Result::error<Statement>(scope.error);
    
    Result::inst<Expression> expr;
    if ((expr = processExpression()).hasValue())
      return Result::success(new Statement{ Statement::Type::EXPRESSION, { .expr = expr.value } });
    else if (expr.isError())
      return Result::error<Statement>(expr.error);

    return Result::error<Statement>(syntaxError("Invalid statement"));
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
    Result::inst<Identifier> identifier;

    if (peekEqual({ TokenType::LITERAL }))
      return processLiteralExpression();
    
    else if ((identifier = processIdentifier()).hasValue()) {
      Identifier* var = identifier.value;

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

  Result::inst<InitIdentifier> SyntaxChecker::processInitIdentifier() {
    Identifier* name;
    expectError(InitIdentifier, Identifier, name, processIdentifier());
    
    InitExpression* initExpr = NULL;
    if (wakeup(TokenType::EQUALS))
      expectError(InitIdentifier, InitExpression, initExpr, processInitExpression());
    
    return Result::success(new InitIdentifier{ name, initExpr });
  }


  #undef expectError
  #undef expectIgnore


  void SyntaxChecker::process() {
    while (hasPeek()) {
      Token token = consume().value();
      
      if (semi(token))
        continue; //* Technically not needed (better than ';')
      else if (wakeup(token, TokenType::COLON))
        addToOutput({ GlobalNode::Type::VAR_DECL, { .vars = expectSemi(processVariables()).expectValue() } });
      else if (wakeup(token, TokenType::FUNC))
        addToOutput({ GlobalNode::Type::FUNC, { .func = processFunction().expectValue() } });
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
