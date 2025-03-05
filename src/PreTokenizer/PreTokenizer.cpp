#include "PreTokenizer.hpp"

namespace PreTokenizer {
  bool PreTokenizer::shouldIgnore(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  void PreTokenizer::process() {
    int line = 1;
    int comment = 0;
    
    while (hasPeek()) {
      char c = consume().value();
      if (c == '\n')
        line++;
      

      if (comment == 1) {
        if (c == '\n')
          comment = 0;
        continue;
      } else if (comment == 2) {
        if (c == '*' && peekEqual('/')) {
          comment = 0;
          consume();
        }
        continue;
      }

      if (c == '/') {
        if (peekEqual('/')) {
          comment = 1;
          continue;
          
        } else if (peekEqual('*')) {
          comment = 2;
          continue;
        }
      }
      
      if (shouldIgnore(c))
        continue;

      
      if (isalpha(c)) {
        std::stringstream stream;
        stream << c;
        while (hasPeek() && (isalnum(peekValue()) || peekValue() == '_'))
          stream << consume().value();
      
        addToOutput({ PreTokenType::IDENTIFIER, { .string = Utils::stringToCString(stream.str()) }, line });

      } else if (isdigit(c)) {
        std::stringstream stream;
        stream << c;
        while (hasPeek() && (isdigit(peekValue()) || peekValue() == '_')) {
          if (peekValue() == '_') {
            consume();
            continue;
          }

          stream << consume().value();
        }
        
        addToOutput({ PreTokenType::NUMBER, { .string = Utils::stringToCString(stream.str()) }, line });

      } else
        addToOutput({ PreTokenType::SYMBOL, { .character = c }, line });
    }
  }

  void PreTokenizer::print(std::ostream& out) {
    std::vector<PreToken> output = getOutput();
    for (PreToken preToken : output) {
      preToken.print(out);
      out << '\n';
    }
  }
}
