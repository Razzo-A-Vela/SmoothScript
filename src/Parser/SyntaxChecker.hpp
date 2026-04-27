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

  #define ignores
  #define alwaysErrors
  #define childOf(parent)
  #define withWakeup(wakeupToken)

  class SyntaxChecker : public Utils::Processor<Token, GlobalNode> {
  protected:
    int scopeDepth = 0;

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
    Result::inst<Variable> alwaysErrors withWakeup(TokenType::COLON) processVariable();   // TYPE NAME [= INIT_EXPRESSION];
    Result::inst<Function> alwaysErrors withWakeup(TokenType::FUNC) processFunction();    // NAME() :RETURN_TYPE SCOPE
    Result::inst<ReturnType> ignores processReturnType();                                 // void | TYPE
    Result::inst<Scope> ignores processScope();                                           // { STATEMENT... }
    Result::inst<Statement> ignores processStatement();                                   // ...
    Result::inst<Identifier> ignores processIdentifier();                                 // RAW_IDENTIFIER
    Result::inst<Type> ignores processType();                                             // ...
    Result::inst<InitExpression> alwaysErrors processInitExpression();                    // INIT_SPECIFIC_EXPRESSION | EXPRESSION
    Result::inst<Expression> ignores processExpression();                                 // ...
    Result::inst<Expression> alwaysErrors childOf(processExpression()) processLiteralExpression();

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
