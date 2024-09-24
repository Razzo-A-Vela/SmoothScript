#pragma once
#include <iostream>
#include <string>
#include <sstream>

#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <Processor/Processor.hpp>

namespace Tokenizer {
  enum class LiteralType {
    integer, floating, byte, character, string, longint, doubled, shorted
  };

  struct Literal {
    LiteralType type;
    union {
      float floating;
      long double doubled;

      unsigned char byte;
      unsigned int integer;
      unsigned long long longint;
      unsigned short shorted;
      
      char character;
      char* string;
    } u;

    void print();
  };

  class LiteralParser : public Processor<char, Literal> {
  private:
    std::string toParse;
    const int line;
    const bool isNumLiteral;

  public:
    LiteralParser(std::string toParse, int line, bool isNumLiteral) : toParse(toParse), line(line), isNumLiteral(isNumLiteral), Processor(toParse.size()) {}
    void process();
    void print();

  protected:
    virtual char get(int index) { return toParse.at(index); }
    void processNumLiteral();
  };
}
