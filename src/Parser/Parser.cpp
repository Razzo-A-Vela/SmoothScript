#include "Parser.hpp"

namespace Parser {
  Expression* Parser::processExpression(Token token) {
    Expression* expression = new Expression();

    if (Token::typeEqual(token, { TokenType::literal })) {
      Tokenizer::Literal* literal = new Tokenizer::Literal();
      literal->type = token.u.literal.type;
      literal->u = token.u.literal.u;

      expression->type = ExpressionType::LITERAL;
      expression->u.literal = literal;

    } else
      Utils::error("Parser Error", "Invalid expression", token.line);

    return expression;
  }

  Statement* Parser::processStatement(Token token) {
    Statement* statement = new Statement();
    
    if (Token::typeEqual(token, { TokenType::ret })) {
      statement->type = StatementType::RETURN;
      if (!hasPeek())
        Utils::error("Parser Error", "Expected expression or ';' after return", token.line);

      if (peekEqual({ TokenType::semi_colon }, Token::typeEqual))
        statement->u.expression = NULL;
      else {
        statement->u.expression = processExpression(consume().value());

        if (peekNotEqual({ TokenType::semi_colon }, Token::typeEqual))
          Utils::error("Parser Error", "Expected ';' after return", token.line);
      }
      consume();
      
    } else
      Utils::error("Parser Error", "Invalid statement", token.line);
    
    return statement;
  }
  
  DataType* Parser::processDataType(Token token) {
    DataType* dataType = new DataType();

    if (Token::typeEqual(token, { TokenType::byte_type })) {
      if (token.u.integer == 0) {
        dataType->type = DataTypeT::VOID;

      } else {
        dataType->type = DataTypeT::BYTE_TYPE;
        dataType->u.byteType = token.u.integer;
      }

    } else
      Utils::error("Parser Error", "Invalid dataType", token.line);

    return dataType;
  }

  Function* Parser::processFunc(Token token) {
    if (!Token::typeEqual(token, { TokenType::func }))
      Utils::error("Parser Error", "Invalid function declaration", token.line);
    Function* function = new Function();
    
    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Parser Error", "Invalid function identifier", token.line);
    function->id = std::string(consume().value().u.string);


    if (peekEqual({ TokenType::open_paren }, Token::typeEqual)) {
      consume();

      if (peekNotEqual({ TokenType::closed_paren }, Token::typeEqual)) { 
        while (true) {
          if (!hasPeek())
            Utils::error("Syntax Error", "Expected ')' after '('", token.line);
          Token currToken = consume().value();


          DataType* paramType = processDataType(currToken);
          if (peekNotEqual({ TokenType::question }, Token::typeEqual))
            Utils::error("Syntax Error", "Expected '?' after parameter type", token.line);
          consume();


          if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
            Utils::error("Syntax Error", "Invalid parameter identifier", token.line);
          std::string paramName = std::string(consume().value().u.string);


          function->params.add(paramName, paramType);
          if (peekEqual({ TokenType::closed_paren }, Token::typeEqual))
            break;

          if (peekNotEqual({ TokenType::comma }, Token::typeEqual))
            Utils::error("Syntax Error", "Expected ',' separating parameters", token.line);
          consume();
        }
      }

      consume();
    }

    if (peekNotEqual({ TokenType::colon }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected ':' in function declaration");
    consume();

    if (!hasPeek())
      Utils::error("Parser Error", "Expected DataType after ':' in function declaration", token.line);
    function->returnType = processDataType(consume().value());


    if (peekEqual({ TokenType::semi_colon }, Token::typeEqual)) {
      consume();
      function->hasDefinition = false;
      return function;
    }
    
    if (peekNotEqual({ TokenType::open_brace }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected '{' in function definition");
    consume();


    while (peekNotEqual({ TokenType::closed_brace }, Token::typeEqual)) {
      if (!hasPeek())
        Utils::error("Syntax Error", "Expected '}' in function definition", token.line);
      
      Token currToken = consume().value();
      function->statements.push_back(processStatement(currToken));
    }
    consume();

    return function;
  }
  
  //TODO: MOVE NAMESPACE FROM PREPROCESSOR TO PARSER

  void Parser::process() {
    while (hasPeek()) {
      Token token = consume().value();
      GlobalStatement globalStatement;

      if (Token::typeEqual(token, { TokenType::func })) {
        globalStatement.type = GlobalStatementType::FUNCTION;
        globalStatement.u.funcDef = processFunc(token);
      } else
        Utils::error("Parser Error", "Invalid token in global scope", token.line);

      addToOutput(globalStatement);
    }
  }

  void Parser::print() {
    std::vector<GlobalStatement> output = getOutput();
    for (GlobalStatement globalStatement : output) {
      globalStatement.print();
      std::cout << '\n';
    }
  }
}
