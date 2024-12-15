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

  bool DataType::operator==(DataType& other)  {
    if (type != other.type)
      return false;

    if (type == DataTypeT::BYTE_TYPE)
      return u.integer == other.u.integer;
    return true;
  }


  void FunctionDeclaration::print(std::ostream& out) {
    //TODO: Parameters
    out << name << "()" << " : ";
    returnType->print(out);
  }

  bool FunctionDeclaration::operator==(FunctionDeclaration& other) {
    return name == other.name && *returnType == *other.returnType;
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
    out << "FUNC_" << (hasDefinition ? "DEF" : "DECL") << '(';
    funcDecl->print(out);
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
