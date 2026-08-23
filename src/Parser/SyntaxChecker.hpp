#pragma once
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
    [[nodiscard]] Utils::Error syntaxError(const char* msg);

  protected:
    virtual Token get(int index) { return tokens->at(index); }
    Context switchContextRaw(Context newContext);
    ContextSwitcher switchContext(Context newContext);
    ContextSwitcher switchContextTo(TokenType type, Utils::Error err);
    ContextSwitcher switchContextToParents();
    ContextSwitcher switchContextToBrackets();
    int getErrorLine();
    bool wakeup(TokenType tokenType, bool consume);
    Utils::Error semiError();
    Utils::Error parentsError();


    //* Rules: If there is a wakeup ALWAYS CALL IT BEFORE THE PROCESS, if there is no wakeup then the process is required
    //* Using "_" because you cannot use namespaces inside classes in C++

    bool variables_wakeup();
    Result::inst<Variables> variables_process();

    bool function_wakeup();
    Result::inst<Function> function_process();
    bool scope_wakeup();
    Result::inst<Scope> scope_process();

    Result::inst<Statement> statement_process();
    Result::inst<Statement> forCompatibleStatement_process();
    Result::inst<StatementAndExpr> exprAndStatement_process();  //? first expression then statement
    Result::inst<DoWhile> doWhile_process();
    Result::inst<For> for_process();

    bool using_wakeup();
    Result::inst<Using> using_process();
    bool typeDef_wakeup();
    Result::inst<TypeDef> typeDef_process();

    Result::inst<Type> type_process();
    Result::inst<Type> baseType_process();
    Result::inst<ReturnType> returnType_process();
    
    Result::inst<Identifier> identifier_process();
    Result::inst<Identifier> rawIdentifier_process();

    Result::inst<InitIdentifier> initIdentifier_process();
    Result::inst<InitExpression> initExpression_process();
    
    Result::inst<Expression> expression_process();
    Result::inst<Expression> baseExpression_process();

    index_t operator_wakeup_index();             //* SPECIAL CASE: For optimization we return the index directly (if found)
    Operator* operatorFromIndex(index_t index);  //* There is no process, we use the index to get the operator directly
    bool literalExpression_wakeup();
    Result::inst<Expression> literalExpression_process();


    template <typename T>
    Result::inst<T> expectSemiOnResult(Result::inst<T> result) {
      if (result.isError() || wakeup(TokenType::SEMI, true))
        return result;
      return Result::error<T>(semiError());
    }

  private:
    std::vector<Token>* tokens;
  };
}
