#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <util/StringUtils.hpp>
#include "Processor/Processor.hpp"
#include "PreToken.hpp"

namespace PreTokenizer {
  class PreTokenizer : public Processor<char, PreToken> {
  private:
    const std::function<bool(char)> shouldIgnore = [](char c) { return c == ' ' || c == '\n'; };
    std::string file;
    int line = 1;
    int commentState = 0;

  public:
    PreTokenizer(std::string file) : file(file), Processor(file.size()) {}
    virtual void process();
    virtual void print();
  
  protected:
    virtual char get(int index) { return file.at(index); }
    std::optional<char> consume();
    char* parsePreProcessor();
    bool isInComment(char c);
  };
}
