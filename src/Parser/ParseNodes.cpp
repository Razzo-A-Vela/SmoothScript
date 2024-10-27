#include "ParseNodes.hpp"

namespace Parser {
  void DataType::print() {
    if (isMutable)
      std::cout << "MUTABLE_";

    switch (type) {
      case DataTypeT::BYTE_TYPE :
        std::cout << "BYTE_TYPE(" << u.byteType << ')';
        break;
      
      case DataTypeT::POINTER :
        std::cout << "POINTER(";
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

  void Operator::print() {
    switch (type) {
      case OperatorType::BINARY :
        std::cout << "BINARY";
        break;
      
      case OperatorType::BEFORE :
        std::cout << "BEFORE";
        break;
      
      case OperatorType::AFTER :
        std::cout << "AFTER";
        break;
    }

    std::cout << ": (PRECEDENCE: " << precedence << ", SYMBOL: " << symbol1.u.character;
    if (symbol2 != NULL)
      std::cout << symbol2->u.character;
    std::cout << ')';
  }

  void FunctionParameters::print() {
    switch (type) {
      case FunctionType::DEFAULT :
        std::cout << "DEFAULT";
        break;
      
      case FunctionType::INLINE :
        std::cout << "INLINE";
        break;
      
      case FunctionType::ENTRY :
        std::cout << "ENTRY";
        break;
      
      case FunctionType::EXTERN :
        std::cout << "EXTERN";
        break;
    }

    std::cout << ": (NO_RETURN: " << noReturn << ", CAST: " << cast << ", OP: " << op;
    if (op) {
      std::cout << " {";
      oper.print();
      std::cout << '}';
    }
    std::cout << ')';
  }

  void Function::print() {
    std::cout << '(';
    funcParams.print();

    if (hasDefinition)
      std::cout << ")FUNC_DEF(";
    else
      std::cout << ")FUNC_DEC(";
    
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
