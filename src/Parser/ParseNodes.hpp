#pragma once
#include <iostream>
#include <vector>

#include <Tokenizer/Literal.hpp>
#include <util/Map.hpp>

#define POINTER_SIZE 8

namespace Parser {
  enum class DataTypeT {
    BYTE_TYPE, MUTABLE_BYTE_TYPE, POINTER, MUTABLE_POINTER, VOID
  };
  
  struct DataType {
    DataTypeT type;
    union {
      unsigned int byteType;
      DataType* dataType;
    } u;

    void print();
  };

  
  enum class ExpressionType {
    LITERAL
  };

  struct Expression {
    ExpressionType type;
    union {
      Tokenizer::Literal* literal;
    } u;

    void print();
  };
  

  enum class StatementType {
    RETURN
  };

  struct Statement {
    StatementType type;
    union {
      Expression* expression;
    } u;

    void print();
  };


  struct FunctionDefinition {
    std::string id;
    DataType* returnType;
    std::vector<Statement*> statements;

    void print();
  };


  enum class GlobalStatementType {
    FUNC_DEF
  };

  struct GlobalStatement {
    GlobalStatementType type;
    union {
      FunctionDefinition* funcDef;
    } u;

    void print();
  };
}
