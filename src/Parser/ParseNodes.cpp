#include "ParseNodes.hpp"

namespace Parser {
  Identifier* Identifier::simple(const char* name) {
    return new Identifier {
      .name = name
    };
  }

  void Identifier::print(std::ostream& out) {
    out << name;
  }


  Type* Type::of(Type::TypeT type, bool isUnsigned, bool isSigned) {
    return new Type{ type, NULL, isUnsigned, isSigned };
  }

  Type* Type::custom(Identifier* identifier, bool isUnsigned, bool isSigned) {
    return new Type{ Type::TypeT::CUSTOM, { .identifier = identifier }, isUnsigned, isSigned };
  }

  Type* Type::specialCase(bool isInt, int bitAmount, bool isUnsigned, bool isSigned) {
    return new Type{ isInt ? Type::TypeT::INT_SPECIAL : Type::TypeT::FLOAT_SPECIAL, { .bitAmount = bitAmount }, isUnsigned, isSigned };
  }

  void Type::print(std::ostream& out) {
    if (isUnsigned)
      out << "UNSIGNED ";

    if (isSigned)
      out << "SIGNED ";

    switch (type) {
      case TypeT::INT :
        out << "INT";
        break;
      
      case TypeT::FLOAT :
        out << "FLOAT";
        break;
      
      case TypeT::INT_LIT :
        out << "INT_LIT";
        break;
      
      case TypeT::FLOAT_LIT :
        out << "FLOAT_LIT";
        break;
      
      case TypeT::INT_SPECIAL :
        out << "INT_SPECIAL<" << u.bitAmount << '>';
        break;
      
      case TypeT::FLOAT_SPECIAL :
        out << "FLOAT_SPECIAL<" << u.bitAmount << '>';
        break;
      
      case TypeT::CUSTOM :
        out << "CUSTOM(";
        u.identifier->print(out);
        out << ')';
        break;
      
      case TypeT::BOOL :
        out << "BOOL";
        break;
      
      case TypeT::CSTR :
        out << "CSTR";
        break;
      
      case TypeT::CHAR :
        out << "CHAR";
        break;
      
      case TypeT::SIZE_T :
        out << "SIZE_T";
        break;
    }
  }


  ReturnType* ReturnType::noReturn() {
    return new ReturnType{ false, false, false, NULL };
  }

  ReturnType* ReturnType::unknown() {
    return new ReturnType{ true, true, false, NULL };
  }

  ReturnType* ReturnType::void_() {
    return new ReturnType{ true, false, true, NULL };
  }

  ReturnType* ReturnType::fromType(Type* type) {
    return new ReturnType{ true, false, false, type };
  }

  void ReturnType::print(std::ostream& out) {
    if (!doesReturn)
      out << "noReturn";
    else if (isVoid)
      out << "void";
    else if (isUnknown)
      out << "unknown";
    else
      type->print(out);
  }


  InitExpression* InitExpression::expression(Expression* expr) {
    return new InitExpression {
      .type = InitExpression::Type::EXPRESSION,
      .u = { .expr = expr }
    };
  }

  void InitExpression::print(std::ostream& out) {
    switch (type) {
      case Type::EXPRESSION :
        out << "INIT ";
        u.expr->print(out);
        break;
    }
  }


  void InitIdentifier::print(std::ostream& out) {
    if (isMutable)
      out << "MUT ";
    else if (isConst)
      out << "CONST ";

    name->print(out);
    if (expr != NULL) {
      out << " = ";
      expr->print(out);
    }
  }


  void Variables::print(std::ostream& out) {
    out << ':';
    type->print(out);
    out << ' ';
    init->print(out);

    if (other != NULL) {
      for (int i = 0; i < other->size(); i++) {
        out << ", ";
        other->at(i)->print(out);
      }
    }
  }


  void VarAssign::print(std::ostream& out) {
    name->print(out);
    out << " = ";
    expr->print(out);
  }


  void FuncCall::print(std::ostream& out) {
    name->print(out);
    out << '(';
    
    if (params != NULL) {
      params->at(0)->print(out);

      for (int i = 1; i < params->size(); i++) {
        out << ", ";
        params->at(i)->print(out);
      }
    }

    out << ')';
  }


  void Operator::print(std::ostream& out) {
    out << " (";
    switch (type) {
      case Type::ADD :
        out << '+';
        break;
      
      case Type::SUB :
        out << '-';
        break;
      
      case Type::MULT :
        out << '*';
        break;
      
      case Type::DIV :
        out << '/';
        break;
      

      case Type::LESS :
        out << '<';
        break;
      
      case Type::LESS_EQ :
        out << "<=";
        break;
      
      case Type::SHIFT_LEFT :
        out << "<<";
        break;


      case Type::GREATER :
        out << '>';
        break;
      
      case Type::GREATER_EQ :
        out << ">=";
        break;
      
      case Type::SHIFT_RIGHT :
        out << ">>";
        break;
      
      
      case Type::AND :
        out << "&&";
        break;
      
      case Type::BIT_AND:
        out << '&';
        break;
      
      case Type::OR :
        out << "||";
        break;
      
      case Type::BIT_OR :
        out << '|';
        break;
      
      
      case Type::EQUALS :
        out << "==";
        break;
      
      case Type::NOT_EQUALS :
        out << "!=";
        break;
    }

    out << " {" << precedence << "}) ";
  }


  void BinaryOp::print(std::ostream& out) {
    left->print(out);
    op->print(out);
    right->print(out);
  }


  Expression* Expression::binaryOp(Expression* left, Operator* op, Expression* right) {
    return new Expression {
      .type = Expression::Type::BINARY_OP,
      .u = { .binaryOp = new BinaryOp{ left, op, right } },
      .returnType = ReturnType::unknown(),
    };
  }

  Expression* Expression::withExpr(Expression::Type type, Expression* expr) {
    return new Expression {
      .type = type,
      .u = { .expr = expr },
      .returnType = ReturnType::unknown(),
    };
  }

  Expression* Expression::withName(Expression::Type type, Identifier* name) {
    return new Expression {
      .type = type,
      .u = { .name = name },
      .returnType = ReturnType::unknown(),
    };
  }

  Expression* Expression::literal(Literal literal, ReturnType* returnType) {
    return new Expression {
      .type = Expression::Type::LITERAL,
      .u = { .literal = literal },
      .returnType = returnType,
    };
  }

  Expression* Expression::varAssign(Identifier* name, Expression* expr) {
    return new Expression {
      .type = Expression::Type::VAR_ASSIGN,
      .u = { .varAssign = new VarAssign{ name, expr } },
      .returnType = ReturnType::unknown(),
    };
  }

  Expression* Expression::funcCall(Identifier* name, std::vector<Expression*>* params) {
    return new Expression {
      .type = Expression::Type::FUNC_CALL,
      .u = { .funcCall = new FuncCall{ name, params } },
      .returnType = ReturnType::unknown(),
    };
  }

  void Expression::print(std::ostream& out) {
    out << '(';
    switch (type) {
      case Type::LITERAL :
        u.literal.print(out);   //TODO: CHANGE THIS PRINT WITH A CUSTOM PARSER PRINT
        break;
      
      case Type::IDENTIFIER :
        u.name->print(out);
        break;

      case Type::VAR_ASSIGN :
        u.varAssign->print(out);
        break;
      
      case Type::EXPR :
        u.expr->print(out);
        break;
      
      case Type::FUNC_CALL :
        u.funcCall->print(out);
        break;
      
      case Type::INCREMENT :
        u.name->print(out);
        out << "++";
        break;
      
      case Type::DECREMENT :
        u.name->print(out);
        out << "--";
        break;
      
      case Type::BINARY_OP :
        u.binaryOp->print(out);
        break;
      
      case Type::NOT :
        out << "NOT ";
        u.expr->print(out);
        break;
      
      case Type::BIT_NOT :
        out << "BIT NOT ";
        u.expr->print(out);
        break;
      
      case Type::MINUS :
        out << "MINUS ";
        u.expr->print(out);
        break;
      
      case Type::PLUS :
        out << "PLUS ";
        u.expr->print(out);
        break;
      
      case Type::PRE_DECREMENT :
        out << "PRE_DECREMENT ";
        u.name->print(out);
        break;
      
      case Type::PRE_INCREMENT :
        out << "PRE_INCREMENT ";
        u.name->print(out);
        break;
    }

    out << ") -> ";
    returnType->print(out);
  }


  void DoWhile::print(std::ostream& out) {
    out << "DO ";
    doStatement->print(out);
    
    out << " WHILE ";
    whileStatementAndExpr->print(out);
  }


  void For::print(std::ostream& out) {
    out << "FOR (";
    initStatement->print(out);
    out << " ";

    if (checkExpression != NULL)
      checkExpression->print(out);
    out << "; ";
    
    if (repeatExpression != NULL)
      repeatExpression->print(out);
    out << ") ";
    
    statement->print(out);
  }


  void TypeDef::print(std::ostream& out) {
    out << ':';
    from->print(out);
    out << ' ';
    to->print(out);

    if (other != NULL) {
      for (int i = 0; i < other->size(); i++) {
        out << ", ";
        other->at(i)->print(out);
      }
    }
  }


  Using* Using::typeDef(TypeDef* typeDef) {
    return new Using {
      .type = Using::Type::TYPE_DEF,
      .u = { .typeDef = typeDef },
    };
  }

  void Using::print(std::ostream& out) {
    out << "USING ";
    
    switch (type) {
      case Type::TYPE_DEF :
        u.typeDef->print(out);
        break;
    }
  }


  Statement* Statement::simple(Type type) {
    return new Statement {
      .type = type,
    };
  }

  Statement* Statement::withName(Type type, Identifier* name) {
    return new Statement {
      .type = type,
      .u = { .name = name },
    };
  }

  Statement* Statement::withExpr(Type type, Expression* expr) {
    return new Statement {
      .type = type,
      .u = { .expr = expr },
    };
  }

  Statement* Statement::withStatement(Type type, Statement* statement) {
    return new Statement {
      .type = type,
      .u = { .statement = statement },
    };
  }

  Statement* Statement::withStatementAndExpr(Type type, StatementAndExpr* statementAndExpr) {
    return new Statement {
      .type = type,
      .u = { .statementAndExpr = statementAndExpr },
    };
  }

  Statement* Statement::return_(nullable Expression* expr) {
    return new Statement {
      .type = Statement::Type::RETURN,
      .u = { .expr = expr },
    };
  }

  Statement* Statement::doWhile(DoWhile* doWhile) {
    return new Statement {
      .type = Statement::Type::DO_WHILE,
      .u = { .doWhile = doWhile },
    };
  }

  Statement* Statement::varDecl(Variables* vars) {
    return new Statement {
      .type = Statement::Type::VAR_DECL,
      .u = { .vars = vars },
    };
  }

  Statement* Statement::using_(Using* using_) {
    return new Statement {
      .type = Statement::Type::USING,
      .u = { .using_ = using_ },
    };
  }

  Statement* Statement::scope(Scope* scope) {
    return new Statement {
      .type = Statement::Type::SCOPE,
      .u = { .scope = scope },
    };
  }

  Statement* Statement::for_(For* for_) {
    return new Statement {
      .type = Statement::Type::FOR,
      .u = { .for_ = for_ },
    };
  }


  void Statement::print(std::ostream& out) {
    switch (type) {
      case Type::RETURN :
        out << "return";
        if (u.expr != NULL) {
          out << ' ';
          u.expr->print(out);
        }
        out << ';';
        break;
      
      case Type::IF :
        out << "IF ";
        u.statementAndExpr->print(out);
        break;
      
      case Type::ELSE :
        out << "ELSE ";
        u.statement->print(out);
        break;
      
      case Type::WHILE :
        out << "WHILE ";
        u.statementAndExpr->print(out);
        break;
      
      case Type::DO_WHILE :
        u.doWhile->print(out);
        break;
      
      case Type::LOOP :
        out << "LOOP ";
        u.statement->print(out);
        break;
      
      case Type::BREAK :
        out << "BREAK;";
        break;
      
      case Type::CONTINUE :
        out << "CONTINUE;";
        break;
      
      case Type::FOR :
        u.for_->print(out);
        break;
      
      case Type::LABEL :
        out << "::";
        u.name->print(out);
        out << ';';
        break;
      
      case Type::GOTO :
        out << "GOTO ";
        u.name->print(out);
        out << ';';
        break;
      
      case Type::USING :
        u.using_->print(out);
        out << ';';
        break;
      

      case Type::VAR_DECL :
        out << "VAR_DECL (";
        u.vars->print(out);
        out << ");";
        break;
      
      case Type::SCOPE :
        u.scope->print(out);
        break;
      
      case Type::EXPRESSION :
        out << "EXPRESSION ( ";
        u.expr->print(out);
        out << " );";
        break;
      
      case Type::NOTHING :
        out << ';';
        break;
    }
  }


  void Scope::print(std::ostream& out) {
    out << "{\n";
    for (int i = 0; i < statements->size(); i++) {
      for (int j = 0; j < depth; j++)
        out << "  ";
      statements->at(i)->print(out);
      out << '\n';
    }
    for (int j = 0; j < depth - 1; j++)
      out << "  ";
    out << '}';
  }


  void StatementAndExpr::print(std::ostream& out) {
    out << '(';
    expr->print(out);
    out << ") ";
    statement->print(out);
  }


  Function* Function::declaration(Identifier* name, ReturnType* returnType, nullable std::vector<Variables*>* params) {
    return new Function {
      .name = name,
      .returnType = returnType,
      .params = params,
      .defined = false,
      .scope = NULL,
    };
  }

  Function* Function::definition(Identifier* name, ReturnType* returnType, nullable std::vector<Variables*>* params, Scope* scope) {
    return new Function {
      .name = name,
      .returnType = returnType,
      .params = params,
      .defined = true,
      .scope = scope,
    };
  }

  void Function::print(std::ostream& out) {
    out << "func ";
    name->print(out);
    out << '(';

    if (params != NULL) {
      params->at(0)->print(out);

      for (int i = 1; i < params->size(); i++) {
        out << ' ';
        params->at(i)->print(out);
      }
    }
    out << ") ";
    returnType->print(out);

    if (defined) {
      out << ' ';
      scope->print(out);
    } else
      out << ';';
  }


  void GlobalNode::print(std::ostream& out) {
    switch (type) {
      case Type::VAR_DECL :
        u.vars->print(out);
        out << ';';
        break;
      
      case Type::FUNC :
        u.func->print(out);
        break;
      
      case Type::USING :
        u.using_->print(out);
        out << ';';
        break;
    }
  }
}
