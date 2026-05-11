#include "ParseNodes.hpp"

namespace Parser {
  void Identifier::print(std::ostream& out) {
    out << name;
  }

  void Type::print(std::ostream& out) {
    if (isConst)
      out << "CONST ";

    if (isUnsigned)
      out << "UNSIGNED ";

    switch (type) {
      case TypeT::INT :
        out << "INT";
        break;
      
      case TypeT::FLOAT :
        out << "FLOAT";
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

  ReturnType* ReturnType::_void() {
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

  void InitExpression::print(std::ostream& out) {
    switch (type) {
      case Type::EXPRESSION :
        out << "INIT ";
        u.expr->print(out);
        break;
    }
  }

  void InitIdentifier::print(std::ostream& out) {
    name->print(out);
    if (expr != NULL) {
      out << " = ";
      expr->print(out);
    }
  }

  void Variable::print(std::ostream& out) {
    type->print(out);
    out << ' ';
    init->print(out);
  }

  void Variables::print(std::ostream& out) {
    out << ':';
    var->print(out);
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
    }
  }
}
