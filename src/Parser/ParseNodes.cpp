#include "ParseNodes.hpp"

namespace Parser {
  // void Expression::print() {
  //   switch (type) {
  //     case ExpressionType::LITERAL:
  //       std::cout << "LITERAL(";
  //       u.literal->print();
  //       std::cout << ')';
  //       break;
  //   }
  // }

  // void DefaultScope::print() {
  //   std::cout << "{\n";
  //   for (int i = 0; i < statements.size(); i++) {
  //     statements.at(i)->print();
  //     std::cout << '\n';
  //   }
  //   std::cout << '}';
  // }

  // void Scope::print() {
  //   switch (type) {
  //     case ScopeType::DEFAULT :
  //       u.defaultScope->print();
  //       break;
  //   }
  // }

  // void Statement::print() {
  //   switch (type) {
  //     case StatementType::SCOPE :
  //       std::cout << "SCOPE(";
  //       u.scope->print();
  //       std::cout << ')';
  //       break;
      
  //     case StatementType::RETURN :
  //       std::cout << "RETURN(";
  //       u.expression->print();
  //       std::cout << ')';
  //       break;
  //   }
  // }

  // void DataType::print() {
  //   switch (type) {
  //     case DataTypeT::BYTE_TYPE :
  //       std::cout << "BYTE_TYPE(" << u.byteType << ')';
  //       break;
  //   }
  // }

  // void NamedDataType::print() {
  //   std::cout << "NAMED_DATATYPE(" << name << ", ";
  //   dataType.print();
  //   std::cout << ")";
  // }

  // void Function::print() {
  //   std::cout << "FUNC_";
  //   if (hasDefinition)
  //     std::cout << "DEF(";
  //   else
  //     std::cout << "DECL(";

  //   std::cout << funcDec.name << ')';
  // }

  // void GlobalStatement::print() {
  //   switch (type) {
  //     case GlobalStatementType::FUNCTION_DECL :
  //     case GlobalStatementType::FUNCTION_DEF :
  //       u.func->print();
  //       break;
  //   }
  // }

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
