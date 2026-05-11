#include "SyntaxChecker.hpp"

namespace Parser {
  Context Context::fromTokens(std::vector<Token>* tokens)  {
    return { tokens, 0 };
  }


  Utils::Error SyntaxChecker::syntaxError(const char* msg) {
    return Parser::syntaxError(msg, getErrorLine());
  }

  Utils::Error SyntaxChecker::parentsError() {
    return syntaxError("Expected '('");
  }

  Context SyntaxChecker::switchContext(Context newContext) {
    Context previous = { tokens, index };
    tokens = newContext.tokens;
    setMaxIndex(tokens->size());
    index = newContext.index;
    return previous;
  }

  Context SyntaxChecker::switchContextTo(TokenType type, Utils::Error err) {
    if (peekNotEqual({ type }))
      Utils::error(err);
    return switchContext(Context::fromTokens(consume().value().u.tokens));
  }

  Context SyntaxChecker::switchContextToParents() {
    return switchContextTo(TokenType::PARENTS, parentsError());
  }

  Context SyntaxChecker::switchContextToBrackets() {
    return switchContextTo(TokenType::BRACKETS, syntaxError("Expected '{'"));
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

  Utils::Error SyntaxChecker::semiError() {
    return syntaxError("Expected ';'");
  }

  
  #define expectSemi(retType) \
    { if (!semi()) \
        return Result::error<retType>(semiError()); } 0

  #define returnIfError(retType, ret) \
    { if (ret.isError()) \
        return Result::error<retType>(ret.error); } 0

  #define returnIfErrorPtr(retType, ret) \
    { if (ret->isError()) \
        return Result::error<retType>(ret->error); } 0


  #define expectError(retType, type, result, function) \
    { Result::inst<type> result##_result = function; \
    if (!result##_result.hasValue()) \
      return Result::error<retType>(result##_result.error); \
    result = result##_result.value; } 0
  
  #define expectErrorWithAlways(retType, type, result, function, alwaysFunction) \
    { Result::inst<type> result##_result = function; \
    alwaysFunction; \
    if (!result##_result.hasValue()) \
      return Result::error<retType>(result##_result.error); \
    result = result##_result.value; } 0
  
  #define expectErrorWithOnError(retType, type, result, function, onErrorFunction) \
    { Result::inst<type> result##_result = function; \
    if (!result##_result.hasValue()) {\
      onErrorFunction; \
      return Result::error<retType>(result##_result.error); \
    } \
    result = result##_result.value; } 0
  
  #define expectIgnore(retType, type, result, function) \
    { Result::inst<type> result##_result = function; \
    if (!result##_result.hasValue()) \
      return Result::ignore<retType>(result##_result.error); \
    result = result##_result.value; } 0
  
  #define expectParentEnd(retType, previous) \
    { if (hasPeek()) { \
      switchContext(previous); \
      return Result::error<retType>(syntaxError("Expected ')'")); \
    } \
    switchContext(previous); } 0


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
    expectError(Function, Identifier, name, processRawIdentifier());

    std::vector<Variables*>* params = NULL;
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::error<Function>(parentsError());
    Context previous = switchContextToParents();
    
    if (hasPeek()) {
      params = new std::vector<Variables*>();
      Variables* param;

      do {
        if (!wakeup(TokenType::COLON)) {
          switchContext(previous);
          return Result::error<Function>(syntaxError("Expected ':'"));
        }

        expectErrorWithOnError(Function, Variables, param, processVariables(), switchContext(previous));
        params->push_back(param);
      } while (hasPeek());
    }

    switchContext(previous);

    ReturnType* returnType;
    expectError(Function, ReturnType, returnType, processReturnType());

    if (semi())
      return Result::success(new Function{ name, returnType, params, false, NULL });

    Scope* scope;
    expectError(Function, Scope, scope, processScope());

    return Result::success(new Function{ name, returnType, params, true, scope });
  }

  Result::inst<ReturnType> SyntaxChecker::processReturnType() {
    if (wakeup(TokenType::VOID))
      return Result::success(ReturnType::_void());

    if (wakeup(TokenType::EXCLAMATION))
      return Result::success(ReturnType::noReturn());
    
    
    Result::inst<Type> type = processType();
    if (type.isIgnored())
      return Result::ignore<ReturnType>(syntaxError("Expected return type"));
    returnIfError(ReturnType, type);
    
    return Result::success(ReturnType::fromType(type.value));
  }

  Result::inst<Scope> SyntaxChecker::processScope() {
    if (peekNotEqual({ TokenType::BRACKETS }))
      return Result::ignore<Scope>(syntaxError("Expected '{'"));
    Context previous = switchContextToBrackets();

    std::vector<Statement*>* statements = new std::vector<Statement*>();
    Result::inst<Statement> statement;
    
    scopeDepth++;
    while ((statement = processStatement()).hasValue())
      statements->push_back(statement.value);

    switchContext(previous);
    returnIfError(Scope, statement);
    return Result::success(new Scope{ statements, scopeDepth-- });
  }

  Result::inst<Statement> SyntaxChecker::processStatement() {
    if (wakeup(TokenType::RETURN)) {
      Expression* expr = NULL;
      if (!semi()) {
        expectError(Statement, Expression, expr, processExpression());
        expectSemi(Statement);
      }
 
      return Result::success(new Statement{ Statement::Type::RETURN, { .expr = expr } });
    }

    if (wakeup(TokenType::IF)) {
      StatementAndExpr* statementAndExpr;
      expectError(Statement, StatementAndExpr, statementAndExpr, processExprAndStatement());
      
      return Result::success(new Statement{ Statement::Type::IF, { .statementAndExpr = statementAndExpr } });
    }

    if (wakeup(TokenType::ELSE)) {
      Statement* statement;
      expectError(Statement, Statement, statement, processStatement());
      
      return Result::success(new Statement{ Statement::Type::ELSE, { .statement = statement } });
    }

    if (wakeup(TokenType::WHILE)) {
      StatementAndExpr* statementAndExpr;
      expectError(Statement, StatementAndExpr, statementAndExpr, processExprAndStatement());
      
      return Result::success(new Statement{ Statement::Type::WHILE, { .statementAndExpr = statementAndExpr } });
    }

    if (wakeup(TokenType::DO)) {
      Statement* doStatement;
      StatementAndExpr* whileStatementAndExpr;

      expectError(Statement, Statement, doStatement, processStatement());
      if (!wakeup(TokenType::WHILE))
        return Result::error<Statement>(syntaxError("Expected 'while'"));
      expectError(Statement, StatementAndExpr, whileStatementAndExpr, processExprAndStatement());
      
      return Result::success(new Statement{ Statement::Type::DO_WHILE, { .doWhile = new DoWhile{ doStatement, whileStatementAndExpr } } });
    }

    if (wakeup(TokenType::LOOP)) {
      Statement* statement;
      expectError(Statement, Statement, statement, processStatement());

      return Result::success(new Statement{ Statement::Type::LOOP, { .statement = statement } });
    }

    if (wakeup(TokenType::BREAK)) {
      expectSemi(Statement);
      return Result::success(new Statement{ Statement::Type::BREAK });
    }

    if (wakeup(TokenType::CONTINUE)) {
      expectSemi(Statement);
      return Result::success(new Statement{ Statement::Type::CONTINUE });
    }

    if (wakeup(TokenType::FOR)) {
      Statement* initStatement;
      Expression* checkExpression = NULL;
      Expression* repeatExpression = NULL;
      Statement* statement;
      
      if (peekNotEqual({ TokenType::PARENTS }))
        return Result::error<Statement>(parentsError());
      Context previous = switchContextToParents();

      expectErrorWithOnError(Statement, Statement, initStatement, processForCompatibleStatement(), switchContext(previous));
      if (!semi()) {
        expectErrorWithOnError(Statement, Expression, checkExpression, processExpression(), switchContext(previous));
        expectSemi(Statement);
      }

      if (hasPeek()) {
        expectErrorWithOnError(Statement, Expression, repeatExpression, processExpression(), switchContext(previous));
        expectParentEnd(Statement, previous);
      } else
        switchContext(previous);
      
      expectError(Statement, Statement, statement, processStatement());
      return Result::success(new Statement{ Statement::Type::FOR, { .for_ = new For{ initStatement, checkExpression, repeatExpression, statement } } });
    }

    Result::inst<Scope> scope;
    if ((scope = processScope()).hasValue())
      return Result::success(new Statement{ Statement::Type::SCOPE, { .scope = scope.value } });
    else
      returnIfError(Statement, scope);
    
    Result::inst<Statement> forCompatible;
    if ((forCompatible = processForCompatibleStatement()).hasValue())
      return forCompatible;
    else
      returnIfError(Statement, forCompatible);

    return Result::ignore<Statement>(syntaxError("Expected statement"));
  }

  Result::inst<Statement> SyntaxChecker::processForCompatibleStatement() {
    if (semi())
      return Result::success(new Statement{ Statement::Type::NOTHING });

    if (wakeup(TokenType::COLON)) {
      Variables* vars;
      expectError(Statement, Variables, vars, processVariables());
      expectSemi(Statement);
      return Result::success(new Statement{ Statement::Type::VAR_DECL, { .vars = vars } });
    }

    Result::inst<Expression> expr;
    if ((expr = processExpression()).hasValue()) {
      expectSemi(Statement);
      return Result::success(new Statement{ Statement::Type::EXPRESSION, { .expr = expr.value } });
    } else
      returnIfError(Statement, expr);
    
    return Result::ignore<Statement>(syntaxError("Expected statement"));
  }

  Result::inst<Identifier> SyntaxChecker::processRawIdentifier() {
    if (peekEqual({ TokenType::IDENTIFIER }))
      return Result::success(new Identifier{ consume().value().u.string });
    return Result::ignore<Identifier>(syntaxError("Expected identifier"));
  }

  Result::inst<Identifier> SyntaxChecker::processIdentifier() {
    return processRawIdentifier();
  }

  Result::inst<Type> SyntaxChecker::processType() {
    bool isUnsigned = false;
    bool isConst = false;

    if (wakeup(TokenType::CONST))
      isConst = true;

    if (wakeup(TokenType::UNSIGNED))
      isUnsigned = true;
    else if (wakeup(TokenType::SIGNED));

    #define success(type) Result::success(new Type{ type, isConst, isUnsigned })

    if (wakeup(TokenType::INT))
      return success(Type::TypeT::INT);

    if (wakeup(TokenType::FLOAT))
      return success(Type::TypeT::FLOAT);

    if (wakeup(TokenType::BOOL))
      return success(Type::TypeT::BOOL);

    if (wakeup(TokenType::CSTR))
      return success(Type::TypeT::CSTR);

    if (wakeup(TokenType::CHAR))
      return success(Type::TypeT::CHAR);

    if (wakeup(TokenType::SIZE_T))
      return success(Type::TypeT::SIZE_T);
    
    #undef success
    
    return Result::ignore<Type>(syntaxError("Expected type"));
  }

  Result::inst<InitExpression> SyntaxChecker::processInitExpression() {
    Expression* expr;
    expectError(InitExpression, Expression, expr, processExpression());
    return Result::success(new InitExpression{ InitExpression::Type::EXPRESSION, { .expr = expr } });
  }

  Result::inst<Expression> SyntaxChecker::processExpression() {
    Result::inst<Expression> ret = processBaseExpression();
    if (!ret.hasValue())
      return ret;

    Result::inst<Operator> opResult;
    Expression* left;
    Operator* op;
    Expression* right;
    while ((opResult = processOperator()).hasValue()) {
      left = ret.value;
      op = opResult.value;
      expectError(Expression, Expression, right, processBaseExpression());

      ret = Result::success(new Expression{ Expression::Type::BINARY_OP, { .binaryOp = new BinaryOp{ left, op, right } }, ReturnType::unknown() });
    }
    
    returnIfError(Expression, opResult);
    return ret;
  }

  Result::inst<Expression> SyntaxChecker::processBaseExpression() {
    Result::inst<Identifier> identifier;

    if (peekEqual({ TokenType::LITERAL }))
      return processLiteralExpression();
    
    if (peekEqual({ TokenType::PARENTS })) {
      Context previous = switchContextToParents();
      
      Expression* expr;
      expectErrorWithAlways(Expression, Expression, expr, processExpression(), expectParentEnd(Expression, previous));
      return Result::success(new Expression{ Expression::Type::EXPR, { .expr = expr }, expr->returnType });
    }

    if (wakeup({ TokenType::EXCLAMATION })) {
      Expression* expr;
      expectError(Expression, Expression, expr, processExpression());
      return Result::success(new Expression{ Expression::Type::NOT, { .expr = expr }, ReturnType::unknown() });
    }

    if (wakeup({ TokenType::TILDE })) {
      Expression* expr;
      expectError(Expression, Expression, expr, processExpression());
      return Result::success(new Expression{ Expression::Type::BIT_NOT, { .expr = expr }, ReturnType::unknown() });
    }
    
    if ((identifier = processIdentifier()).hasValue()) {
      Identifier* name = identifier.value;

      if (wakeup(TokenType::EQUALS)) {
        Expression* expr;
        expectError(Expression, Expression, expr, processExpression());
        return Result::success(new Expression{ Expression::Type::VAR_ASSIGN, { .varAssign = new VarAssign{ name, expr } }, expr->returnType });
      }

      if (peekEqual({ TokenType::PARENTS })) {
        Context previous = switchContextToParents();
        std::vector<Expression*>* params = NULL;

        if (hasPeek()) {
          params = new std::vector<Expression*>();
          Expression* expr;
        
          while (true) {
            expectErrorWithOnError(Expression, Expression, expr, processExpression(), switchContext(previous));
            params->push_back(expr);

            if (!wakeup(TokenType::COMMA)) {
              if (!hasPeek())
                break;
              
              switchContext(previous);
              return Result::error<Expression>(syntaxError("Expected ','"));
            }
          };
        }

        switchContext(previous);
        return Result::success(new Expression{ Expression::Type::FUNC_CALL, { .funcCall = new FuncCall{ name, params } }, ReturnType::unknown() });
      }

      if (wakeup(TokenType::PLUSPLUS))
        return Result::success(new Expression{ Expression::Type::INCREMENT, { .name = name }, ReturnType::unknown() });

      if (wakeup(TokenType::MINUSMINUS))
        return Result::success(new Expression{ Expression::Type::DECREMENT, { .name = name }, ReturnType::unknown() });

      return Result::success(new Expression{ Expression::Type::VAR, { .name = name }, ReturnType::unknown() });
    } else
      returnIfError(Expression, identifier);
    
    return Result::ignore<Expression>(syntaxError("Expected expression"));
  }

  Result::inst<Expression> SyntaxChecker::processLiteralExpression() {
    Token token = consume().value();
    Literal literal = token.u.literal;

    if (literal.type != LiteralType::INTEGER)
      return Result::error<Expression>(Parser::syntaxError("Invalid expression", token.line));
    return Result::success(new Expression{ Expression::Type::LITERAL, { .literal = literal }, ReturnType::fromType(new Type{ Type::TypeT::INT }) });
  }

  Result::inst<Operator> SyntaxChecker::processOperator() {
    const TokenType tokens[] = { TokenType::PLUS, TokenType::MINUS, TokenType::ASTERISK, TokenType::SLASH,
                                  TokenType::LESS, TokenType::LESS_EQ, TokenType::SHIFT_LEFT,
                                  TokenType::GREATER, TokenType::GREATER_EQ, TokenType::SHIFT_RIGHT,
                                  TokenType::AND, TokenType::AMPERSAND, TokenType::OR, TokenType::PIPE,
                                  TokenType::DOUBLE_EQUALS, TokenType::NOT_EQUAL };
    const Operator operators[] = { { Operator::Type::ADD, 1 }, { Operator::Type::SUB, 1 }, { Operator::Type::MULT, 2 }, { Operator::Type::DIV, 2 },
                                    { Operator::Type::LESS, 0 }, { Operator::Type::LESS_EQ, 0 }, { Operator::Type::SHIFT_LEFT, -2 },
                                    { Operator::Type::GREATER, 0 }, { Operator::Type::GREATER_EQ, 0 }, { Operator::Type::SHIFT_RIGHT, -2 }, 
                                    { Operator::Type::AND, -1 }, { Operator::Type::BIT_AND, -3 }, { Operator::Type::OR, -1 }, { Operator::Type::BIT_OR, -3 },
                                    { Operator::Type::EQUALS, 0 }, { Operator::Type::NOT_EQUALS, 0 } };
    const int OP_AMOUNT = sizeof(tokens) / sizeof(tokens[0]);

    for (int i = 0; i < OP_AMOUNT; i++) {
      if (wakeup(tokens[i]))
        return Result::success(new Operator(operators[i]));
    }
    return Result::ignore<Operator>(syntaxError("Expected operator"));
  }

  Result::inst<InitIdentifier> SyntaxChecker::processInitIdentifier() {
    Identifier* name;
    expectError(InitIdentifier, Identifier, name, processRawIdentifier());
    
    InitExpression* initExpr = NULL;
    if (wakeup(TokenType::EQUALS))
      expectError(InitIdentifier, InitExpression, initExpr, processInitExpression());
    
    return Result::success(new InitIdentifier{ name, initExpr });
  }

  Result::inst<StatementAndExpr> SyntaxChecker::processExprAndStatement() {
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::ignore<StatementAndExpr>(parentsError());
    Context previous = switchContextToParents();
    Expression* expr;
    Statement* statement;

    expectErrorWithAlways(StatementAndExpr, Expression, expr, processExpression(), expectParentEnd(StatementAndExpr, previous));
    expectError(StatementAndExpr, Statement, statement, processStatement());
    return Result::success(new StatementAndExpr{ statement, expr });
  }


  void SyntaxChecker::process() {
    while (hasPeek()) {
      Token token = consume().value();
      
      if (semi(token))
        continue; //* Technically not needed (better than ';')
      else if (wakeup(token, TokenType::COLON))
        addToOutput({ GlobalNode::Type::VAR_DECL, { .vars = expectSemiOnResult(processVariables()).expectValue() } });
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
