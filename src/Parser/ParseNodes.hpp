#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>

namespace Parser {
  struct Type {
    enum class TypeT {
      INT
      // FLOAT, CUSTOM,
      // BOOL, CSTR, CHAR, SIZE_T,
      // STRUCT, UNION, BFIELD, ENUM
    } type;

    void print(std::ostream& out);
  };

  struct ReturnType {
    bool isVoid;
    Type* type;

    void print(std::ostream& out);
  };

  struct Expression;
  struct Statement;

  struct InitExpression {
    enum class Type {
      EXPRESSION
    } type;
    union {
      Expression* expr;
    } u;

    void print(std::ostream& out);
  };

  struct Variable {
    Type* type;
    const char* name;
    InitExpression* init;

    void print(std::ostream& out);
  };

  struct VarAssign {
    const char* name;
    Expression* expr;
  };

  struct Expression {
    enum class Type {
      LITERAL, VAR_ASSIGN
    } type;
    union {
      Literal literal;
      VarAssign* varAssign;
    } u;
    ReturnType* returnType;

    void print(std::ostream& out);
  };

  struct Scope {
    std::vector<Statement*>* statements;
    int depth;

    void print(std::ostream& out);
  };

  struct StatementAndExpr {
    Statement* statement;
    Expression* expr;
  };

  struct Statement {
    enum class Type {
      RETURN, VAR_DECL, EXPRESSION, IF, SCOPE, NOTHING
    } type;
    union {
      Expression* expr;
      Variable* var;
      Scope* scope;
      StatementAndExpr* statementAndExpr;
    } u;

    void print(std::ostream& out);
  };


  struct Function {
    const char* name;
    ReturnType* returnType;
    Scope* scope;

    void print(std::ostream& out);
  };

  struct GlobalNode {
    enum class Type {
      VAR, FUNC
    } type;
    union {
      Variable* var;
      Function* func;
    } u;

    void print(std::ostream& out);
  };
}
