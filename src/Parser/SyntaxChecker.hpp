#pragma once
#include <ostream>
#include <vector>
#include <sstream>

#include <util/Processor.hpp>
#include <Tokenizer/Token.hpp>
#include <util/ErrorUtils.hpp>

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
    int scopeDepth = 0;


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
    bool wakeup(TokenType tokenType, bool consume);
    
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


    //* Rules: If there is a wakeup ALWAYS CALL IT BEFORE THE PROCESS, if there is no wakeup then the process is required
    //*        If a wakeup function is required for the syntax, call the wakeupError function in case the wakeup failed
    
    //* Using "_" because you cannot use namespaces inside classes in C++

    bool variables_wakeup();
    Utils::Error variables_wakeupError();
    Result::inst<Variables> variables_process();

    bool function_wakeup();
    Utils::Error function_wakeupError();
    Result::inst<Function> function_process();
    bool scope_wakeup();
    Utils::Error scope_wakeupError();
    Result::inst<Scope> scope_process();

    Result::inst<Statement> statement_process();
    Result::inst<Statement> forCompatibleStatement_process();
    Result::inst<StatementAndExpr> exprAndStatement_process();  //? first expression then statement
    bool doWhile_wakeup();
    Utils::Error doWhile_wakeupError();
    Result::inst<DoWhile> doWhile_process();
    bool for_wakeup();
    Utils::Error for_wakeupError();
    Result::inst<For> for_process();

    bool using_wakeup();
    Utils::Error using_wakeupError();
    Result::inst<Using> using_process();
    bool typeDef_wakeup();
    Utils::Error typeDef_wakeupError();
    Result::inst<TypeDef> typeDef_process();

    Result::inst<Type> type_process();
    Result::inst<Type> baseType_process();
    Result::inst<ReturnType> returnType_process();
    
    Result::inst<Identifier> identifier_process();
    Result::inst<Identifier> rawIdentifier_process();

    Result::inst<InitIdentifier> initIdentifier_process();
    Result::inst<InitExpression> initExpression_process();
    
    Result::inst<Expression> expression_process(Utils::Error identifierError);  //* SPECIAL CASE: Passing in the error in case the expression is not found
    Result::inst<Expression> baseExpression_process(Utils::Error identifierError);

    index_t operator_wakeup_index();             //* SPECIAL CASE: For optimization we return the index directly (if found)
    Operator* operatorFromIndex(index_t index);  //* There is no process, we use the index to get the operator directly
    bool literalExpression_wakeup();
    Utils::Error literalExpression_wakeupError();
    Result::inst<Expression> literalExpression_process();


    template <typename T>
    Result::inst<T> expectSemiOnResult(Result::inst<T> result) {
      if (result.isError() || wakeup(TokenType::SEMI, true))
        return result;
      return Result::error<T>(expectedSemiError());
    }

  private:
    std::vector<Token>* tokens;
  };
}
