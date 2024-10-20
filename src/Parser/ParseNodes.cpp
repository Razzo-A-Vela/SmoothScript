#include "ParseNodes.hpp"

namespace Parser {
  void DataType::print() {
    switch (type) {
      case DataTypeT::BYTE_TYPE :
        std::cout << "BYTE_TYPE(" << u.byteType << ')';
        break;
      
      case DataTypeT::MUTABLE_BYTE_TYPE :
        std::cout << "MUTABLE_BYTE_TYPE(" << u.byteType << ')';
        break;
      
      case DataTypeT::POINTER :
        std::cout << "POINTER(";
        u.dataType->print();
        std::cout << ')';
        break;
      
      case DataTypeT::MUTABLE_POINTER :
        std::cout << "MUTABLE_POINTER(";
        u.dataType->print();
        std::cout << ')';
        break;
      
      case DataTypeT::VOID :
        std::cout << "VOID";
        break;
    }
  }

  void Expression::print() {
    switch (type) {
      case ExpressionType::LITERAL :
        std::cout << "LITERAL(";
        u.literal->print();
        std::cout << ')';
        break;
    }
  }

  void Statement::print() {
    switch (type) {
      case StatementType::RETURN :
        std::cout << "RETURN(";
        u.expression->print();
        std::cout << ')';
        break;
    }
  }

  void FunctionDefinition::print() {
    std::cout << "FUNC_DEF(" << id << ", ";
    returnType->print();
    std::cout << "){\n";
    
    for (Statement* statement : statements) {
      statement->print();
      std::cout << '\n';
    }
    std::cout << '}';
  }
  
  void GlobalStatement::print() {
    switch (type) {
      case GlobalStatementType::FUNC_DEF :
        u.funcDef->print();
        break;
    }
  }
}
