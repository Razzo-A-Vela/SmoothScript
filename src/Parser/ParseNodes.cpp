#include "ParseNodes.hpp"

namespace Parser {
  void DataType::print(std::ostream& out) {
    switch (type) {
      case DataTypeT::VOID :
        out << "VOID";
        break;
      
      case DataTypeT::BYTE_TYPE :
        out << "BYTE_TYPE(" << u.integer << ')';
        break;
    }
  }

  void Scope::print(std::ostream& out) {
    out << "{\n";
    for (Statement* statement : statements) {
      statement->print(out);
      out << '\n';
    }
    out << '}';
  }

  void Expression::print(std::ostream& out) {
    switch (type) {
      case ExpressionType::LITERAL :
        out << "LITERAL(";
        u.literal.print(out);
        out << ')';
        break;
    }
  }

  void Statement::print(std::ostream& out) {
    switch (type) {
      case StatementType::SCOPE :
        u.scope->print(out);
        break;
      
      case StatementType::RETURN :
        out << "RETURN";
        
        if (u.expression != NULL) {
          out << "(";
          u.expression->print(out);
          out << ')';
        }
        break;
    }
  }

  void Function::print(std::ostream& out) {
    out << "FUNC_";
    out << (hasDefinition ? "DEF" : "DECL");
    out << '(' << funcDecl.name << ", ";
    funcDecl.returnType->print(out);
    out << ')';

    if (hasDefinition) {
      out << ' ';
      scopeStatement->print(out);
    }
  }

  void GlobalStatement::print(std::ostream& out) {
    switch (type) {
      case GlobalStatementType::FUNC :
        u.func->print(out);
    }
  }
}
