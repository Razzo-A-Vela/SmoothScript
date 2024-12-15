#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <Tokenizer/Token.hpp>
#include <Tokenizer/Literal.hpp>
#include <util/Map.hpp>

namespace Parser {
  enum class DataTypeT {
    VOID, BYTE_TYPE
  };

  struct DataType {
    DataTypeT type;
    union {
      unsigned int integer;
    } u;
  
    void print(std::ostream& out);
  };


  enum class ExpressionType {
    LITERAL
  };

  struct Expression {
    ExpressionType type;
    union {
      Tokenizer::Literal literal;
    } u;

    void print(std::ostream& out);
  };


  struct Statement;

  struct Scope {
    std::vector<Statement*> statements;

    void print(std::ostream& out);
  };

  enum class StatementType {
    SCOPE, RETURN
  };

  struct Statement {
    StatementType type;
    union {
      Scope* scope;
      Expression* expression;
    } u;

    void print(std::ostream& out);
  };
  

  struct FunctionDeclaration {
    std::string name;
    DataType* returnType;
  };

  struct Function {
    bool hasDefinition;
    FunctionDeclaration funcDecl;
    Scope* scope;

    void print(std::ostream& out);
  };
  

  enum class GlobalStatementType {
    FUNC
  };

  struct GlobalStatement {
    GlobalStatementType type;
    union {
      Function* func;
    } u;

    void print(std::ostream& out);
  };
}
