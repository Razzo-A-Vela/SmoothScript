#include "Parser.hpp"

namespace Parser {
  Operator Parser::processFunctionOperator(int errLine) {
    Operator ret;

    if (peekNotEqual({ TokenType::open_paren }, Token::typeEqual))
      Utils::error("Parser Error", "Expected '(' after $op", errLine);
    consume();
    
    if (!(hasPeek() && Token::isSymbol(peekValue())))
      Utils::error("Parser Error", "Expected Symbol after '(' in $op", errLine);
    ret.symbol1 = consume().value();

    if (hasPeek() && Token::isSymbol(peekValue()) && peekNotEqual({ TokenType::comma }, Token::typeEqual))
      ret.symbol2 = new Token(consume().value());

    if (peekNotEqual({ TokenType::comma }, Token::typeEqual))
      Utils::error("Parser Error", "Expected ',' after symbol in $op", errLine);
    consume();

    if (peekNotEqual({ TokenType::literal }, Token::typeEqual))
      Utils::error("Parser Error", "Expected literal after ',' in $op", errLine);
    Tokenizer::Literal literal = consume().value().u.literal;

    if (literal.type != Tokenizer::LiteralType::integer)
      Utils::error("Parser Error", "Precedence literal must be an integer in $op", errLine);
    ret.precedence = literal.u.integer;

    if (peekEqual({ TokenType::comma }, Token::typeEqual)) {
      consume();

      if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
        Utils::error("Parser Error", "Invalid $op type", errLine);
      std::string opType = std::string(consume().value().u.string);
      
      if (opType == "BINARY")
        ret.type = OperatorType::BINARY;
      else if (opType == "BEFORE")
        ret.type = OperatorType::BEFORE;
      else if (opType == "AFTER")
        ret.type = OperatorType::AFTER;
      else
        Utils::error("Parser Error", "Invalid $op type (only: 'BINARY', 'BEFORE', 'AFTER')", errLine);
    }

    if (peekNotEqual({ TokenType::closed_paren }, Token::typeEqual))
      Utils::error("Parser Error", "Expected ')' at the end of $op", errLine);
    consume();

    return ret;
  }

  bool Parser::isFunctionParameter(std::string parameterType) {
    return parameterType == "extern" ||  parameterType == "entry" || parameterType == "inline" || parameterType == "noReturn" || parameterType == "op" || parameterType == "cast"; 
  }

  FunctionParameters Parser::processOneFunctionParameter(FunctionParameters functionParameters, std::string parameterType, int errLine) {
    if ((parameterType == "inline" || parameterType == "entry" || parameterType == "extern") && functionParameters.type != FunctionType::DEFAULT)
      Utils::error("Parser Error", "Cannot put more than one function type parameter", errLine);

    if (parameterType == "inline")
      functionParameters.type = FunctionType::INLINE;
    else if (parameterType == "entry")
      functionParameters.type = FunctionType::ENTRY;
    else if (parameterType == "extern")
      functionParameters.type = FunctionType::EXTERN;

    else if (parameterType == "cast")
      functionParameters.cast = true;
    else if (parameterType == "noReturn")
      functionParameters.noReturn = true;
    else if (parameterType == "op") {
      functionParameters.op = true;
      functionParameters.oper = processFunctionOperator(errLine);
    }

    return functionParameters;
  }
  
  void Parser::processGlobalParameters(Token token) {
    std::string parameterType = std::string(token.u.string);

    if (isFunctionParameter(parameterType)) {
      FunctionParameters functionParameters = processOneFunctionParameter({}, parameterType, token.line);
      
      while (peekNotEqual({ TokenType::func }, Token::typeEqual)) {
        if (!hasPeek())
          Utils::error("Expected 'func' after function parser parameter", token.line);
        Token currToken = consume().value();
        
        if (!Token::typeEqual(currToken, { TokenType::parserParameter }) || !isFunctionParameter(std::string(currToken.u.string)))
          Utils::error("Parser Error", "Invalid function parser parameter", currToken.line);
        std::string parameterType = std::string(currToken.u.string);

        functionParameters = processOneFunctionParameter(functionParameters, parameterType, currToken.line);
      }

      GlobalStatement globalStatement;
      globalStatement.type = GlobalStatementType::FUNCTION;
      globalStatement.u.funcDef = processFunc(consume().value());
      globalStatement.u.funcDef->funcParams = functionParameters;
      addToOutput(globalStatement);
      
    } else
      Utils::error("Parser Error", "Invalid parserParameter", token.line);
  }
  
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

      if (Token::typeEqual(token, { TokenType::parserParameter })) {
        processGlobalParameters(token);
        continue;

      } else if (Token::typeEqual(token, { TokenType::func })) {
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
