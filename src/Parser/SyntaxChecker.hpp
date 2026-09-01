#pragma once
#include <ostream>
#include <vector>
#include <sstream>

#include <util/Processor.hpp>
#include <util/ErrorUtils.hpp>
#include <Tokenizer/Token.hpp>

#include "ParseNodes.hpp"
#include "ParserUtils.hpp"

namespace Parser {
  using Tokenizer::Token;
  using Tokenizer::TokenType;

  #define ignores
  #define alwaysErrors
  #define childOf(parent)
  #define withWakeup(wakeupToken)

  class SyntaxChecker : public Utils::Processor<Token, GlobalNode> {
  protected:
    struct Context {
      std::vector<Token>* tokens;
      int index;

      static Context fromTokens(std::vector<Token>* tokens);
    };

    class ContextSwitcher {
      SyntaxChecker* checker;
      Context previous;
      bool isDone;

    public:
      ContextSwitcher(SyntaxChecker* checker, Context previous) : checker(checker), previous(previous), isDone(false) {}

      ~ContextSwitcher() {
        switchContextToPrevious();
      }

      void switchContextToPrevious() {
        if (isDone)
          return;
        
        checker->switchContextRaw(previous);
        isDone = true;
      }
    };

    int scopeDepth = 0;

  public:
    SyntaxChecker(std::vector<Token> tokens) : Processor(tokens.size()), tokens(new std::vector<Token>(tokens)) {}
    virtual void process();
    virtual void print(std::ostream& out);

  protected:
    virtual Token get(int index) { return tokens->at(index); }
    Context switchContextRaw(Context newContext);
    ContextSwitcher switchContext(Context newContext);
    ContextSwitcher switchContextTo(TokenType type, Utils::Error err);
    ContextSwitcher switchContextToParents();
    ContextSwitcher switchContextToBrackets();
    int getErrorLine();
    
    [[nodiscard]] Utils::Error syntaxError(const char* msg);
    [[nodiscard]] Utils::Error expectedError(const char* expected);
    [[nodiscard]] Utils::Error expectedOpenParentError();
    [[nodiscard]] Utils::Error expectedClosedParentError();
    [[nodiscard]] Utils::Error expectedOpenBracketError();
    [[nodiscard]] Utils::Error expectedClosedBracketError();
    [[nodiscard]] Utils::Error expectedSemiError();
    [[nodiscard]] Utils::Error expectedColonError();
    [[nodiscard]] Utils::Error expectedCommaError();
    [[nodiscard]] Utils::Error expectedExpressionError();


    /*
    * Rules:  If there is a peek and the syntax is NOT REQUIRED CALL THE PEEK FIRST AND THEN THE PROCESS PASSING FALSE.
    *         If there is a peek and the syntax is REQUIRED CALL THE PROCESS PASSING TRUE.
    *         If there is no peek then the syntax is ALWAYS REQUIRED.
    *
    *         Note: The peek function only uses peeks, so in the process you MUST consume the tokens in the peek
    *         Note: Using "_" because you cannot use namespaces inside classes in C++
    */

    bool variables_peek(int offset = 0);
    Result::inst<Variables> variables_process(bool required);

    bool function_peek(int offset = 0);
    Result::inst<Function> function_process(bool required);
    bool scope_peek(int offset = 0);
    Result::inst<Scope> scope_process(bool required);

    Result::inst<Statement> statement_process();
    Result::inst<Statement> forCompatibleStatement_process();
    Result::inst<StatementAndExpr> exprAndStatement_process();  //? first expression then statement
    bool doWhile_peek(int offset = 0);
    Result::inst<DoWhile> doWhile_process(bool required);
    bool for_peek(int offset = 0);
    Result::inst<For> for_process(bool required);

    bool using_peek(int offset = 0);
    Result::inst<Using> using_process(bool required);
    bool typeDef_peek(int offset = 0);
    Result::inst<TypeDef> typeDef_process(bool required);

    Result::inst<Type> type_process();
    Result::inst<Type> baseType_process();
    Result::inst<ReturnType> returnType_process();
    
    Result::inst<Identifier> identifier_process();
    Result::inst<Identifier> rawIdentifier_process();

    Result::inst<InitIdentifier> initIdentifier_process();
    Result::inst<InitExpression> initExpression_process();
    
    Result::inst<Expression> expression_process(Utils::Error identifierError);  //* SPECIAL CASE: Passing in the error in case the expression is not found
    Result::inst<Expression> baseExpression_process(Utils::Error identifierError);

    index_t operator_peek_index();             //* SPECIAL CASE: For optimization we return the index directly (if found)
    Operator* operatorFromIndex(index_t index);  //* There is no process, we use the index to get the operator directly
    bool literalExpression_peek(int offset = 0);
    Result::inst<Expression> literalExpression_process(bool required);


    template <typename T>
    Result::inst<T> expectSemiOnResult(Result::inst<T> result) {
      if (result.isError() || tryConsume({ TokenType::SEMI }))
        return result;
      return Result::error<T>(expectedSemiError());
    }

  private:
    std::vector<Token>* tokens;
  };
}
