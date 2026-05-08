#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>


#define nullable
#define nullableInCase(case)

namespace Parser {
  struct Identifier {
    const char* name;

    void print(std::ostream& out);
  };

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
    bool doesReturn;
    bool isUnknown;
    bool isVoid;
    nullable Type* type;

    static ReturnType* noReturn();
    static ReturnType* unknown();
    static ReturnType* _void();
    static ReturnType* fromType(Type* type);

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

  struct InitIdentifier {
    Identifier* name;
    nullable InitExpression* expr;

    void print(std::ostream& out);
  };

  struct Variable {
    Type* type;
    InitIdentifier* init;

    void print(std::ostream& out);
  };

  struct Variables {
    Variable* var;
    nullable std::vector<InitIdentifier*>* other;

    void print(std::ostream& out);
  };

  struct VarAssign {
    Identifier* name;
    Expression* expr;
  };

  struct FuncCall {
    Identifier* name;
    nullable std::vector<Expression*>* params;

    void print(std::ostream& out);
  };

  struct Expression {
    enum class Type {
      LITERAL, VAR_ASSIGN, VAR, EXPR, FUNC_CALL
    } type;
    union {
      Literal literal;
      VarAssign* varAssign;
      Identifier* name;
      Expression* expr;
      FuncCall* funcCall;
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

    void print(std::ostream& out);
  };

  struct Statement {
    enum class Type {
      RETURN, VAR_DECL, EXPRESSION, IF, ELSE, SCOPE, NOTHING
    } type;
    union {
      nullableInCase(Type::RETURN) Expression* expr;
      Variables* vars;
      Scope* scope;
      StatementAndExpr* statementAndExpr;
      Statement* statement;
    } u;

    bool ignoresSemi();
    void print(std::ostream& out);
  };


  struct Function {
    Identifier* name;
    ReturnType* returnType;
    nullable std::vector<Variables*>* params;
    bool defined;
    nullable Scope* scope;

    void print(std::ostream& out);
  };

  struct GlobalNode {
    enum class Type {
      VAR_DECL, FUNC
    } type;
    union {
      Variables* vars;
      Function* func;
    } u;

    void print(std::ostream& out);
  };
}
