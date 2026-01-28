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

  protected:
    virtual Token get(int index) { return tokens.at(index); }

  private:
    std::vector<Token> tokens;
  };
}
