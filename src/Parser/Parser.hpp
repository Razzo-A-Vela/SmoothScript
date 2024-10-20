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
    FunctionDefinition* processFunc(Token token);
    DataType* processDataType(Token token);
    Statement* processStatement(Token token);
    Expression* processExpression(Token token);
  };
}
