#include <ostream>
#include <vector>

#include <util/Processor.hpp>
#include <Tokenizer/Token.hpp>
#include <util/ErrorUtils.hpp>

#include "ParseNodes.hpp"
#include "ParserUtils.hpp"

namespace Parser {
  using Tokenizer::Token;
  using Tokenizer::TokenType;

  class SyntaxChecker : public Utils::Processor<Token, GlobalNode> {
  public:
    SyntaxChecker(std::vector<Token> tokens) : Processor(tokens.size()), tokens(tokens) {}
    virtual void process();
    virtual void print(std::ostream& out);
    [[nodiscard]] Utils::Error syntaxError(const char* msg);
    int getErrorLine();
    //? A wakeup token is a disposable token that is used to indicate the start of a specific syntax
    bool wakeup(Token token, TokenType tokenType);
    bool wakeup(TokenType tokenType);
    Result::inst<Variable> processVariable();
    bool identifierPeek();
    Result::inst<Identifier> processIdentifier();
    Result::inst<Type> processType();
    Result::inst<InitExpression> processInitExpression();
    Result::inst<Expression> processExpression();
    Result::inst<Expression> processLiteralExpression();
    
    template <typename T>
    Result::inst<T> expectSemi(Result::inst<T> other) {
      if (!wakeup(TokenType::SEMI))
        return Result::error<T>(syntaxError("Expected ';'"));
      return other;
    }

  protected:
    virtual Token get(int index) { return tokens.at(index); }

  private:
    std::vector<Token> tokens;
  };
}
