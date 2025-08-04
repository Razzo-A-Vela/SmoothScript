#include "ParseNodes.hpp"

namespace Parser {
  void Type::printIntSize(IntSize intSize, std::ostream& out) {
    switch (intSize) {
      case IntSize::INT8 :
        out << "INT8";
        break;
      
      case IntSize::INT16 :
        out << "INT16";
        break;

      case IntSize::INT32 :
        out << "INT32";
        break;

      case IntSize::INT64 :
        out << "INT64";
        break;
    }
  }

  void Type::print(std::ostream& out) {
    switch (type) {
      case TypeT::INT :
        printIntSize(u.intSize, out);
        break;
    }
  }

  void ReturnType::print(std::ostream& out) {
    if (isVoid)
      out << "void";
    else
      type->print(out);
  }

  void Variable::print(std::ostream& out) {
    type->print(out);
    out << ": " << name;
    if (initialValue != NULL) {
      out << " = ";
      initialValue->print(out);
    }
  }

  void Expression::print(std::ostream& out) {
    out << "{ ";
    returnType->print(out);
    out << " } ";

    switch (type) {
      case Type::LITERAL :
        u.literal.print(out);   //TODO: CHANGE THIS PRINT WITH A CUSTOM PARSER PRINT
        break;
      
      case Type::VAR_ASSIGN :
        out << u.varAssign->name << " = ";
        u.varAssign->expr->print(out);
        break;
    }
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
      
      case Type::VAR_DECL :
        out << "VAR_DECL(";
        u.var->print(out);
        out << ");";
        break;
      
      case Type::EXPRESSION :
        out << "EXPRESSION(";
        u.expr->print(out);
        out << ");";
        break;
      
      case Type::IF :
        out << "IF(";
        u.statementAndExpr->expr->print(out);
        out << ") ";
        u.statementAndExpr->statement->print(out);
        break;
      
      case Type::SCOPE :
        u.scope->print(out);
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

  void Function::print(std::ostream& out) {
    out << name << "()";    //TODO: PARAMS
    out << "->";
    returnType->print(out);
    out << ' ';
    scope->print(out);
  }

  void GlobalNode::print(std::ostream& out) {
    switch (type) {
      case Type::VAR :
        out << "VAR(";
        u.var->print(out);
        out << ')';
        break;
      
      case Type::FUNC :
        out << "FUNC(";
        u.func->print(out);
        out << ')';
        break;
    }
  }
}
