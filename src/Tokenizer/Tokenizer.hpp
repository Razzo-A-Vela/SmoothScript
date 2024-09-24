#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <stdlib.h>

#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <Processor/Processor.hpp>
#include <PreTokenizer/PreTokenizer.hpp>
#include "Token.hpp"

namespace Tokenizer {
  using PreTokenizer::PreToken;
  using PreTokenizer::PreTokenType;

  class Tokenizer : public Processor<PreToken, Token> {
  private:
    const std::function<bool(PreToken, PreToken)> typeEqual = [](PreToken preToken, PreToken toEqual) { return preToken.type == toEqual.type; };
    const std::function<bool(PreToken, PreToken)> typeCharEqual = [](PreToken preToken, PreToken toEqual) { return preToken.type == toEqual.type && preToken.u.character == toEqual.u.character; };
    std::vector<PreToken> preTokens;
    
  public:
    Tokenizer(std::vector<PreToken> preTokens) : preTokens(preTokens), Processor(preTokens.size()) {}
    virtual void process();
    virtual void print();

  protected:
    virtual PreToken get(int index) { return preTokens.at(index); }
    void processSymbol(PreToken preToken);
    Literal parseLiteral(std::string str, int line);
    Literal parseNumLiteral(std::string str, int line);
  };
}
