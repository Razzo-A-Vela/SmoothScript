#include "Parser.hpp"

namespace Parser {
  //TODO: MOVE NAMESPACE FROM PREPROCESSOR TO PARSER

  int Parser::getErrLine() {
    return hasPeek() ? peekValue().line : peekValue(-1).line;
  }

  Expression* Parser::processExpression() {
    if (peekEqual({ TokenType::literal }, Token::typeEqual)) {
      Expression* ret = new Expression();
      ret->type = ExpressionType::LITERAL;
      ret->u.literal = consume().value().u.literal;
      return ret;

    } else if (peekEqual({ TokenType::identifier }, Token::typeEqual)) {
      if (peekEqual({ TokenType::open_paren }, Token::typeEqual, 1)) {
        FunctionDeclaration* funcDecl = new FunctionDeclaration();
        funcDecl->name = std::string(consume().value().u.string);
        int errLine = getErrLine();
        consume();

        //TODO: Parameters
        if (peekNotEqual({ TokenType::closed_paren }, Token::typeEqual))
          Utils::error("Parser Error", "Expected ')' closing function call", errLine);
        consume();

        DataType* returnType = NULL;
        if (peekEqual({ TokenType::open_angolare }, Token::typeEqual)) {
          consume();
          returnType = processDataType();
          if (returnType == NULL)
            Utils::error("Parser Error", "Expected return type after '<' in function call", errLine);
          
          if (peekNotEqual({ TokenType::closed_angolare }, Token::typeEqual))
            Utils::error("Parser Error", "Expected '>' after return type in function call", errLine);
          consume();
        }
        funcDecl->returnType = returnType;
        
        for (Function* savedFunc : functions) {
          if (*savedFunc->funcDecl == *funcDecl) {
            Expression* ret = new Expression();
            ret->type = ExpressionType::FUNC_CALL;
            ret->u.funcDecl = new FunctionDeclaration(*savedFunc->funcDecl);
            return ret;
          }
        }

        std::stringstream funcDeclStream;
        funcDecl->print(funcDeclStream);
        Utils::error("Parser Error", std::string("Function: \"") + funcDeclStream.str() + std::string("\" was not declared"), errLine);
      }
    }

    return NULL;
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

    } else {
      Expression* expression = processExpression();

      if (expression != NULL) {
        if (peekNotEqual({ TokenType::semi_colon }, Token::typeEqual))
          Utils::error("Parser Error", "Expected ';' after expression statement", errLine);
        consume();

        Statement* ret = new Statement();
        ret->type = StatementType::EXPRESSION;
        ret->u.expression = expression;
        return ret;
      }
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
    FunctionDeclaration* funcDecl = new FunctionDeclaration();
    int errLine = getErrLine();

    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Parser Error", "Expected identifier after func", errLine);
    funcDecl->name = std::string(consume().value().u.string);

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
    funcDecl->returnType = returnType;

    Function* ret = new Function();
    ret->hasDefinition = peekNotEqual({ TokenType::semi_colon }, Token::typeEqual);
    if (!ret->hasDefinition) {
      consume();
      ret->scopeStatement = NULL;
    }
    ret->funcDecl = funcDecl;
    
    int foundIndex = -1;
    for (int i = 0; i < functions.size(); i++) {
      Function* savedFunc = functions.at(i);
      if (*savedFunc->funcDecl != *ret->funcDecl)
        continue;
      
      if (!ret->hasDefinition) {
        delete ret;
        return NULL;
      }
      
      if (savedFunc->hasDefinition)
        Utils::error("Parser Error", "Function already defined", errLine);
      
      foundIndex = i;
    }


    if (foundIndex == -1)
      functions.push_back(ret);
    
    if (ret->hasDefinition) {
      Statement* statement = processStatement();
      if (statement->type != StatementType::SCOPE)
        Utils::error("Parser Error", "Expected scope or ';' after function declaration", errLine);
      ret->scopeStatement = statement;

      if (foundIndex != -1) {
        delete functions.at(foundIndex);
        functions.at(foundIndex) = ret;
      }
      return ret;
    }

    return NULL;
  }

  void Parser::process() {
    while (hasPeek()) {
      Token token = consume().value();

      if (Token::typeEqual(token, { TokenType::func })) {
        Function* func = processFunc();
        
        if (func != NULL) {
          GlobalStatement globalStatement;
          globalStatement.type = GlobalStatementType::FUNC;
          globalStatement.u.func = func;
          addToOutput(globalStatement);
        }
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
