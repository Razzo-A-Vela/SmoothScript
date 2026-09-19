#include "SyntaxChecker.hpp"

namespace Parser {
  SyntaxChecker::Context SyntaxChecker::Context::fromTokens(std::vector<Token>* tokens)  {
    return { tokens, 0 };
  }

  

  SyntaxChecker::ContextSwitcher::~ContextSwitcher() {
    switchContextToPrevious();
  }
  
  void SyntaxChecker::ContextSwitcher::switchContextToPrevious() {
    if (isDone)
      return;
    
    checker->switchContextRaw(previous);
    isDone = true;
  }



  SyntaxChecker::Context SyntaxChecker::switchContextRaw(Context newContext) {
    Context previous = { tokens, index };
    tokens = newContext.tokens;
    setMaxIndex(tokens->size());
    index = newContext.index;
    return previous;
  }

  SyntaxChecker::ContextSwitcher SyntaxChecker::switchContext(Context newContext) {
    return ContextSwitcher(this, switchContextRaw(newContext));
  }

  SyntaxChecker::ContextSwitcher SyntaxChecker::switchContextTo(TokenType type, Utils::Error err) {
    if (peekNotEqual({ type }))
      Utils::error(err);
    return switchContext(Context::fromTokens(consume().value().u.tokens));
  }

  SyntaxChecker::ContextSwitcher SyntaxChecker::switchContextToParents() {
    return switchContextTo(TokenType::PARENTS, expectedOpenParentError());
  }

  SyntaxChecker::ContextSwitcher SyntaxChecker::switchContextToBrackets() {
    return switchContextTo(TokenType::BRACKETS, expectedOpenBracketError());
  }

  int SyntaxChecker::getErrorLine() {
    if (hasPeek())
      return peekValue().line;
    else if (hasPeek(-1))
      return peekValue(-1).line;
    else
      return -2; // Technically it SHOULD be unreachable
  }


  Utils::Error SyntaxChecker::syntaxError(const char* msg) {
    return Parser::syntaxError(msg, getErrorLine());
  }

  Utils::Error SyntaxChecker::expectedError(const char* expected) {
    std::stringstream stream;
    stream << "Expected " << expected;
    std::string* str = new std::string(stream.str());
    return syntaxError(str->c_str());
  }

  Utils::Error SyntaxChecker::expectedOpenParentError() {
    return expectedError("'('");
  }

  Utils::Error SyntaxChecker::expectedClosedParentError() {
    return expectedError("')'");
  }
  
  Utils::Error SyntaxChecker::expectedOpenBracketError() {
    return expectedError("'{'");
  }

  Utils::Error SyntaxChecker::expectedClosedBracketError() {
    return expectedError("'}'");
  }

  Utils::Error SyntaxChecker::expectedSemiError() {
    return expectedError("';'");
  }

  Utils::Error SyntaxChecker::expectedColonError() {
    return expectedError("':'");
  }

  Utils::Error SyntaxChecker::expectedCommaError() {
    return expectedError("','");
  }

  Utils::Error SyntaxChecker::expectedExpressionError() {
    return expectedError("expression");
  }



  #define expect(errType, type, result, function) do {       \
    Result::inst<type> result##_result = function;           \
    if (result##_result.isError())                           \
      return Result::error<errType>(result##_result.error);  \
    result = result##_result.value;                          \
  } while (0)

  #define expectWithMessage(errType, type, result, function, errMessage) do {  \
    Result::inst<type> result##_result = function;                             \
    if (result##_result.isError())                                             \
      return Result::error<errType>(errMessage);                               \
    result = result##_result.value;                                            \
  } while (0)

  #define expectParentEnd(errType, switcher) do {                  \
    if (hasPeek())                                                 \
      return Result::error<errType>(expectedClosedParentError());  \
    switcher.switchContextToPrevious();                            \
  } while (0)

  #define expectSemi(errType) do {                         \
    if (!tryConsume({ TokenType::SEMI }))                  \
      return Result::error<errType>(expectedSemiError());  \
  } while (0)


  bool SyntaxChecker::variables_peek(int offset) {
    return peekEqual({ TokenType::COLON }, offset);
  }

  Result::inst<Variables> SyntaxChecker::variables_process() {
    if (!variables_peek())
      return Result::error<Variables>(expectedColonError());
    consume();  // ':'

    Type* type;
    InitIdentifier* init;
    expect(Variables, Type, type, type_process());
    expect(Variables, InitIdentifier, init, initIdentifier_process());

    std::vector<InitIdentifier*>* other = NULL;
    if (tryConsume({ TokenType::COMMA })) {
      other = new std::vector<InitIdentifier*>();
      InitIdentifier* other_init;
      
      do {
        expect(Variables, InitIdentifier, other_init, initIdentifier_process());
        other->push_back(other_init);
      } while(tryConsume({ TokenType::COMMA }));
    }

    return Result::success(
      new Variables {
        .type = type, 
        .init = init,
        .other = other,
      }
    );
  }

  
  bool SyntaxChecker::autoVariable_peek(int offset) {
    if (peekEqual({ TokenType::MUT }, offset) || peekEqual({ TokenType::CONST }, offset))
      offset += 1;
    return rawIdentifier_peek(offset) && peekEqual({ TokenType::COLON_EQUAL }, offset + 1);
  }

  Result::inst<AutoVariable> SyntaxChecker::autoVariable_process() {
    if (!autoVariable_peek())
      return Result::error<AutoVariable>(expectedError("auto variable"));  //TODO: Better error (Maybe inside peek?)
    
    bool isMutable = tryConsume({ TokenType::MUT });
    bool isConst = tryConsume({ TokenType::CONST });
    Identifier* name;
    expect(AutoVariable, Identifier, name, rawIdentifier_process());
    consume();  // ':='

    Expression* expr;
    expect(AutoVariable, Expression, expr, expression_process(expectedExpressionError()));

    return Result::success(
      new AutoVariable {
        .isMutable = isMutable,
        .isConst = isConst,
        .name = name,
        .expr = expr
      }
    );
  }


  bool SyntaxChecker::function_peek(int offset) {
    return peekEqual({ TokenType::FUNC }, offset);
  }

  Result::inst<Function> SyntaxChecker::function_process() {
    if (!function_peek())
      return Result::error<Function>(expectedError("'func'"));
    consume();  // 'func'

    Identifier* name;
    expectWithMessage(Function, Identifier, name, rawIdentifier_process(), expectedError("function name"));

    std::vector<Variables*>* params = NULL;
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::error<Function>(expectedOpenParentError());
    ContextSwitcher switcher = switchContextToParents();

    if (hasPeek()) {
      params = new std::vector<Variables*>();
      Variables* param;

      do {
        expect(Function, Variables, param, variables_process());

        params->push_back(param);
      } while (hasPeek());
    }
    switcher.switchContextToPrevious();
    
    ReturnType* returnType;
    expect(Function, ReturnType, returnType, returnType_process());

    if (tryConsume({ TokenType::SEMI }))
      return Result::success(Function::declaration(name, returnType, params));

    Scope* scope;
    expect(Function, Scope, scope, scope_process());

    return Result::success(Function::definition(name, returnType, params, scope));
  }

  bool SyntaxChecker::scope_peek(int offset) {
    return peekEqual({ TokenType::BRACKETS }, offset);
  }

  Result::inst<Scope> SyntaxChecker::scope_process() {
    if (!scope_peek())
      return Result::error<Scope>(expectedError("scope"));
    ContextSwitcher switcher = switchContextToBrackets();

    std::vector<Statement*>* statements = new std::vector<Statement*>();
    Statement* statement;
    
    scopeDepth++;
    while (hasPeek()) {
      expect(Scope, Statement, statement, statement_process());
      statements->push_back(statement);
    }

    return Result::success(new Scope{ statements, scopeDepth-- });
  }
  

  Result::inst<Statement> SyntaxChecker::statement_process() {
    Expression* expr;
    Identifier* name;
    StatementAndExpr* statementAndExpr;
    Statement* statement;

    if (tryConsume({ TokenType::RETURN })) {
      expr = NULL;
      if (!tryConsume({ TokenType::SEMI })) {
        expect(Statement, Expression, expr, expression_process(expectedExpressionError()));
        expectSemi(Statement);
      }
 
      return Result::success(Statement::return_(expr));
    }

    if (tryConsume({ TokenType::IF })) {
      expect(Statement, StatementAndExpr, statementAndExpr, exprAndStatement_process());
      return Result::success(Statement::withStatementAndExpr(Statement::Type::IF, statementAndExpr));
    }

    if (tryConsume({ TokenType::ELSE })) {
      expect(Statement, Statement, statement, statement_process());
      return Result::success(Statement::withStatement(Statement::Type::ELSE, statement));
    }

    if (tryConsume({ TokenType::WHILE })) {
      expect(Statement, StatementAndExpr, statementAndExpr, exprAndStatement_process());
      return Result::success(Statement::withStatementAndExpr(Statement::Type::WHILE, statementAndExpr));
    }

    if (doWhile_peek()) {
      DoWhile* doWhile;
      expect(Statement, DoWhile, doWhile, doWhile_process());
      return Result::success(Statement::doWhile(doWhile));
    }

    if (tryConsume({ TokenType::LOOP })) {
      expect(Statement, Statement, statement, statement_process());
      return Result::success(Statement::withStatement(Statement::Type::LOOP, statement));
    }

    if (tryConsume({ TokenType::BREAK })) {
      expectSemi(Statement);
      return Result::success(Statement::simple(Statement::Type::BREAK));
    }

    if (tryConsume({ TokenType::CONTINUE })) {
      expectSemi(Statement);
      return Result::success(Statement::simple(Statement::Type::CONTINUE));
    }

    if (for_peek()) {
      For* for_;
      expect(Statement, For, for_, for_process());
      return Result::success(Statement::for_(for_));
    }

    if (tryConsume({ TokenType::DOUBLE_COLON })) {
      expectWithMessage(Statement, Identifier, name, rawIdentifier_process(), expectedError("label name"));
      expectSemi(Statement);
      return Result::success(Statement::withName(Statement::Type::LABEL, name));
    }

    if (tryConsume({ TokenType::GOTO })) {
      expectWithMessage(Statement, Identifier, name, identifier_process(), expectedError("label name"));
      expectSemi(Statement);
      return Result::success(Statement::withName(Statement::Type::GOTO, name));
    }

    if (using_peek()) {
      Using* using_;
      expect(Statement, Using, using_, using_process());
      expectSemi(Statement);
      return Result::success(Statement::using_(using_));
    }

    if (scope_peek()) {
      Scope* scope;
      expect(Statement, Scope, scope, scope_process());
      return Result::success(Statement::scope(scope));
    }

    return forCompatibleStatement_process();
  }

  Result::inst<Statement> SyntaxChecker::forCompatibleStatement_process() {
    if (tryConsume({ TokenType::SEMI }))
      return Result::success(Statement::simple(Statement::Type::NOTHING));

    if (variables_peek()) {
      Variables* vars;
      expect(Statement, Variables, vars, variables_process());
      expectSemi(Statement);
      return Result::success(Statement::varDecl(vars));
    }

    if (autoVariable_peek()) {
      AutoVariable* autoVar;
      expect(Statement, AutoVariable, autoVar, autoVariable_process());
      expectSemi(Statement);
      return Result::success(Statement::autoVar(autoVar));
    }

    Expression* expr;
    expect(Statement, Expression, expr, expression_process(expectedError("statement")));
    expectSemi(Statement);
    return Result::success(Statement::withExpr(Statement::Type::EXPRESSION, expr));
  }

  Result::inst<StatementAndExpr> SyntaxChecker::exprAndStatement_process() {
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::error<StatementAndExpr>(expectedOpenParentError());
    ContextSwitcher switcher = switchContextToParents();
    Expression* expr;
    Statement* statement;

    expect(StatementAndExpr, Expression, expr, expression_process(expectedExpressionError()));
    expectParentEnd(StatementAndExpr, switcher);
    expect(StatementAndExpr, Statement, statement, statement_process());
    
    return Result::success(
      new StatementAndExpr {
        .statement = statement,
        .expr = expr,
      }
    );
  }
  
  bool SyntaxChecker::doWhile_peek(int offset) {
    return peekEqual({ TokenType::DO }, offset);
  }

  Result::inst<DoWhile> SyntaxChecker::doWhile_process() {
    if (!doWhile_peek())
      return Result::error<DoWhile>(expectedError("'do'"));
    consume();  // 'do'

    Statement* doStatement;
    StatementAndExpr* whileStatementAndExpr;

    expect(DoWhile, Statement, doStatement, statement_process());
    if (!tryConsume({ TokenType::WHILE }))
      return Result::error<DoWhile>(expectedError("'while'"));
    expect(DoWhile, StatementAndExpr, whileStatementAndExpr, exprAndStatement_process());
    
    return Result::success(
      new DoWhile{
        .doStatement = doStatement,
        .whileStatementAndExpr = whileStatementAndExpr,
      }
    );
  }

  bool SyntaxChecker::for_peek(int offset) {
    return peekEqual({ TokenType::FOR }, offset);
  }

  Result::inst<For> SyntaxChecker::for_process() {
    if (!for_peek())
      return Result::error<For>(expectedError("'for'"));
    consume();  // 'for'

    Statement* initStatement;
    Expression* checkExpression = NULL;
    Expression* repeatExpression = NULL;
    Statement* statement;
    
    if (peekNotEqual({ TokenType::PARENTS }))
      return Result::error<For>(expectedOpenParentError());
    ContextSwitcher switcher = switchContextToParents();

    expect(For, Statement, initStatement, forCompatibleStatement_process());
    if (!tryConsume({ TokenType::SEMI })) {
      expect(For, Expression, checkExpression, expression_process(expectedExpressionError()));
      expectSemi(For);
    }

    if (hasPeek())
      expect(For, Expression, repeatExpression, expression_process(expectedExpressionError()));
    expectParentEnd(For, switcher);
    expect(For, Statement, statement, statement_process());
    
    return Result::success(
      new For {
        .initStatement = initStatement,
        .checkExpression = checkExpression,
        .repeatExpression = repeatExpression,
        .statement = statement
      }
    );
  }

  
  bool SyntaxChecker::using_peek(int offset) {
    return peekEqual({ TokenType::USING }, offset);
  }
  
  Result::inst<Using> SyntaxChecker::using_process() {
    if (!using_peek())
      return Result::error<Using>(expectedError("'using'"));
    consume();  // 'using'

    TypeDef* typeDef;  //TODO: Other using types
    expect(Using, TypeDef, typeDef, typeDef_process());
    return Result::success(Using::typeDef(typeDef));
  }

  bool SyntaxChecker::typeDef_peek(int offset) {
    return peekEqual({ TokenType::COLON }, offset);
  }

  Result::inst<TypeDef> SyntaxChecker::typeDef_process() {
    if (!typeDef_peek())
      return Result::error<TypeDef>(expectedColonError());
    consume();  // ':'

    Type* from;
    Type* to;
    std::vector<Type*>* other = NULL;

    expect(TypeDef, Type, from, type_process());

    bool isInt = from->type == Type::TypeT::INT;
    bool isSpecialCase = (isInt || from->type == Type::TypeT::FLOAT) && tryConsume({ TokenType::LESS });
    if (isSpecialCase) {
      Expression* expr;
      Literal literal;
      int bitAmount;

      expectWithMessage(TypeDef, Expression, expr, baseExpression_process(expectedExpressionError()), expectedError("integer literal"));
      if (expr->type != Expression::Type::LITERAL || (literal = expr->u.literal).type != LiteralType::INTEGER)
        return Result::error<TypeDef>(expectedError("integer literal"));
      bitAmount = literal.u.integer;

      if (bitAmount <= 0)
        return Result::error<TypeDef>(syntaxError("Bit amount must be greater than 0"));
      if (!tryConsume({ TokenType::GREATER }))
        return Result::error<TypeDef>(expectedError("'>'"));
      
      from = Type::specialCase(isInt, bitAmount, from->isUnsigned, from->isSigned);
    }
    
    expectWithMessage(TypeDef, Type, to, baseType_process(), expectedError("type name"));

    if (tryConsume({ TokenType::COMMA })) {
      other = new std::vector<Type*>();
      Type* type;
      
      do {
        expectWithMessage(TypeDef, Type, type, baseType_process(), expectedError("type name"));
        other->push_back(type);
      } while (tryConsume({ TokenType::COMMA }));
    }

    return Result::success(
      new TypeDef {
        .from = from,
        .to = to,
        .other = other,
      }
    );
  }


  Result::inst<Type> SyntaxChecker::type_process() {
    bool isUnsigned = false;
    bool isSigned = false;

    if (tryConsume({ TokenType::UNSIGNED }))
      isUnsigned = true;
    else if (tryConsume({ TokenType::SIGNED }))
      isSigned = true;

    Result::inst<Type> ret = baseType_process();
    if (ret.isError())
      return ret;

    ret.value->isSigned = isSigned;
    ret.value->isUnsigned = isUnsigned;
    return ret;
  }

  Result::inst<Type> SyntaxChecker::baseType_process() {
    #define success(type) Result::success(Type::of(type))

    if (tryConsume({ TokenType::INT }))
      return success(Type::TypeT::INT);

    else if (tryConsume({ TokenType::FLOAT }))
      return success(Type::TypeT::FLOAT);

    else if (tryConsume({ TokenType::BOOL }))
      return success(Type::TypeT::BOOL);

    else if (tryConsume({ TokenType::CSTR }))
      return success(Type::TypeT::CSTR);

    else if (tryConsume({ TokenType::CHAR }))
      return success(Type::TypeT::CHAR);

    else if (tryConsume({ TokenType::SIZE_T }))
      return success(Type::TypeT::SIZE_T);
    
    #undef success

    Identifier* identifier;
    expectWithMessage(Type, Identifier, identifier, identifier_process(), expectedError("type"));
    return Result::success(Type::custom(identifier));
  }

  Result::inst<ReturnType> SyntaxChecker::returnType_process() {
    if (tryConsume({ TokenType::VOID }))
      return Result::success(ReturnType::void_());

    else if (tryConsume({ TokenType::EXCLAMATION }))
      return Result::success(ReturnType::noReturn());
    
    Type* type;
    expectWithMessage(ReturnType, Type, type, type_process(), expectedError("return type"));
    return Result::success(ReturnType::fromType(type));
  }


  Result::inst<Identifier> SyntaxChecker::identifier_process() {
    NamespaceIdentifier* last;
    bool globalScope = false;
    Identifier* identifier;

    if (tryConsume({ TokenType::DOT }))
      globalScope = true;

    expect(Identifier, Identifier, identifier, rawIdentifier_process());
    identifier->globalScope = globalScope;
    last = &identifier->identifier;

    while (peekEqual({ TokenType::IDENTIFIER }, 1) && tryConsume({ TokenType::DOT })) {
      last->next = new NamespaceIdentifier { .name = consume().value().u.string };
      last = last->next;
    }

    return Result::success(identifier);
  }

  bool SyntaxChecker::rawIdentifier_peek(int offset) {
    return peekEqual({ TokenType::IDENTIFIER }, offset);
  }

  Result::inst<Identifier> SyntaxChecker::rawIdentifier_process() {
    if (!rawIdentifier_peek())
      return Result::error<Identifier>(expectedError("identifier"));
    
    return Result::success(Identifier::simple(consume().value().u.string));
  }

  
  Result::inst<InitIdentifier> SyntaxChecker::initIdentifier_process() {
    bool isMutable = false;
    bool isConst = false;
    if (tryConsume({ TokenType::MUT }))
      isMutable = true;
    else if (tryConsume({ TokenType::CONST }))
      isConst = true;
    
    Identifier* name;
    expectWithMessage(InitIdentifier, Identifier, name, rawIdentifier_process(), expectedError("variable name"));
    
    InitExpression* initExpr = NULL;
    if (tryConsume({ TokenType::EQUALS }))
      expect(InitIdentifier, InitExpression, initExpr, initExpression_process());
    
    return Result::success(
      new InitIdentifier{ 
        .isMutable = isMutable,
        .isConst = isConst,
        .name = name,
        .expr = initExpr,
      }
    );
  }

  Result::inst<InitExpression> SyntaxChecker::initExpression_process() {
    Expression* expr;
    expect(InitExpression, Expression, expr, expression_process(expectedExpressionError()));
    return Result::success(InitExpression::expression(expr));
  }


  Result::inst<Expression> SyntaxChecker::expression_process(Utils::Error identifierError) {
    Result::inst<Expression> ret = baseExpression_process(identifierError);
    if (ret.isError())
      return ret;

    index_t opIndex;
    Expression* left;
    Operator* op;
    Expression* right;
    while ((opIndex = operator_tryConsume_index()) != INDEX_T_NOT_FOUND) {
      left = ret.value;
      op = operatorFromIndex(opIndex);
      expect(Expression, Expression, right, baseExpression_process(identifierError));

      if (op->type == Operator::Type::WALK) {
        if (left->type != Expression::Type::BINARY_OP && left->type != Expression::Type::IDENTIFIER &&
            left->type != Expression::Type::FUNC_CALL && left->type != Expression::Type::EXPR)
          return Result::error<Expression>(syntaxError("Invalid Expression before '.'"));

        if (right->type != Expression::Type::IDENTIFIER && right->type != Expression::Type::FUNC_CALL)
          return Result::error<Expression>(syntaxError("Invalid Expression after '.'"));
      }


      #define leftBinaryOp left->u.binaryOp
      
      if (left->type == Expression::Type::BINARY_OP && op->precedence > leftBinaryOp->op->precedence)
        leftBinaryOp->right = Expression::binaryOp(leftBinaryOp->right, op, right);
      else
        ret.value = Expression::binaryOp(left, op, right);

      #undef leftBinaryOp
    }
    
    return ret;
  }

  Result::inst<Expression> SyntaxChecker::baseExpression_process(Utils::Error identifierError) {
    Identifier* name;
    Expression* expr;

    if (literalExpression_peek())
      return literalExpression_process();

    else if (peekEqual({ TokenType::PARENTS })) {
      ContextSwitcher switcher = switchContextToParents();
      
      expect(Expression, Expression, expr, expression_process(expectedExpressionError()));
      expectParentEnd(Expression, switcher);
      return Result::success(Expression::withExpr(Expression::Type::EXPR, expr));
    }
    
    else if (tryConsume({ TokenType::MINUSMINUS })) {
      expect(Expression, Identifier, name, identifier_process());
      return Result::success(Expression::withName(Expression::Type::PRE_DECREMENT, name));
    }

    else if (tryConsume({ TokenType::PLUSPLUS })) {
      expect(Expression, Identifier, name, identifier_process());
      return Result::success(Expression::withName(Expression::Type::PRE_INCREMENT, name));
    }


    //? Calling baseExpresion_process ensures that unary operators are ALWAYS before binary operators
    #define unaryOperator(tokenType, exprType) do {                                                 \
        if (tryConsume({ tokenType })) {                                                            \
          expect(Expression, Expression, expr, baseExpression_process(expectedExpressionError()));  \
          return Result::success(Expression::withExpr(exprType, expr));                             \
        }                                                                                           \
      } while(0)

    unaryOperator(TokenType::EXCLAMATION, Expression::Type::NOT);
    unaryOperator(TokenType::TILDE, Expression::Type::BIT_NOT);
    unaryOperator(TokenType::MINUS, Expression::Type::MINUS);
    unaryOperator(TokenType::PLUS, Expression::Type::PLUS);

    #undef unaryOperator
    
    
    expectWithMessage(Expression, Identifier, name, identifier_process(), identifierError);
    
    if (tryConsume({ TokenType::EQUALS })) {
      expect(Expression, Expression, expr, expression_process(expectedExpressionError()));
      return Result::success(Expression::varAssign(name, expr));
    }

    else if (peekEqual({ TokenType::PARENTS })) {
      ContextSwitcher switcher = switchContextToParents();
      std::vector<Expression*>* params = NULL;

      if (hasPeek()) {
        params = new std::vector<Expression*>();
      
        while (true) {
          expect(Expression, Expression, expr, expression_process(expectedExpressionError()));
          params->push_back(expr);

          if (!tryConsume({ TokenType::COMMA })) {
            if (!hasPeek())
              break;
            
            return Result::error<Expression>(expectedCommaError());
          }
        };
      }

      return Result::success(Expression::funcCall(name, params));
    }

    else if (tryConsume({ TokenType::PLUSPLUS }))
      return Result::success(Expression::withName(Expression::Type::INCREMENT, name));

    else if (tryConsume({ TokenType::MINUSMINUS }))
      return Result::success(Expression::withName(Expression::Type::DECREMENT, name));

    return Result::success(Expression::withName(Expression::Type::IDENTIFIER, name));
  }


  const TokenType operatorTokens[] = {
    TokenType::DOT,

    TokenType::ASTERISK, TokenType::SLASH,

    TokenType::PLUS, TokenType::MINUS,
    
    TokenType::DOUBLE_EQUALS, TokenType::NOT_EQUAL, TokenType::LESS,
    TokenType::LESS_EQ, TokenType::GREATER, TokenType::GREATER_EQ,
    
    TokenType::AND,
    
    TokenType::OR,

    TokenType::SHIFT_RIGHT, TokenType::SHIFT_LEFT,
    
    TokenType::AMPERSAND, TokenType::PIPE
  };
  const Operator operators[] = {  //? Unary operators are ALWAYS before binary operators
    { Operator::Type::WALK, 3 },

    { Operator::Type::MULT, 2 }, { Operator::Type::DIV, 2 },
    
    { Operator::Type::ADD, 1 }, { Operator::Type::SUB, 1 },
    
    { Operator::Type::EQUALS, 0 }, { Operator::Type::NOT_EQUALS, 0 }, { Operator::Type::LESS, 0 },
    { Operator::Type::LESS_EQ, 0 }, { Operator::Type::GREATER, 0 }, { Operator::Type::GREATER_EQ, 0 },
    
    { Operator::Type::AND, -1 },

    { Operator::Type::OR, -2 },
    
    { Operator::Type::SHIFT_RIGHT, -3 }, { Operator::Type::SHIFT_LEFT, -3 },
    
    { Operator::Type::BIT_AND, -4 }, { Operator::Type::BIT_OR, -4 }
  };
  const int OP_AMOUNT = sizeof(operators) / sizeof(operators[0]);

  index_t SyntaxChecker::operator_tryConsume_index() {
    for (int i = 0; i < OP_AMOUNT; i++) {
      if (tryConsume({ operatorTokens[i] }))
        return i;
    }

    return INDEX_T_NOT_FOUND;
  }

  Operator* SyntaxChecker::operatorFromIndex(index_t index) {
    return new Operator(operators[index]);
  }

  bool SyntaxChecker::literalExpression_peek(int offset) {
    #define nextLiteral() peekEqual({ TokenType::LITERAL }, offset + 1)

    return peekEqual({ TokenType::LITERAL }, offset) || (peekEqual({ TokenType::MINUS }, offset) && nextLiteral()) || (peekEqual({ TokenType::PLUS }, offset) && nextLiteral());
    
    #undef nextLiteral
  }

  Result::inst<Expression> SyntaxChecker::literalExpression_process() {
    if (!literalExpression_peek())
      return Result::error<Expression>(expectedExpressionError());

    bool isPositive = tryConsume({ TokenType::PLUS });
    bool isNegative = tryConsume({ TokenType::MINUS });
    Token token = consume().value();
    Literal literal = token.u.literal;

    #define success(type) Result::success(Expression::literal(literal, ReturnType::fromType(Type::of(type))));

    if (literal.type == LiteralType::INTEGER) {
      if (isNegative)
        literal.u.integer *= -1; 
      return success(Type::TypeT::INT_LIT);
    }
    
    else if (literal.type == LiteralType::FLOATING) {
      if (isNegative)
        literal.u.floating *= -1; 
      return success(Type::TypeT::FLOAT_LIT);
    }
    
    else if (literal.type == LiteralType::STRING) {
      if (isNegative)
        return Result::error<Expression>(syntaxError("Unexpected '-' before string literal"));
      if (isPositive)
        return Result::error<Expression>(syntaxError("Unexpected '+' before string literal"));
      return success(Type::TypeT::CSTR);
    }
    
    else if (literal.type == LiteralType::CHAR) {
      if (isNegative)
        return Result::error<Expression>(syntaxError("Unexpected '-' before char literal"));
      if (isPositive)
        return Result::error<Expression>(syntaxError("Unexpected '+' before char literal"));
      return success(Type::TypeT::CHAR);
    }

    #undef success

    //* SHOULD be unreachable
    return Result::error<Expression>(Parser::syntaxError("Invalid expression", token.line));
  }


  #undef expect
  #undef expectWithAlways
  #undef expectWithOnError
  #undef expectParentEnd
  #undef expectSemi



  void SyntaxChecker::process() {
    while (hasPeek()) {
      if (tryConsume({ TokenType::SEMI }))
        continue; // Technically not needed (better than ';')
      else if (variables_peek())
        addToOutput({ GlobalNode::Type::VAR_DECL, { .vars = expectSemiOnResult(variables_process()).expectValue() } });
      else if (function_peek())
        addToOutput({ GlobalNode::Type::FUNC, { .func = function_process().expectValue() } });
      else if (using_peek())
        addToOutput({ GlobalNode::Type::USING, { .using_ = expectSemiOnResult(using_process()).expectValue() } });
      else if (autoVariable_peek())
        addToOutput({ GlobalNode::Type::AUTO_VAR, { .autoVar = expectSemiOnResult(autoVariable_process()).expectValue() } });
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
