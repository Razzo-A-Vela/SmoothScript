#include "SyntaxChecker.hpp"

namespace Parser {
  Context Context::fromTokens(std::vector<Token>* tokens)  {
    return { tokens, 0 };
  }


  Utils::Error SyntaxChecker::syntaxError(const char* msg) {
    return Parser::syntaxError(msg, getErrorLine());
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
      return -2; // Technically it SHOULD be unreachable
  }

  bool SyntaxChecker::wakeup(TokenType tokenType, bool consume) {
    return (consume && tryConsume({ tokenType })) || (!consume && peekEqual({ tokenType }));
  }

  Utils::Error SyntaxChecker::semiError() {
    return syntaxError("Expected ';'");
  }
  
  Utils::Error SyntaxChecker::parentsError() {
    return syntaxError("Expected '('");
  }



  #define expect(errType, type, result, function) do {       \
    Result::inst<type> result##_result = function;           \
    if (result##_result.isError())                           \
      return Result::error<errType>(result##_result.error);  \
    result = result##_result.value;                          \
  } while (0)

  #define expectWithAlways(errType, type, result, function, alwaysFunction) do {  \
    Result::inst<type> result##_result = function;                                \
    alwaysFunction;                                                               \
    if (result##_result.isError())                                                \
      return Result::error<errType>(result##_result.error);                       \
    result = result##_result.value;                                               \
  } while(0);

  #define expectWithOnError(errType, type, result, function, errorFunction) do {  \
    Result::inst<type> result##_result = function;                                \
    if (result##_result.isError()) {                                              \
      errorFunction;                                                              \
      return Result::error<errType>(result##_result.error);                       \
    }                                                                             \
    result = result##_result.value;                                               \
  } while (0)

  #define expectParentEnd(errType, previous) do {                 \
    if (hasPeek()) {                                              \
      switchContext(previous);                                    \
      return Result::error<errType>(syntaxError("Expected ')"));  \
    }                                                             \
    switchContext(previous);                                      \
  } while (0)

  #define expectSemi(errType) do {                 \
    if (!wakeup(TokenType::SEMI, true))            \
      return Result::error<errType>(semiError());  \
  } while (0)


  bool SyntaxChecker::variables_wakeup() {
    return wakeup({ TokenType::COLON }, true);
  }

  Result::inst<Variables> SyntaxChecker::variables_process() {
    Type* type;
    InitIdentifier* init;
    expect(Variables, Type, type, type_process());
    expect(Variables, InitIdentifier, init, initIdentifier_process());

    std::vector<InitIdentifier*>* other = NULL;
    if (wakeup(TokenType::COMMA, true)) {
      other = new std::vector<InitIdentifier*>();
      InitIdentifier* other_init;
      
      do {
        expect(Variables, InitIdentifier, other_init, initIdentifier_process());
        other->push_back(other_init);
      } while(wakeup(TokenType::COMMA, true));
    }

    return Result::success(
      new Variables {
        .type = type, 
        .init = init,
        .other = other,
      }
    );
  }


  bool SyntaxChecker::function_wakeup() {
    return wakeup(TokenType::FUNC, true);
  }

  Result::inst<Function> SyntaxChecker::function_process() {
    Identifier* name;
    expect(Function, Identifier, name, rawIdentifier_process());

    std::vector<Variables*>* params = NULL;
    if (!wakeup(TokenType::PARENTS, false))
      return Result::error<Function>(parentsError());
    Context previous = switchContextToParents();
    
    if (hasPeek()) {
      params = new std::vector<Variables*>();
      Variables* param;

      do {
        if (!variables_wakeup()) {
          switchContext(previous);
          return Result::error<Function>(syntaxError("Expected ':'"));
        }

        expectWithOnError(Function, Variables, param, variables_process(), switchContext(previous));
        params->push_back(param);
      } while (hasPeek());
    }

    switchContext(previous);

    ReturnType* returnType;
    expect(Function, ReturnType, returnType, returnType_process());

    if (wakeup(TokenType::SEMI, true))
      return Result::success(Function::declaration(name, returnType, params));

    Scope* scope;
    if (!scope_wakeup())
      return Result::error<Function>(syntaxError("Expected '{'"));
    expect(Function, Scope, scope, scope_process());

    return Result::success(Function::definition(name, returnType, params, scope));
  }

  bool SyntaxChecker::scope_wakeup() {
    return wakeup(TokenType::BRACKETS, false);
  }

  Result::inst<Scope> SyntaxChecker::scope_process() {
    Context previous = switchContextToBrackets();

    std::vector<Statement*>* statements = new std::vector<Statement*>();
    Statement* statement;
    
    scopeDepth++;
    while (hasPeek()) {
      expectWithOnError(Scope, Statement, statement, statement_process(), switchContext(previous));
      statements->push_back(statement);
    }

    switchContext(previous);
    return Result::success(new Scope{ statements, scopeDepth-- });
  }
  

  Result::inst<Statement> SyntaxChecker::statement_process() {
    Expression* expr;
    Identifier* name;
    StatementAndExpr* statementAndExpr;
    Statement* statement;

    if (wakeup(TokenType::RETURN, true)) {
      expr = NULL;
      if (!wakeup(TokenType::SEMI, true)) {
        expect(Statement, Expression, expr, expression_process());
        expectSemi(Statement);
      }
 
      return Result::success(Statement::return_(expr));
    }

    if (wakeup(TokenType::IF, true)) {
      expect(Statement, StatementAndExpr, statementAndExpr, exprAndStatement_process());
      return Result::success(Statement::withStatementAndExpr(Statement::Type::IF, statementAndExpr));
    }

    if (wakeup(TokenType::ELSE, true)) {
      expect(Statement, Statement, statement, statement_process());
      return Result::success(Statement::withStatement(Statement::Type::ELSE, statement));
    }

    if (wakeup(TokenType::WHILE, true)) {
      expect(Statement, StatementAndExpr, statementAndExpr, exprAndStatement_process());
      return Result::success(Statement::withStatementAndExpr(Statement::Type::WHILE, statementAndExpr));
    }

    if (wakeup(TokenType::DO, true)) {
      DoWhile* doWhile;
      expect(Statement, DoWhile, doWhile, doWhile_process());
      return Result::success(Statement::doWhile(doWhile));
    }

    if (wakeup(TokenType::LOOP, true)) {
      expect(Statement, Statement, statement, statement_process());
      return Result::success(Statement::withStatement(Statement::Type::LOOP, statement));
    }

    if (wakeup(TokenType::BREAK, true)) {
      expectSemi(Statement);
      return Result::success(Statement::simple(Statement::Type::BREAK));
    }

    if (wakeup(TokenType::CONTINUE, true)) {
      expectSemi(Statement);
      return Result::success(Statement::simple(Statement::Type::CONTINUE));
    }

    if (wakeup(TokenType::FOR, true)) {
      For* for_;
      expect(Statement, For, for_, for_process());
      return Result::success(Statement::for_(for_));
    }

    if (wakeup(TokenType::DOUBLE_COLON, true)) {
      expect(Statement, Identifier, name, rawIdentifier_process());
      expectSemi(Statement);
      return Result::success(Statement::withName(Statement::Type::LABEL, name));
    }

    if (wakeup(TokenType::GOTO, true)) {
      expect(Statement, Identifier, name, identifier_process());
      expectSemi(Statement);
      return Result::success(Statement::withName(Statement::Type::GOTO, name));
    }

    if (using_wakeup()) {
      Using* using_;
      expect(Statement, Using, using_, using_process());
      expectSemi(Statement);
      return Result::success(Statement::using_(using_));
    }

    if (scope_wakeup()) {
      Scope* scope;
      expect(Statement, Scope, scope, scope_process());
      return Result::success(Statement::scope(scope));
    }

    return forCompatibleStatement_process();
  }

  Result::inst<Statement> SyntaxChecker::forCompatibleStatement_process() {
    if (wakeup(TokenType::SEMI, true))
      return Result::success(Statement::simple(Statement::Type::NOTHING));

    if (variables_wakeup()) {
      Variables* vars;
      expect(Statement, Variables, vars, variables_process());
      expectSemi(Statement);
      return Result::success(Statement::varDecl(vars));
    }

    Expression* expr;
    expect(Statement, Expression, expr, expression_process());
    expectSemi(Statement);
    return Result::success(Statement::withExpr(Statement::Type::EXPRESSION, expr));
  }

  Result::inst<StatementAndExpr> SyntaxChecker::exprAndStatement_process() {
    if (!wakeup(TokenType::PARENTS, false))
      return Result::error<StatementAndExpr>(parentsError());
    Context previous = switchContextToParents();
    Expression* expr;
    Statement* statement;

    expectWithAlways(StatementAndExpr, Expression, expr, expression_process(), expectParentEnd(StatementAndExpr, previous));
    expect(StatementAndExpr, Statement, statement, statement_process());
    
    return Result::success(
      new StatementAndExpr {
        .statement = statement,
        .expr = expr,
      }
    );
  }
  
  Result::inst<DoWhile> SyntaxChecker::doWhile_process() {
    Statement* doStatement;
    StatementAndExpr* whileStatementAndExpr;

    expect(DoWhile, Statement, doStatement, statement_process());
    if (!wakeup(TokenType::WHILE, true))
      return Result::error<DoWhile>(syntaxError("Expected 'while'"));
    expect(DoWhile, StatementAndExpr, whileStatementAndExpr, exprAndStatement_process());
    
    return Result::success(
      new DoWhile{
        .doStatement = doStatement,
        .whileStatementAndExpr = whileStatementAndExpr,
      }
    );
  }

  Result::inst<For> SyntaxChecker::for_process() {
    Statement* initStatement;
    Expression* checkExpression = NULL;
    Expression* repeatExpression = NULL;
    Statement* statement;
    
    if (!wakeup(TokenType::PARENTS, false))
      return Result::error<For>(parentsError());
    Context previous = switchContextToParents();

    expectWithOnError(For, Statement, initStatement, forCompatibleStatement_process(), switchContext(previous));
    if (!wakeup(TokenType::SEMI, true)) {
      expectWithOnError(For, Expression, checkExpression, expression_process(), switchContext(previous));
      expectSemi(For);
    }

    if (hasPeek())
      expectWithOnError(For, Expression, repeatExpression, expression_process(), switchContext(previous));
    expectParentEnd(For, previous);
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

  
  bool SyntaxChecker::using_wakeup() {
    return wakeup(TokenType::USING, true);
  }
  
  Result::inst<Using> SyntaxChecker::using_process() {
    if (!typeDef_wakeup())
      return Result::error<Using>(syntaxError("Expected ':'"));  //TODO: Other using types

    TypeDef* typeDef;
    expect(Using, TypeDef, typeDef, typeDef_process());
    return Result::success(Using::typeDef(typeDef));
  }

  bool SyntaxChecker::typeDef_wakeup() {
    return wakeup(TokenType::COLON, true);
  }

  Result::inst<TypeDef> SyntaxChecker::typeDef_process() {
    Type* from;
    Type* to;
    std::vector<Type*>* other = NULL;

    expect(TypeDef, Type, from, type_process());

    bool isInt = from->type == Type::TypeT::INT;
    bool isSpecialCase = (isInt || from->type == Type::TypeT::FLOAT) && wakeup(TokenType::LESS, true);
    if (isSpecialCase) {
      Expression* expr;
      Literal literal;
      int bitAmount;

      expect(TypeDef, Expression, expr, baseExpression_process());
      if (expr->type != Expression::Type::LITERAL || (literal = expr->u.literal).type != LiteralType::INTEGER)
        return Result::error<TypeDef>(syntaxError("Expected integer literal"));
      bitAmount = literal.u.integer;

      if (bitAmount <= 0)
        return Result::error<TypeDef>(syntaxError("Bit amount must be greater than 0"));
      if (!wakeup(TokenType::GREATER, true))
        return Result::error<TypeDef>(syntaxError("Expected '>'"));
      
      from = Type::specialCase(isInt, bitAmount, from->isUnsigned, from->isSigned);
    }
    
    expect(TypeDef, Type, to, baseType_process());

    if (wakeup(TokenType::COMMA, true)) {
      other = new std::vector<Type*>();
      Type* type;
      
      do {
        expect(TypeDef, Type, type, baseType_process());
        other->push_back(type);
      } while (wakeup(TokenType::COMMA, true));
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

    if (wakeup(TokenType::UNSIGNED, true))
      isUnsigned = true;
    else if (wakeup(TokenType::SIGNED, true))
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

    if (wakeup(TokenType::INT, true))
      return success(Type::TypeT::INT);

    else if (wakeup(TokenType::FLOAT, true))
      return success(Type::TypeT::FLOAT);

    else if (wakeup(TokenType::BOOL, true))
      return success(Type::TypeT::BOOL);

    else if (wakeup(TokenType::CSTR, true))
      return success(Type::TypeT::CSTR);

    else if (wakeup(TokenType::CHAR, true))
      return success(Type::TypeT::CHAR);

    else if (wakeup(TokenType::SIZE_T, true))
      return success(Type::TypeT::SIZE_T);
    
    #undef success

    Identifier* identifier;
    expect(Type, Identifier, identifier, identifier_process());
    return Result::success(Type::custom(identifier));
  }

  Result::inst<ReturnType> SyntaxChecker::returnType_process() {
    if (wakeup(TokenType::VOID, true))
      return Result::success(ReturnType::void_());

    else if (wakeup(TokenType::EXCLAMATION, true))
      return Result::success(ReturnType::noReturn());
    
    Type* type;
    expect(ReturnType, Type, type, type_process());
    return Result::success(ReturnType::fromType(type));
  }


  Result::inst<Identifier> SyntaxChecker::identifier_process() {
    return rawIdentifier_process();
  }

  Result::inst<Identifier> SyntaxChecker::rawIdentifier_process() {
    if (peekEqual({ TokenType::IDENTIFIER }))
      return Result::success(Identifier::simple(consume().value().u.string));
    return Result::error<Identifier>(syntaxError("Expected identifier"));
  }

  
  Result::inst<InitIdentifier> SyntaxChecker::initIdentifier_process() {
    bool isMutable = false;
    bool isConst = false;
    if (wakeup(TokenType::MUT, true))
      isMutable = true;
    else if (wakeup(TokenType::CONST, true))
      isConst = true;
    
    Identifier* name;
    expect(InitIdentifier, Identifier, name, rawIdentifier_process());
    
    InitExpression* initExpr = NULL;
    if (wakeup(TokenType::EQUALS, true))
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
    expect(InitExpression, Expression, expr, expression_process());
    return Result::success(InitExpression::expression(expr));
  }


  Result::inst<Expression> SyntaxChecker::expression_process() {
    Result::inst<Expression> ret = baseExpression_process();
    if (ret.isError())
      return ret;

    index_t opIndex;
    Expression* left;
    Operator* op;
    Expression* right;
    while ((opIndex = operator_wakeup_index()) != INDEX_T_NOT_FOUND) {
      op = operatorFromIndex(opIndex);

      left = ret.value;
      expect(Expression, Expression, right, baseExpression_process());

      ret = Result::success(Expression::binaryOp(left, op, right));
    }
    
    return ret;
  }

  Result::inst<Expression> SyntaxChecker::baseExpression_process() {
    Identifier* name;
    Expression* expr;

    if (literalExpression_wakeup())
      return literalExpression_process();

    else if (wakeup(TokenType::PARENTS, false)) {
      Context previous = switchContextToParents();
      
      expectWithAlways(Expression, Expression, expr, expression_process(), expectParentEnd(Expression, previous));
      return Result::success(Expression::withExpr(Expression::Type::EXPR, expr));
    }
    
    else if (wakeup(TokenType::MINUSMINUS, true)) {
      expect(Expression, Identifier, name, identifier_process());
      return Result::success(Expression::withName(Expression::Type::PRE_DECREMENT, name));
    }

    else if (wakeup(TokenType::PLUSPLUS, true)) {
      expect(Expression, Identifier, name, identifier_process());
      return Result::success(Expression::withName(Expression::Type::PRE_INCREMENT, name));
    }


    //? Calling baseExpresion_process ensures that unary operators are ALWAYS before binary operators
    #define unaryOperator(tokenType, exprType) do {                        \
        if (wakeup(tokenType, true)) {                                     \
          expect(Expression, Expression, expr, baseExpression_process());  \
          return Result::success(Expression::withExpr(exprType, expr));    \
        }                                                                  \
      } while(0)

    unaryOperator(TokenType::EXCLAMATION, Expression::Type::NOT);
    unaryOperator(TokenType::TILDE, Expression::Type::BIT_NOT);
    unaryOperator(TokenType::MINUS, Expression::Type::MINUS);
    unaryOperator(TokenType::PLUS, Expression::Type::PLUS);

    #undef unaryOperator
    
    
    expect(Expression, Identifier, name, identifier_process());
    
    if (wakeup(TokenType::EQUALS, true)) {
      expect(Expression, Expression, expr, expression_process());
      return Result::success(Expression::varAssign(name, expr));
    }

    else if (wakeup(TokenType::PARENTS, false)) {
      Context previous = switchContextToParents();
      std::vector<Expression*>* params = NULL;

      if (hasPeek()) {
        params = new std::vector<Expression*>();
      
        while (true) {
          expectWithOnError(Expression, Expression, expr, expression_process(), switchContext(previous));
          params->push_back(expr);

          if (!wakeup(TokenType::COMMA, true)) {
            if (!hasPeek())
              break;
            
            switchContext(previous);
            return Result::error<Expression>(syntaxError("Expected ','"));
          }
        };
      }

      switchContext(previous);
      return Result::success(Expression::funcCall(name, params));
    }

    else if (wakeup(TokenType::PLUSPLUS, true))
      return Result::success(Expression::withName(Expression::Type::INCREMENT, name));

    else if (wakeup(TokenType::MINUSMINUS, true))
      return Result::success(Expression::withName(Expression::Type::DECREMENT, name));

    else
      return Result::success(Expression::withName(Expression::Type::IDENTIFIER, name));
  }


  const TokenType operatorTokens[] = { 
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

  index_t SyntaxChecker::operator_wakeup_index() {
    for (int i = 0; i < OP_AMOUNT; i++) {
      if (wakeup(operatorTokens[i], true))
        return i;
    }

    return INDEX_T_NOT_FOUND;
  }

  Operator* SyntaxChecker::operatorFromIndex(index_t index) {
    return new Operator(operators[index]);
  }

  bool SyntaxChecker::literalExpression_wakeup() {
    #define nextLiteral() peekEqual({ TokenType::LITERAL }, 1)
    return wakeup(TokenType::LITERAL, false) || (wakeup(TokenType::MINUS, false) && nextLiteral()) || (wakeup(TokenType::PLUS, false) && nextLiteral());
    #undef nextLiteral
  }

  Result::inst<Expression> SyntaxChecker::literalExpression_process() {
    bool isPositive = wakeup(TokenType::PLUS, true);
    bool isNegative = wakeup(TokenType::MINUS, true);
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

    return Result::error<Expression>(Parser::syntaxError("Invalid expression", token.line));
  }


  #undef expect
  #undef expectWithAlways
  #undef expectWithOnError
  #undef expectParentEnd
  #undef expectSemi



  void SyntaxChecker::process() {
    while (hasPeek()) {
      if (wakeup(TokenType::SEMI, true))
        continue; // Technically not needed (better than ';')
      else if (variables_wakeup())
        addToOutput({ GlobalNode::Type::VAR_DECL, { .vars = expectSemiOnResult(variables_process()).expectValue() } });
      else if (function_wakeup())
        addToOutput({ GlobalNode::Type::FUNC, { .func = function_process().expectValue() } });
      else if (using_wakeup())
        addToOutput({ GlobalNode::Type::USING, { .using_ = expectSemiOnResult(using_process()).expectValue() } });
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
