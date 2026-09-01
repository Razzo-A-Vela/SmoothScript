#pragma once
#include <ostream>
#include <vector>

#include <Literal/Literal.hpp>


#define nullable
#define nullableInCase(case)

namespace Parser {
  struct Identifier {
    const char* name;

    static Identifier* simple(const char* name);

    void print(std::ostream& out);
  };

  struct Type {
    enum class TypeT {
      INT, FLOAT, INT_LIT, FLOAT_LIT, INT_SPECIAL, FLOAT_SPECIAL, CUSTOM,
      BOOL, CSTR, CHAR, SIZE_T
      // STRUCT, UNION, BFIELD, ENUM
    } type;
    union {
      Identifier* identifier;
      int bitAmount;
    } u;
    bool isUnsigned;
    bool isSigned;

    static Type* of(TypeT type, bool isUnsigned = false, bool isSigned = false);
    static Type* custom(Identifier* identifier, bool isUnsigned = false, bool isSigned = false);
    static Type* specialCase(bool isInt, int bitAmount, bool isUnsigned = false, bool isSigned = false);

    void print(std::ostream& out);
  };

  struct ReturnType {
    enum class ReturnTypeT {
      UNKNOWN, VOID, NO_RETURN, WITH_TYPE
    } returnType;
    nullable Type* type;

    static ReturnType* noReturn();
    static ReturnType* unknown();
    static ReturnType* void_();
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

    static InitExpression* expression(Expression* expr);

    void print(std::ostream& out);
  };

  struct InitIdentifier {
    bool isMutable;
    bool isConst;
    Identifier* name;
    nullable InitExpression* expr;

    void print(std::ostream& out);
  };

  struct Variables {
    Type* type;
    InitIdentifier* init;
    nullable std::vector<InitIdentifier*>* other;

    void print(std::ostream& out);
  };

  struct VarAssign {
    Identifier* name;
    Expression* expr;

    void print(std::ostream& out);
  };

  struct FuncCall {
    Identifier* name;
    nullable std::vector<Expression*>* params;

    void print(std::ostream& out);
  };

  struct Operator {
    enum class Type {
      ADD, SUB, MULT, DIV,
      LESS, LESS_EQ, SHIFT_LEFT,
      GREATER, GREATER_EQ, SHIFT_RIGHT,
      AND, BIT_AND, OR, BIT_OR,
      EQUALS, NOT_EQUALS
    } type;
    int precedence;

    void print(std::ostream& out);
  };

  struct BinaryOp {
    Expression* left;
    Operator* op;
    Expression* right;

    void print(std::ostream& out);
  };

  struct Expression {
    enum class Type {
      LITERAL, VAR_ASSIGN, IDENTIFIER, EXPR, FUNC_CALL, INCREMENT, DECREMENT,
      BINARY_OP, NOT, BIT_NOT, MINUS, PLUS, PRE_DECREMENT, PRE_INCREMENT
    } type;
    union {
      Literal literal;
      VarAssign* varAssign;
      Identifier* name;
      Expression* expr;
      FuncCall* funcCall;
      BinaryOp* binaryOp;
    } u;
    ReturnType* returnType;

    static Expression* binaryOp(Expression* left, Operator* op, Expression* right);
    static Expression* withExpr(Type type, Expression* expr);
    static Expression* withName(Type type, Identifier* name);
    static Expression* literal(Literal literal, ReturnType* returnType);
    static Expression* varAssign(Identifier* name, Expression* expr);
    static Expression* funcCall(Identifier* name, std::vector<Expression*>* params);

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

  struct DoWhile {
    Statement* doStatement;
    StatementAndExpr* whileStatementAndExpr;

    void print(std::ostream& out);
  };

  struct For {
    Statement* initStatement;
    nullable Expression* checkExpression;
    nullable Expression* repeatExpression;
    Statement* statement;

    void print(std::ostream& out);
  };

  struct TypeDef {
    Type* from;
    Type* to;
    nullable std::vector<Type*>* other;

    void print(std::ostream& out);
  };

  struct Using {
    enum class Type {
      TYPE_DEF
    } type;
    union {
      TypeDef* typeDef;
    } u;

    static Using* typeDef(TypeDef* typeDef);
    
    void print(std::ostream& out);
  };

  struct Statement {
    enum class Type {
      RETURN, IF, ELSE, WHILE, DO_WHILE, LOOP, BREAK,
      CONTINUE, FOR, LABEL, GOTO, USING,
      
      VAR_DECL, SCOPE, EXPRESSION, NOTHING
    } type;
    union {
      nullableInCase(Type::RETURN) Expression* expr;
      Variables* vars;
      Scope* scope;
      StatementAndExpr* statementAndExpr;
      Statement* statement;
      DoWhile* doWhile;
      For* for_;
      Identifier* name;
      Using* using_;
    } u;

    static Statement* simple(Type type);
    static Statement* withName(Type type, Identifier* name);
    static Statement* withExpr(Type type, Expression* expr);
    static Statement* withStatement(Type type, Statement* statement);
    static Statement* withStatementAndExpr(Type type, StatementAndExpr* statementAndExpr);
    static Statement* return_(nullable Expression* expr);
    static Statement* doWhile(DoWhile* doWhile);
    static Statement* varDecl(Variables* vars);
    static Statement* using_(Using* using_);
    static Statement* scope(Scope* scope);
    static Statement* for_(For* for_);

    void print(std::ostream& out);
  };


  struct Function {
    Identifier* name;
    ReturnType* returnType;
    nullable std::vector<Variables*>* params;
    bool defined;
    nullable Scope* scope;

    static Function* declaration(Identifier* name, ReturnType* returnType, nullable std::vector<Variables*>* params);
    static Function* definition(Identifier* name, ReturnType* returnType, nullable std::vector<Variables*>* params, Scope* scope);

    void print(std::ostream& out);
  };

  struct GlobalNode {
    enum class Type {
      VAR_DECL, FUNC, USING
    } type;
    union {
      Variables* vars;
      Function* func;
      Using* using_;
    } u;

    void print(std::ostream& out);
  };
}
