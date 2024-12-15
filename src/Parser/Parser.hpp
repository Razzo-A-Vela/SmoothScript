#pragma once
#include <iostream>
#include <functional>

#include <util/ErrorUtils.hpp>
#include <Processor/Processor.hpp>
#include <Tokenizer/Token.hpp>
#include "ParseNodes.hpp"

/*
  TODO: Use a Token buffer and then when you find a symbol (ex. '?' for variables and parameters) analyze the buffer and what comes next
*/

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
    Function* processFunc();
    DataType* processDataType();
    Statement* processStatement();
    Expression* processExpression();
    int getErrLine();
  };
}
