#pragma once
#include <iostream>
#include <vector>

#include <Tokenizer/Token.hpp>
#include <Tokenizer/Literal.hpp>
#include <util/Map.hpp>

#define POINTER_SIZE 8

namespace Parser {
  enum class DataTypeT {
    BYTE_TYPE, POINTER, VOID
  };
  
  struct DataType {
    DataTypeT type;
    bool isMutable;
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


  enum class OperatorType {
    BINARY, BEFORE, AFTER
  };

  struct Operator {
    OperatorType type = OperatorType::BINARY;
    int precedence = 0;
    Tokenizer::Token symbol1;
    Tokenizer::Token* symbol2 = NULL;

    void print();
  };


  enum class FunctionType {
    DEFAULT, INLINE, ENTRY, EXTERN
  };

  struct FunctionParameters {
    FunctionType type = FunctionType::DEFAULT;
    std::string externIdentifier;
    bool noReturn = false;
    bool cast = false;
    bool op = false;
    Operator oper;

    void print();
  };

  struct Function {
    std::string id;
    DataType* returnType;
    Map<std::string, DataType*> params;
    std::vector<Statement*> statements;
    FunctionParameters funcParams;
    bool hasDefinition = true;

    void print();
  };


  enum class GlobalStatementType {
    FUNCTION
  };

  struct GlobalStatement {
    GlobalStatementType type;
    union {
      Function* funcDef;
    } u;

    void print();
  };
}
