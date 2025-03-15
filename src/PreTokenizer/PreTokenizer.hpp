#pragma once
#include <ostream>
#include <sstream>

#include <util/Processor.hpp>
#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>

#include "PreToken.hpp"


namespace PreTokenizer {
  class PreTokenizer : public Utils::Processor<char, PreToken> {
  public:
    PreTokenizer(std::string file) : Processor(file.size()), file(file) {}
    virtual void process();
    virtual void print(std::ostream& out);

  protected:
    virtual char get(int index) { return file.at(index); }
    bool shouldIgnore(char c);

  private:
    std::string file;
  };
}
