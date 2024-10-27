#pragma once
#include <iostream>

#include <util/ErrorUtils.hpp>
#include <Processor/Processor.hpp>
#include <Tokenizer/Token.hpp>
#include "ParseNodes.hpp"

namespace Parser {
  using Tokenizer::Token;
  using Tokenizer::TokenType;

  class Parser : public Processor<Token, GlobalStatement> {
  private:
    std::vector<Token> tokens;
    
  public:
    Parser(std::vector<Token> tokens) : tokens(tokens), Processor(tokens.size()) {}
    void process();
    void print();
    
  protected:
    virtual Token get(int index) { return tokens.at(index); }
    Function* processFunc(Token token);
    DataType* processDataType(Token token, bool throwError = true);
    Statement* processStatement(Token token);
    Expression* processExpression(Token token);
    void processGlobalParameters(Token token);
    FunctionParameters processOneFunctionParameter(FunctionParameters functionParameters, std::string parameterType, int errLine);
    bool isFunctionParameter(std::string parameterType);
    Operator processFunctionOperator(int errLine);
    std::string processFunctionExtern(int errLine);
  };
}
