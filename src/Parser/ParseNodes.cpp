#include "ParseNodes.hpp"

namespace Parser {
  void DataType::print() {
    switch (type) {
      case DataTypeT::VOID :
        std::cout << "VOID";
        break;
      
      case DataTypeT::BYTE_TYPE :
        std::cout << "BYTE_TYPE(" << u.integer << ')';
        break;
    }
  }

  void Scope::print() {
    std::cout << "{\n";
    for (Statement* statement : statements) {
      statement->print();
      std::cout << '\n';
    }
    std::cout << '}';
  }

  void Expression::print() {
    switch (type) {
      case ExpressionType::LITERAL :
        std::cout << "LITERAL(";
        u.literal.print();
        std::cout << ')';
        break;
    }
  }

  void Statement::print() {
    switch (type) {
      case StatementType::SCOPE :
        u.scope->print();
        break;
      
      case StatementType::RETURN :
        std::cout << "RETURN(";
        u.expression->print();
        std::cout << ')';
        break;
    }
  }

  void Function::print() {
    std::cout << "FUNC_";
    std::cout << (hasDefinition ? "DEF" : "DECL");
    std::cout << '(' << funcDecl.name << ", ";
    funcDecl.returnType->print();
    std::cout << ')';

    if (hasDefinition) {
      std::cout << ' ';
      scope->print();
    }
  }

  void GlobalStatement::print() {
    switch (type) {
      case GlobalStatementType::FUNC :
        u.func->print();
    }
  }
}
