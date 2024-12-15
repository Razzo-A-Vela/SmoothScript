#include "Parser.hpp"

namespace Parser {
  //TODO: MOVE NAMESPACE FROM PREPROCESSOR TO PARSER

  int Parser::getErrLine() {
    return hasPeek() ? peekValue().line : peekValue(-1).line;
  }

  Expression* Parser::processExpression() {
    if (peekNotEqual({ TokenType::literal }, Token::typeEqual))
      return NULL;
    
    Expression* ret = new Expression();
    ret->type = ExpressionType::LITERAL;
    ret->u.literal = consume().value().u.literal;
    return ret;
  }

  Statement* Parser::processStatement() {
    int errLine = getErrLine();

    if (peekEqual({ TokenType::open_brace }, Token::typeEqual)) {
      Statement* ret = new Statement();
      
      consume();
      ret->type = StatementType::SCOPE;
      Scope* scope = new Scope();

      while (peekNotEqual({ TokenType::closed_brace }, Token::typeEqual)) {
        if (!hasPeek())
          Utils::error("Parser Error", "Expected '}' to close scope", errLine);
        
        scope->statements.push_back(processStatement());
      }
      consume();
      ret->u.scope = scope;
      return ret;

    } else if (peekEqual({ TokenType::ret }, Token::typeEqual)) {
      Statement* ret = new Statement();

      consume();
      ret->type = StatementType::RETURN;
      if (peekEqual({ TokenType::semi_colon }, Token::typeEqual)) {
        ret->u.expression = NULL;
        consume();
      
      } else {
        Expression* expression = processExpression();
        if (expression == NULL)
          Utils::error("Parser Error", "Invalid return expression", errLine);
        
        if (peekNotEqual({ TokenType::semi_colon }, Token::typeEqual))
          Utils::error("Parser Error", "Expected ';' after return statement", errLine);
        consume();

        ret->u.expression = expression;
      }
      
      return ret;
    }

    Utils::error("Parser Error", "Invalid Statement", errLine);
  }

  DataType* Parser::processDataType() {
    if (peekNotEqual({ TokenType::byte_type }, Token::typeEqual))
      return NULL;
    Token byte_type = consume().value();
    
    DataType* ret = new DataType();
    if (byte_type.u.integer == 0)
      ret->type = DataTypeT::VOID;
    
    else {
      ret->type = DataTypeT::BYTE_TYPE;
      ret->u.integer = byte_type.u.integer;
    }
    return ret;
  }

  Function* Parser::processFunc() {
    int errLine = getErrLine();

    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Parser Error", "Expected identifier after func", errLine);
    std::string funcName = std::string(consume().value().u.string);

    //TODO: Parameters
    if (peekNotEqual({ TokenType::open_paren }, Token::typeEqual) || peekNotEqual({ TokenType::closed_paren }, Token::typeEqual, 1))
      Utils::error("Internal Error", "Expected '()' after func name", errLine);
    consume();
    consume();

    if (peekNotEqual({ TokenType::colon }, Token::typeEqual))
      Utils::error("Parser Error", "Expected ':' after func parameters", errLine);
    consume();

    DataType* returnType = processDataType();
    if (returnType == NULL)
      Utils::error("Parser Error", "Invalid return type", errLine);
    
    Function* func = new Function();
    func->funcDecl.name = funcName;
    func->funcDecl.returnType = returnType;
    func->hasDefinition = false;

    if (peekEqual({ TokenType::semi_colon }, Token::typeEqual)) {
      consume();
      func->scopeStatement = NULL;

    } else {
      func->hasDefinition = true;
      Statement* statement = processStatement();
      if (statement->type != StatementType::SCOPE)
        Utils::error("Parser Error", "Expected scope or ';' after function declaration", errLine);
      func->scopeStatement = statement;
    }

    return func;
  }

  void Parser::process() {
    while (hasPeek()) {
      Token token = consume().value();

      if (Token::typeEqual(token, { TokenType::func })) {
        Function* func = processFunc();

        GlobalStatement globalStatement;
        globalStatement.type = GlobalStatementType::FUNC;
        globalStatement.u.func = func;
        addToOutput(globalStatement);
      } else
        Utils::error("Parser Error", "Invalid GlobalStatement", token.line);
    }
  }

  void Parser::print(std::ostream& out) {
    std::vector<GlobalStatement> output = getOutput();
    for (GlobalStatement globalStatement : output) {
      globalStatement.print(out);
      out << '\n';
    }
  }
}
