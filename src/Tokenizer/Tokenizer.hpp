#pragma once
#include <ostream>
#include <sstream>

#include <util/Processor.hpp>
#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <PreTokenizer/PreToken.hpp>

#include "Token.hpp"

namespace Tokenizer {
  using namespace PreTokenizer;

  class Tokenizer : public Utils::Processor<PreToken, Token> {
  public:
    Tokenizer(std::vector<PreToken> preTokens) : preTokens(preTokens), Processor(preTokens.size()) {};
    virtual void process();
    virtual void print(std::ostream& out);

  protected:
    virtual PreToken get(int index) { return preTokens.at(index); }
    Token* processToken(PreToken preToken);
    Token* processLiteral(PreToken preToken);

  private:
    std::vector<PreToken> preTokens;
  };
}
