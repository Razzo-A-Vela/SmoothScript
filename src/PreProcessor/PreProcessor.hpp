#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <util/ErrorUtils.hpp>
#include <util/VectorUtils.hpp>
#include <util/Map.hpp>
#include <Processor/Processor.hpp>
#include <Tokenizer/Token.hpp>

namespace PreProcessor {
  using Tokenizer::Token;
  using Tokenizer::TokenType;

  enum class PreProcessPass {
    include, define, iff, namespaceWalker, namespaces, type, sizeoff, op, externer
  };

  class PreProcessor : public Processor<Token, Token> {
  private:
    std::vector<Token> tokens;
    PreProcessPass currentPass = (PreProcessPass) 0;

    Map<std::string, std::vector<Token>*> defines;

    std::string currentNamespacePrefix = "";
    std::vector<std::string> namespaceIdentifiers;

  public:
    const int RECURSION_LIMIT = 25;

    PreProcessor(std::vector<Token> tokens) : tokens(tokens), Processor(tokens.size()) {}
    void process();
    void print();
  
  protected:
    Token get(int index) { return tokens.at(index); }
    void cycleOutput();
    bool isPreProcessor(Token token, const char* preProcessor);
    std::string getPreProcessorIdentifier(int errLine);
    bool notEndPreProcessor(int errLine, bool autoConsume = true);

    void definePass(Token token);
    bool defineFind(Token token, std::function<void(Token)> addFunction, std::string notInside = "");

    void namespacePass(Token token);
    void addToNamespacePrefix(std::string prefix);
    void removeLastNamespacePrefix();
    bool namespaceFind(Token token);
    bool hasNamespacePrefix(int depth);
    std::string getNamespacePrefix(int depth);
    std::string namespaceReplace(Token token, std::string errString);
    
    void namespaceWalkerPass(Token token);
  };
}
