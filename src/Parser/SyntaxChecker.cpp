#include "SyntaxChecker.hpp"

namespace Parser {
  Result::inst<Type> SyntaxChecker::processType() {
    PushConsumes pushConsumes(this, PushConsumes::Ok);

    if (!hasPeek()) {
      pushConsumes.notOk();
      return Result::error<Type>("Expected type");
    }

    Token token = consume().value();
    if (token.type == TokenType::INT32)
      return Result::success(new Type{ Type::TypeT::INT, { .intSize = IntSize::INT32 } });

    pushConsumes.notOk();
    return Result::error<Type>("Invalid token for type");
  }

  Result::inst<ReturnType> SyntaxChecker::processReturnType() {
    PushConsumes pushConsumes(this);
    if (!hasPeek())
      return Result::error<ReturnType>("Expected returnType");

    if (tryConsume({ TokenType::VOID })) {
      pushConsumes.ok();
      return Result::success(new ReturnType{ true, NULL });
    }
    
    Result::inst<Type> type = processType();
    if (Result::isError(type))
      return Result::error<ReturnType>("Invalid returnType");
    
    pushConsumes.ok();
    return Result::success(new ReturnType{ false, type.value });
  }

  Result::inst<Variable> SyntaxChecker::processVariable() {
    PushConsumes pushConsumes(this);
    Result::inst<Type> varType = processType();
    if (Result::isError(varType))
      return Result::ignore<Variable>();   //? Invalid type for variable

    if (!tryConsume({ TokenType::COLON }))
      return Result::ignore<Variable>(2);   //? Expected ':' after type

    if (peekNotEqual({ TokenType::IDENTIFIER }))
      return Result::error<Variable>("Expected identifier after ':'");
    const char* name = consume().value().u.string;

    Variable* var = new Variable();
    var->initialValue = NULL;
    if (tryConsume({ TokenType::EQUALS })) {
      Result::inst<Expression> expr = _processExpression();
      if (Result::isError(expr))
        return Result::error<Variable>(expr.error, expr.line);
      
      var->initialValue = expr.value;
    }

    if (!tryConsume({ TokenType::SEMI }))
      return Result::error<Variable>("Expected ';' after identifier");

    pushConsumes.ok();
    var->type = varType.value;
    var->name = name;
    return Result::success(var);
  }

  Result::inst<Function> SyntaxChecker::processFunction() {
    PushConsumes pushConsumes(this);
    if (!hasPeek() || !tryConsume({ TokenType::FUNC }))
      return Result::error<Function>("Expected 'func'");
    
    if (peekNotEqual({ TokenType::IDENTIFIER }))
      return Result::error<Function>("Expected identifier");
    const char* name = consume().value().u.string;
    
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::error<Function>("Expected (");
    consume();    //TODO: PARAMS

    if (!tryConsume({ TokenType::ARROW }))
      return Result::error<Function>("Expected '->'");
    
    if (!hasPeek())
      return Result::error<Function>("Expected return type");
    Result::inst<ReturnType> returnType = processReturnType();
    if (Result::isError(returnType))
      return Result::error<Function>("Expected return type");
    
    if (!hasPeek())
      return Result::error<Function>("Expected scope");
    Result::inst<Scope> scope = processScope();
    if (Result::isError(scope))
      return Result::error<Function>(scope.error, scope.line);
    if (!Result::hasValue(scope))
      Result::error<Scope>("Expected '{'");
    
    pushConsumes.ok();
    Function* function = new Function();
    function->name = name;
    function->returnType = returnType.value;
    function->scope = scope.value;
    return Result::success(function);
  }

  Result::inst<Scope> SyntaxChecker::processScope() {
    PushConsumes pushConsumes(this);
    if (!hasPeek())
      return Result::error<Scope>("Expected '{'");
    Token token = consume().value();

    //TODO: asm and unsafe scopes
    if (token.type != TokenType::BRACKETS)
      return Result::ignore<Scope>();
    std::vector<Token> savedTokens;
    int savedIndex = pushParents(&savedTokens, token.u.tokens);
    scopeDepth++;
    
    int line = -1;
    const char* error = NULL;
    Scope* scope = new Scope{ new std::vector<Statement*>(), scopeDepth };

    while (hasPeek()) {
      Result::inst<Statement> statement = _processStatement();
      if (Result::isError(statement)) {
        error = statement.error;
        line = statement.line;
        break;
      }
      scope->statements->push_back(statement.value);
    }

    scopeDepth--;
    popParents(&savedTokens, savedIndex);
    if (error != NULL)
      return Result::error<Scope>(error, line);
    
    pushConsumes.ok();
    return Result::success(scope);
  }

  Result::inst<Statement> SyntaxChecker::_processStatementVariable() {
    PushConsumes pushConsumes(this);
    Result::inst<Variable> var = processVariable();
    if (Result::isError(var))
      return Result::error<Statement>(var.error, var.line);
    if (!Result::hasValue(var))
      return Result::ignore<Statement>();

    pushConsumes.ok();
    return Result::success(new Statement{ Statement::Type::VAR_DECL, { .var = var.value } });
  }

  Result::inst<Statement> SyntaxChecker::_processStatementScope() {
    PushConsumes pushConsumes(this);

    Result::inst<Scope> scope = processScope();
    if (Result::isError(scope))
      return Result::error<Statement>(scope.error, scope.line);
    if (!Result::hasValue(scope))
      return Result::ignore<Statement>();
    
    pushConsumes.ok();
    return Result::success(new Statement{ Statement::Type::SCOPE, { .scope = scope.value } });
  }

  Result::inst<Statement> SyntaxChecker::_processStatementIf() {
    PushConsumes pushConsumes(this);

    if (!tryConsume({ TokenType::IF}))
      return Result::ignore<Statement>();
    
    Token token = consume().value();
    if (token.type != TokenType::PARENTS)
      return Result::error<Statement>("Expected '('");
    std::vector<Token> savedTokens;
    int savedIndex = pushParents(&savedTokens, token.u.tokens);
    
    Result::inst<Expression> expr = _processExpression();
    popParents(&savedTokens, savedIndex);
    if (Result::isError(expr))
      return Result::error<Statement>(expr.error, expr.line);

    Result::inst<Statement> statement = _processStatement();
    if (Result::isError(statement))
      return statement;

    pushConsumes.ok();
    return Result::success(new Statement{ Statement::Type::IF, { .statementAndExpr = new StatementAndExpr{ statement.value, expr.value } } });
  }

  Result::inst<Statement> SyntaxChecker::_processStatementExpression() {
    PushConsumes pushConsumes(this);

    Result::inst<Expression> expr = _processExpression();
    if (Result::isError(expr))
      return Result::ignore<Statement>();
    
    if (!tryConsume({ TokenType::SEMI }))
      return Result::error<Statement>("Expected ';'");

    pushConsumes.ok();
    return Result::success(new Statement{ Statement::Type::EXPRESSION, { .expr = expr.value } });
  }

  Result::inst<Statement> SyntaxChecker::_processStatementReturn() {
    PushConsumes pushConsumes(this);

    if (!tryConsume({ TokenType::RETURN }))
      return Result::ignore<Statement>();

    if (tryConsume({ TokenType::SEMI })) {
      pushConsumes.ok();
      return Result::success(new Statement{ Statement::Type::RETURN, { .expr = NULL } });
    }

    Result::inst<Expression> expr = _processExpression();
    if (Result::isError(expr))
      return Result::error<Statement>(expr.error, expr.line);
    
    if (!tryConsume({ TokenType::SEMI }))
      return Result::error<Statement>("Expected ';'");

    pushConsumes.ok();
    return Result::success(new Statement{ Statement::Type::RETURN, { .expr = expr.value } });
  }

  Result::inst<Statement> SyntaxChecker::_processStatement() {
    PushConsumes pushConsumes(this);
    if (!hasPeek())
      return Result::error<Statement>("Expected statement (IMPOSSIBLE)"); //? Technically impossible
    
    int errLine = peekValue().line;
    Result::inst<Statement> ret;
    
    if (tryConsume({ TokenType::SEMI })) {
      pushConsumes.ok();
      return Result::success(new Statement{ Statement::Type::NOTHING });
    }

    #define statement(funcName) ret = funcName(); \
                                if (Result::isError(ret)) { \
                                  if (!Result::hasLine(ret)) \
                                    return Result::error<Statement>(ret.error, errLine); \
                                  else \
                                    return ret; \
                                } \
                                if (Result::hasValue(ret)) { \
                                  pushConsumes.ok(); \
                                  return ret; \
                                }

    statement(_processStatementIf);
    statement(_processStatementReturn);
    statement(_processStatementScope);
    statement(_processStatementVariable);
    statement(_processStatementExpression);

    #undef statement

    return Result::error<Statement>("Invalid Statement", errLine);
  }

  Result::inst<Expression> SyntaxChecker::_processExpressionAssign() {
    PushConsumes pushConsumes(this);
    //TODO: dereference, parents and index (basically lvalue)
    
    if (peekNotEqual({ TokenType::IDENTIFIER }))
      return Result::ignore<Expression>();
    const char* varName = consume().value().u.string;

    if (!tryConsume({ TokenType::EQUALS }))
      return Result::ignore<Expression>();
    
    Result::inst<Expression> expr = _processExpression();
    if (Result::isError(expr))
      return Result::error<Expression>(expr.error, expr.line);
    if (expr.value->returnType->isVoid)
      return Result::error<Expression>("Expression cannot be void");

    pushConsumes.ok();
    return Result::success(new Expression{ Expression::Type::VAR_ASSIGN, { .varAssign = new VarAssign{ varName, expr.value } }, expr.value->returnType });
  }
  
  Result::inst<Expression> SyntaxChecker::_processExpressionLiteral() {
    PushConsumes pushConsumes(this);

    // TODO: Prefix
    if (peekNotEqual({ TokenType::LITERAL }))
      return Result::ignore<Expression>();
    
    Token token = consume().value();
    ReturnType* returnType = literalToReturnType(token.u.literal);
    if (returnType == NULL)
      return Result::error<Expression>("Cannot transform literal to returnType (INTERNAL)", token.line);    //TODO: Remove this check

    pushConsumes.ok();
    return Result::success(new Expression{ Expression::Type::LITERAL, { .literal = token.u.literal }, returnType });
  }

  Result::inst<Expression> SyntaxChecker::_processExpression() {
    PushConsumes pushConsumes(this);
    if (!hasPeek())
      Result::error<Expression>("Expected expression");

    int errLine = peekValue().line;
    Result::inst<Expression> ret;

    #define expression(funcName) ret = funcName(); \
                                 if (Result::isError(ret)) { \
                                   if (!Result::hasLine(ret)) \
                                     return Result::error<Expression>(ret.error, errLine); \
                                   else \
                                     return ret; \
                                 } \
                                 if (Result::hasValue(ret)) { \
                                   pushConsumes.ok(); \
                                   return ret; \
                                 }

    expression(_processExpressionLiteral);
    expression(_processExpressionAssign);

    #undef expression

    return Result::error<Expression>("Invalid expression", errLine);
  }

  
  
  int SyntaxChecker::getLine() {
    return hasPeek() ? peekValue().line : peekValue(-1).line;
  }

  void SyntaxChecker::syntaxError(std::string msg, int line) {
    if (line == -1)
      line = getLine();
    Parser::syntaxError(msg, line);
  }

  bool SyntaxChecker::trueConsume(bool check, int times) {
    if (check) {
      consume();
      for (int i = 1; i < times; i++)
        consume();
    }
    return check;
  }

  bool SyntaxChecker::falseConsume(bool check, int times) {
    return !trueConsume(!check, times);
  }

  void SyntaxChecker::pushConsumes() {
    pushindex.push(index);
  }

  void SyntaxChecker::popConsumes(bool ok) {
    int resetIndex = pushindex.pop();
    if (!ok)
      index = resetIndex;
  }

  int SyntaxChecker::pushParents(std::vector<Token>* savedTokens, std::vector<Token>* parenTokens) {
    int savedIndex = getIndex();
    *savedTokens = tokens;
    resetIndex();
    tokens = *parenTokens;
    setMaxIndex(tokens.size());
    return savedIndex;
  }

  void SyntaxChecker::popParents(std::vector<Token>* savedTokens, int savedIndex) {
    index = savedIndex;
    tokens = *savedTokens;
    setMaxIndex(tokens.size());
  }

  ReturnType* SyntaxChecker::literalToReturnType(Literal literal) {
    if (literal.type == LiteralType::INTEGER)
      return new ReturnType{ false, new Type{ Type::TypeT::INT, { .intSize = IntSize::INT32 } } };
    return NULL;
  }


  void SyntaxChecker::process() {
    while (hasPeek()) {
      GlobalNode globalNode;

      if (tryConsume({ TokenType::SEMI }))
        continue;

      else if (peekEqual({ TokenType::FUNC })) {
        Result::inst<Function> func = processFunction();
        if (Result::isError(func)) {
          if (Result::hasLine(func))
            syntaxError(func.error, func.line);
          else
            syntaxError(func.error);
        }
        
        globalNode.type = GlobalNode::Type::FUNC;
        globalNode.u.func = func.value;

      } else {
        Result::inst<Variable> var = processVariable();
        if (Result::isError(var))
          syntaxError(var.error);
        if (!Result::hasValue(var)) {
          if (var.ignoredStep == 1)
            syntaxError("Invalid type for variable");
          else // if (var.customData == 2)
            syntaxError("Expected ':' after type");
        }

        globalNode.type = GlobalNode::Type::VAR;
        globalNode.u.var = var.value;
      }

      addToOutput(globalNode);
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
