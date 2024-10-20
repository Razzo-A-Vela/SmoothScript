#include "PreProcessor.hpp"

namespace PreProcessor {
  void PreProcessor::namespaceWalkerPass(Token token) {
    if (Token::typeEqual(token, { TokenType::identifier }) &&
        peekEqual({ TokenType::double_colon }, Token::typeEqual) &&
        peekEqual({ TokenType::identifier }, Token::typeEqual, 1)) {
      consume();
      Token ident_2 = consume().value();
      std::stringstream stream;
      stream << std::string(token.u.string) << ':' << std::string(ident_2.u.string);
      token.u.string = Utils::stringToCString(stream.str());
    }

    if (peekEqual({ TokenType::double_colon }, Token::typeEqual))
      namespaceWalkerPass(token);
    else
      addToOutput(token);
  }


  std::string PreProcessor::namespaceReplace(Token token, std::string errString) {
    addToOutput(token);
    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected identifier after " + errString + " inside namespace", token.line);
    Token toAdd = consume().value();

    std::stringstream stream;
    stream << currentNamespacePrefix << std::string(toAdd.u.string);
    std::string streamStr = stream.str();

    toAdd.u.string = Utils::stringToCString(streamStr);
    addToOutput(toAdd);
    return streamStr;
  }

  std::string PreProcessor::getNamespacePrefix(int depth) {
    if (depth == 0)
      return currentNamespacePrefix;
    
    std::string temp = currentNamespacePrefix;
    for (int i = 0; i < depth; i++)
      removeLastNamespacePrefix();
    
    std::string ret = currentNamespacePrefix;
    currentNamespacePrefix = temp;
    return ret;
  }

  bool PreProcessor::hasNamespacePrefix(int depth) {
    if (depth == 0)
      return currentNamespacePrefix != "";
    
    bool ret = true;
    std::string temp = currentNamespacePrefix;
    for (int i = 0; i < depth; i++) {
      removeLastNamespacePrefix();
      
      if (currentNamespacePrefix == "") {
        ret = false;
        break;
      }
    }

    currentNamespacePrefix = temp;
    return ret;
  }

  bool PreProcessor::namespaceFind(Token token) {
    if (Token::typeEqual(token, { TokenType::identifier })) {
      std::string toFindIdentifier = std::string(token.u.string);

      int depth = 0;
      while (hasNamespacePrefix(depth)) {
        std::string identifier = getNamespacePrefix(depth) + toFindIdentifier;
        if (Utils::isInVector(namespaceIdentifiers, identifier)) {
          token.u.string = Utils::stringToCString(identifier);
          addToOutput(token);
          return true;
        }

        depth++;
      }
    }
    return false;
  }
  
  void PreProcessor::removeLastNamespacePrefix() {
    int index = -1;
    for (int i = currentNamespacePrefix.size() - 1 - 1; i >= 0; i--) {
      char c = currentNamespacePrefix.at(i);
      if (c == ':') {
        index = i;
        break;
      }
    }

    if (index == -1)
      currentNamespacePrefix = "";
    else
      currentNamespacePrefix = currentNamespacePrefix.substr(0, index + 1);
  }

  void PreProcessor::addToNamespacePrefix(std::string prefix) {
    currentNamespacePrefix += prefix + ':';
  }

  void PreProcessor::namespacePass(Token token) {
    if (!isPreProcessor(token, "namespace")) {
      addToOutput(token);
      return;
    }

    std::string namespaceIdentifier = getPreProcessorIdentifier(token.line);
    addToNamespacePrefix(namespaceIdentifier);
    while (notEndPreProcessor(token.line)) {
      Token currToken = consume().value();
      
      if (isPreProcessor(currToken, "namespace")) {
        namespacePass(currToken);
      
      } else if (Token::typeStringEqual(currToken, { TokenType::parserParameter, { .string = "type" } })) {
        std::string toAddString = namespaceReplace(currToken, "parserParameter");

        while (peekNotEqual({ TokenType::semi_colon }, Token::typeEqual)) {
          Token currCurrToken = consume().value();
          if (!namespaceFind(currCurrToken))
            addToOutput(currCurrToken);
        }
        addToOutput(consume().value());
        namespaceIdentifiers.push_back(toAddString);

      } else if (Token::typeEqual(currToken, { TokenType::preProcessor })) {
        std::string toAddString = namespaceReplace(currToken, "preProcessor");

        while (notEndPreProcessor(currToken.line, false)) {
          Token currCurrToken = consume().value();
          if (!namespaceFind(currCurrToken))
            addToOutput(currCurrToken);
        }
        addToOutput(consume().value());
        namespaceIdentifiers.push_back(toAddString);

      } else if (Token::typeEqual(currToken, { TokenType::double_question })) {
        std::string toAddString = namespaceReplace(currToken, "??");

        while (peekNotEqual({ TokenType::semi_colon }, Token::typeEqual)) {
          if (!hasPeek() || !notEndPreProcessor(currToken.line))
            Utils::error("Syntax Error", "Expected ';' after global variable declaration (type??identifier;)", currToken.line);

          Token currCurrToken = consume().value();
          if (!namespaceFind(currCurrToken))
            addToOutput(currCurrToken);
        }
        addToOutput(consume().value());
        namespaceIdentifiers.push_back(toAddString);
        
      } else if (Token::typeEqual(currToken, { TokenType::func })) {
        std::string toAddString = namespaceReplace(currToken, "func");

        while (peekNotEqual({ TokenType::open_brace }, Token::typeEqual) && peekNotEqual({ TokenType::semi_colon }, Token::typeEqual)) {
          if (!hasPeek() || !notEndPreProcessor(currToken.line))
            Utils::error("Syntax Error", "Expected '{' or ';' after function declaration (func)", currToken.line);

          Token currCurrToken = consume().value();
          if (!namespaceFind(currCurrToken))
            addToOutput(currCurrToken);
        }
        addToOutput(consume().value());
        namespaceIdentifiers.push_back(toAddString);
        
      } else if (!namespaceFind(currToken))
        addToOutput(currToken);
    }

    removeLastNamespacePrefix();
  }


  bool PreProcessor::defineFind(Token token, std::function<void(Token)> addFunction, std::string notInside) {
    if (Token::typeEqual(token, { TokenType::identifier })) {
      std::string identifier = std::string(token.u.string);
      if (identifier == notInside)
        Utils::error("Define Error", "Cannot put self define identifier inside define preProcessor", token.line);
      
      if (defines.hasKey(identifier)) {
        for (Token toAdd : *(defines.getValue(identifier).value()))
          addFunction(toAdd);
        
        return true;
      }
    }
    return false;
  }

  void PreProcessor::definePass(Token token) {
    if (!isPreProcessor(token, "define")) {
      if (!defineFind(token, [this](Token toAdd) { addToOutput(toAdd); }))
        addToOutput(token);
      return;
    }

    std::string defineIdentifier = getPreProcessorIdentifier(token.line);
    if (defines.hasKey(defineIdentifier))
      Utils::error("Define Error", defineIdentifier + " alredy defined", token.line);
    defines.add(defineIdentifier, new std::vector<Token>());

    while (notEndPreProcessor(token.line)) {
      Token currToken = consume().value();

      if (Token::typeEqual(currToken, { TokenType::preProcessor }))
        Utils::error("Define Error", "Cannot put preProcessors inside defines", token.line);

      if (!defineFind(currToken, [this](Token toAdd) { defines.last().value()->push_back(toAdd); }, defineIdentifier))
        defines.last().value()->push_back(currToken);
    }
  }


  bool PreProcessor::notEndPreProcessor(int errLine, bool autoConsume) {
    if (!hasPeek())
      Utils::error("Syntax Error", "Expected endPreProcessor(#) after preProcessor", errLine);

    bool ret = peekNotEqual({ TokenType::endPreProcessor }, Token::typeEqual);
    if (autoConsume && !ret)
      consume();
    return ret;
  }

  std::string PreProcessor::getPreProcessorIdentifier(int errLine) {
    if (peekNotEqual({ TokenType::identifier }, Token::typeEqual))
      Utils::error("Syntax Error", "Expected identifier after preProcessor", errLine);
    return std::string(consume().value().u.string);
  }
  
  bool PreProcessor::isPreProcessor(Token token, const char* preProcessor) {
    return Token::typeStringEqual(token, { TokenType::preProcessor, { .string = preProcessor } });
  }

  void PreProcessor::cycleOutput() {
    tokens = getOutput();
    setMaxIndex(tokens.size());
    resetOutput();
    resetIndex();
  }

  void PreProcessor::process() {
    while (true) {
      while (hasPeek()) {
        Token token = consume().value();

        if (currentPass == PreProcessPass::define) {
          definePass(token);

        } else if (currentPass == PreProcessPass::namespaces) {
          namespacePass(token);

        } else if (currentPass == PreProcessPass::namespaceWalker) {
          namespaceWalkerPass(token);

        } else
          addToOutput(token);
      }
      
      if (currentPass == PreProcessPass::namespaces)
        break;
      
      cycleOutput();
      currentPass = (PreProcessPass) ((int) currentPass + 1);
    }

    for (Token token : getOutput()) {
      if (Token::typeEqual(token, { TokenType::preProcessor }))
        Utils::error("Syntax Error", "Unrecognised preProcessor", token.line);
      
      if (Token::typeEqual(token, { TokenType::endPreProcessor }))
        Utils::error("Syntax Error", "Found endPreProcessor(#) without preProcessor", token.line);
    }
  }

  void PreProcessor::print() {
    std::vector<Token> output = getOutput();
    for (Token token : output) {
      token.print();
      std::cout << '\n';
    }
  }
}
