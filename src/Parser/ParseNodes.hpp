#pragma once
#include <iostream>
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
  
    void print();
  };


  enum class ExpressionType {
    LITERAL
  };

  struct Expression {
    ExpressionType type;
    union {
      Tokenizer::Literal literal;
    } u;

    void print();
  };


  struct Statement;

  struct Scope {
    std::vector<Statement*> statements;

    void print();
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

    void print();
  };
  

  struct FunctionDeclaration {
    std::string name;
    DataType* returnType;
  };

  struct Function {
    bool hasDefinition;
    FunctionDeclaration funcDecl;
    Scope* scope;

    void print();
  };
  

  enum class GlobalStatementType {
    FUNC
  };

  struct GlobalStatement {
    GlobalStatementType type;
    union {
      Function* func;
    } u;

    void print();
  };
}
