#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <util/StringUtils.hpp>
#include <util/ErrorUtils.hpp>
#include <Processor/Processor.hpp>
#include "PreToken.hpp"

namespace PreTokenizer {
  class PreTokenizer : public Processor<char, PreToken> {
  private:
    const std::function<bool(char)> shouldIgnore = [](char c) { return c == ' ' || c == '\n'; };
    std::string file;
    int multiLineCommentDepth = 0;
    int commentState = 0;
    int line = 1;

  public:
    PreTokenizer(std::string file) : file(file), Processor(file.size()) {}
    virtual void process();
    virtual void print(std::ostream& out);
  
  protected:
    virtual char get(int index) { return file.at(index); }
    std::optional<char> consume();
    bool isInComment(char c);
  };
}
