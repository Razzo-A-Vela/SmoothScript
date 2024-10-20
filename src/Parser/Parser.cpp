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

  FunctionDefinition* Parser::processFunc(Token token) {
    if (!Token::typeEqual(token, { TokenType::func }))
      Utils::error("Parser Error", "Invalid function declaration", token.line);
    
    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Parser Error", "Invalid function identifier", token.line);
    std::string identifier = std::string(consume().value().u.string);

    if (peekEqual({ TokenType::open_paren }, Token::typeEqual)) {
      //TODO: PARAMS
      Utils::error("Internal Error", "Params are not allowed for now", token.line);
    }

    if (peekNotEqual({ TokenType::colon }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected ':' in function declaration");
    consume();

    if (!hasPeek())
      Utils::error("Parser Error", "Expected DataType after ':' in function declaration", token.line);
    DataType* returnType = processDataType(consume().value());

    if (peekEqual({ TokenType::semi_colon }, Token::typeEqual)) {
      //TODO: FUNCTION DECLARATION
      Utils::error("Internal Error", "Function declarations are not allowed for now", token.line);
    }

    if (peekNotEqual({ TokenType::open_brace }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected '{' in function definition");
    consume();

    std::vector<Statement*> statements;
    while (peekNotEqual({ TokenType::closed_brace }, Token::typeEqual)) {
      if (!hasPeek())
        Utils::error("Syntax Error", "Expected '}' in function definition", token.line);
      
      Token currToken = consume().value();
      statements.push_back(processStatement(currToken));
    }
    consume();

    FunctionDefinition* functionDefinition = new FunctionDefinition();
    functionDefinition->id = identifier;
    functionDefinition->returnType = returnType;
    functionDefinition->statements = statements;
    return functionDefinition;
  }
  
  void Parser::process() {
    while (hasPeek()) {
      Token token = consume().value();
      GlobalStatement globalStatement;

      if (Token::typeEqual(token, { TokenType::func })) {
        globalStatement.type = GlobalStatementType::FUNC_DEF;
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
