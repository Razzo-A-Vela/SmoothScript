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
        if (u.expression != NULL) {
          std::cout << "RETURN(";
          u.expression->print();
          std::cout << ')';
          
        } else
          std::cout << "RETURN";
        break;
    }
  }

  void Function::print() {
    if (isDeclaration)
      std::cout << "FUNC_DEC(";
    else
      std::cout << "FUNC_DEF(";
    
    std::cout << id << ", ";
    returnType->print();
    std::cout << ")(\n";

    for (DataType* param : params.getValues()) {
      param->print();
      std::cout << '\n';
    }
    std::cout << "){\n";

    for (Statement* statement : statements) {
      statement->print();
      std::cout << '\n';
    }
    std::cout << '}';
  }
  
  void GlobalStatement::print() {
    switch (type) {
      case GlobalStatementType::FUNCTION :
        u.funcDef->print();
        break;
    }
  }
}
