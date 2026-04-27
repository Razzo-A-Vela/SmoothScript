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

  struct Context {
    std::vector<Token>* tokens;
    int index;

    static Context fromTokens(std::vector<Token>* tokens) {
      return { tokens, 0 };
    }
  };


  class SyntaxChecker : public Utils::Processor<Token, GlobalNode> {
  public:
    SyntaxChecker(std::vector<Token> tokens) : Processor(tokens.size()), tokens(new std::vector<Token>(tokens)) {}
    virtual void process();
    virtual void print(std::ostream& out);
    [[nodiscard]] Utils::Error syntaxError(const char* msg);

  protected:
    virtual Token get(int index) { return tokens->at(index); }
    Context switchContext(Context newContext);
    int getErrorLine();
    //? A wakeup token is a disposable token that is used to indicate the start of a specific syntax
    bool wakeup(Token token, TokenType tokenType);
    bool wakeup(TokenType tokenType);
    bool semi();
    bool semi(Token token);
    Result::inst<Variable> processVariable();
    Result::inst<Identifier> processIdentifier();
    Result::inst<Type> processType();
    Result::inst<InitExpression> processInitExpression();
    Result::inst<Expression> processExpression();
    Result::inst<Expression> processLiteralExpression();
    
    template <typename T>
    Result::inst<T> expectSemi(Result::inst<T> other) {
      if (other.hasValue() && !semi())
        return Result::error<T>(syntaxError("Expected ';'"));
      return other;
    }

  private:
    std::vector<Token>* tokens;
  };
}
