#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <functional>

#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <util/FileUtils.hpp>
#include <Processor/Processor.hpp>

#include "TOMLContent.hpp"

namespace TOML {
  enum class TokenType {
    IDENTIFIER, EQUALS, TRUE, FALSE, STRING, NEW_LINE, DOT, OPEN_SQUARE, CLOSED_SQUARE
  };
  
  struct Token {
    TokenType type;
    union {
      const char* string;
    } u;
    int line;

    void print(std::ostream& out);

    static const std::function<bool(Token, Token)> typeEqual;
  };


  class Tokenizer : public Processor<char, Token> {
  private:
    std::string fileContent;
    int line = 1;

  public:
    Tokenizer(std::string fileContent) : fileContent(fileContent), Processor(fileContent.size()) {}
    virtual void process();
    virtual void print(std::ostream& out);

  protected:
    virtual char get(int index) { return fileContent.at(index); }
    std::optional<char> consume();
  };


  class Parser : public Processor<Token, Table*> {
  private:
    std::vector<Token> tokens;
    TOML::Table* root;
    TOML::Table* currTable;
    TOML::Table* toModifyTable;

    static Parser* read(std::string fileContent);
  
  public:
    Parser(std::vector<Token> tokens) : tokens(tokens), Processor(tokens.size()) {}
    virtual void process();
    virtual void print(std::ostream& out);
    
    static Parser* readOrError(std::string fileName);
    static Parser* readOrCreate(std::string fileName, std::string defaultFileString);

  protected:
    virtual Token get(int index) { return tokens.at(index); }
    std::string processKey();
    Content* processValue();
    Table* addTableTo(Table* toAdd, std::string tableName);
    Table* getTable(Table* table, std::string name);

    int getErrLine() {
      return hasPeek() ? peekValue().line : peekValue(-1).line;
    }
  };
}
