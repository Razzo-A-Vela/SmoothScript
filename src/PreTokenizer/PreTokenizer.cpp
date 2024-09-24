#include "PreTokenizer.hpp"

namespace PreTokenizer {
  // commentState: 0 = not inside comment; 1 = inside single line comment; 2 = inside multi-line comment
  bool PreTokenizer::isInComment(char c) {
    if (commentState == 0 && c == '/' && peekEqual('/')) {
      commentState = 1;
      return true;

    } else if (commentState == 1 && c == '\n') {
      commentState = 0;
      return true;

    } else if (commentState == 0 && c == '/' && peekEqual('*')) {
      consume();
      commentState = 2;
      return true;

    } else if (commentState == 2 && c == '*' && peekEqual('/')) {
      consume();
      commentState = 0;
      return true;
    }

    return commentState;
  }

  char* PreTokenizer::parsePreProcessor() {
    bool isInBrace = false;
    const int preLine = line;
    std::stringstream stream;
    while (peekNotEqual('\n') || isInBrace) {
      if (!hasPeek())
        Utils::error("Syntax Error", "PreProcessor Braces not closed", preLine);
      char c = consume().value();
      if (isInComment(c))
        continue;

      if (c == '\\' && peekEqual('\n')) {
        consume();
        continue;
      } else if (c == '\\' && peekEqual('{')) {
        consume();
        stream << '{';
        isInBrace = true;
        continue;
      } else if (c == '\\' && peekEqual('}')) {
        consume();
        stream << '}';
        isInBrace = false;
        continue;
      }

      if (c != '\n')
        stream << c;
    }
    return Utils::stringToCString(stream.str());
  }

  void PreTokenizer::process() {
    while (hasPeek()) {
      char c = consume().value();
      if (isInComment(c) || shouldIgnore(c))
        continue;
      PreToken preToken;
      preToken.line = line;

      if (!isalnum(c) && c != '_' && c != '\"' && c != '\'') {
        if (c == '#') {
          preToken.type = PreTokenType::preProcessor;
          preToken.u.string = parsePreProcessor();
          addToOutput(preToken);
          continue;
        }

        preToken.type = PreTokenType::symbol;
        preToken.u.character = c;
        addToOutput(preToken);
        continue;
      }

      const int preLine = line;
      const bool isNumLiteral = isdigit(c);
      const bool isLiteral = c == '\"' || c == '\'';
      const char literalQuote = c == '\"' ? '\"' : '\'';
      std::stringstream stream;
      stream << c;

      if (isLiteral) {
        char c2 = 0;
        while (true) {
          if (!hasPeek())
            Utils::error("Syntax Error", "Invalid literal", preLine);
          c2 = consume().value();
          if (c2 == literalQuote)
            break;

          if (c2 == '\\' && hasPeek()) {
            stream << "\\" << consume().value();
            continue;

          } else if (c2 == '\\' && peekEqual('\n')) {
            consume();
            continue;

          } else if (c2 == '\n')
            Utils::error("Syntax Error", "Invalid literal", preLine);

          stream << c2;
        }

      } else if (isNumLiteral) {
        while (hasPeek() && (isalnum(peekValue()) || peekValue() == '_' || peekValue() == '.'))
          stream << consume().value();

      } else {
        while (hasPeek() && (isalnum(peekValue()) || peekValue() == '_'))
          stream << consume().value();
      }
      
      preToken.type = isLiteral ? PreTokenType::literal : (isNumLiteral ? PreTokenType::numLiteral : PreTokenType::identifier);
      preToken.u.string = Utils::stringToCString(stream.str());;
      addToOutput(preToken);
    }
  }

  void PreTokenizer::print() {
    std::vector<PreToken> output = getOutput();
    for (PreToken preToken : output) {
      preToken.print();
      std::cout << '\n';
    }
  }

  std::optional<char> PreTokenizer::consume() {
    std::optional<char> ret = Processor::consume();
    if (ret.has_value() && ret.value() == '\n')
      line++;
    return ret;
  }
}
