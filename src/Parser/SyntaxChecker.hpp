#pragma once
#include <ostream>

#include <util/Processor.hpp>
#include <tokenizer/Token.hpp>
#include <util/ErrorUtils.hpp>

#include "ParseNodes.hpp"
#include "ParserUtils.hpp"

namespace Parser {
  using namespace Tokenizer;

  class SyntaxChecker : public Utils::Processor<Token, GlobalNode> {
  public:
    SyntaxChecker(std::vector<Token> tokens) : tokens(tokens), Processor(tokens.size()) {}
    virtual void process();
    virtual void print(std::ostream& out);

  protected:
    virtual Token get(int index) { return tokens.at(index); }
    Result::inst<Type> processType();
    Result::inst<ReturnType> processReturnType();
    Result::inst<Variable> processVariable();
    Result::inst<Function> processFunction();
    Result::inst<Scope> processScope();
    Result::inst<Statement> _processStatementVariable();
    Result::inst<Statement> _processStatementScope();
    Result::inst<Statement> _processStatementIf();
    Result::inst<Statement> _processStatementExpression();
    Result::inst<Statement> _processStatementReturn();
    Result::inst<Statement> _processStatement();
    Result::inst<Expression> _processExpressionAssign();
    Result::inst<Expression> _processExpressionLiteral();
    Result::inst<Expression> _processExpression();
    
    int getLine();
    [[noreturn]] void syntaxError(std::string msg, int line = -1);
    bool trueConsume(bool check, int times = 1);
    bool falseConsume(bool check, int times = 1);
    void pushConsumes();
    void popConsumes(bool ok);
    int pushParents(std::vector<Token>* savedTokens, std::vector<Token>* parenTokens);
    void popParents(std::vector<Token>* savedTokens, int savedIndex);
    ReturnType* literalToReturnType(Literal literal);
  
    class PushConsumes {
    public:
      PushConsumes(SyntaxChecker* instance, bool isOk = NotOk) : instance(instance), isOk(isOk) { instance->pushConsumes(); }
      ~PushConsumes() { instance->popConsumes(isOk); }
      void notOk() { isOk = NotOk; }
      void ok() { isOk = Ok; }

      static constexpr bool NotOk = false;
      static constexpr bool Ok = true;
    
    private:
      SyntaxChecker* instance;
      bool isOk;
    };

  private:
    std::vector<Token> tokens;
    IntStack pushindex;
    int scopeDepth = 0;
  };
}
