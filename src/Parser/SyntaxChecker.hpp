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

    static Context fromTokens(std::vector<Token>* tokens);
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
    Context switchContextTo(TokenType type, Utils::Error err);
    Context switchContextToParents();
    Context switchContextToBrackets();
    int getErrorLine();
    //? A wakeup token is a disposable token that is used to indicate the start of a specific syntax
    bool wakeup(Token token, TokenType tokenType);
    bool wakeup(TokenType tokenType);
    bool semi();
    bool semi(Token token);
    Utils::Error semiError();
    Utils::Error parentsError();
    Result::inst<Variables> alwaysErrors withWakeup(TokenType::COLON) processVariables(); // VARIABLE [(, INIT_IDENTIFIER)...]
    Result::inst<Variable> alwaysErrors processVariable();                                // TYPE INIT_IDENTIFIER
    Result::inst<Function> alwaysErrors withWakeup(TokenType::FUNC) processFunction();    // NAME() RETURN_TYPE SCOPE
    Result::inst<Using> alwaysErrors withWakeup(TokenType::USING) processUsing();         // :TYPE BASE_TYPE [(, BASE_TYPE)...]
    Result::inst<TypeDef> alwaysErrors childOf(processUsing) withWakeup(TokenType::COLON) processTypeDef();
    Result::inst<ReturnType> ignores processReturnType();                                 // void | ! | TYPE
    Result::inst<Scope> ignores processScope();                                           // { (STATEMENT;)... }
    Result::inst<Statement> ignores processStatement();                                   // ...
    Result::inst<Statement> ignores childOf(processStatement) processForCompatibleStatement();
    Result::inst<For> alwaysErrors withWakeup(TokenType::FOR) processFor();               // \(FOR_COMPATIBLE_STATEMENT [EXPRESSION]; [EXPRESSION]\) STATEMENT
    Result::inst<DoWhile> alwaysErrors withWakeup(TokenType::DO) processDoWhile();        // STATEMENT while EXPR_AND_STATEMENT
    Result::inst<Identifier> ignores processRawIdentifier();
    Result::inst<Identifier> ignores processIdentifier();                                 // RAW_IDENTIFIER
    Result::inst<Type> ignores processType();                                             // [unsigned | signed] BASE_TYPE
    Result::inst<Type> ignores childOf(processType) processBaseType();                    // int | float | bool | cstr | char | size_t | IDENTIFIER
    Result::inst<InitExpression> alwaysErrors processInitExpression();                    // INIT_SPECIFIC_EXPRESSION | EXPRESSION
    Result::inst<Expression> ignores processExpression();                                 // BASE_EXPRESSION [(BINARY_OPERATOR BASE_EXPRESSION)...]
    Result::inst<Expression> ignores processBaseExpression();                             // ...
    Result::inst<Expression> alwaysErrors childOf(processBaseExpression) processLiteralExpression();
    Result::inst<Operator> ignores processOperator();
    Result::inst<InitIdentifier> alwaysErrors processInitIdentifier();                    // [mut | const] NAME [= INIT_EXPRESSION]
    Result::inst<StatementAndExpr> ignores processExprAndStatement();                     // \(EXPRESSION\) STATEMENT

    template <typename T>
    Result::inst<T> expectSemiOnResult(Result::inst<T> other) {
      if (other.hasValue() && !semi())
        return Result::error<T>(semiError());
      return other;
    }

  private:
    std::vector<Token>* tokens;
  };
}
