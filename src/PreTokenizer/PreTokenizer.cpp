#include "PreTokenizer.hpp"

namespace PreTokenizer {
  bool PreTokenizer::shouldIgnore(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  void PreTokenizer::process() {
    int line = 1;
    int comment = 0;
    bool lastIgnored = false;
    
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
      
      if (shouldIgnore(c)) {
        lastIgnored = true;
        continue;
      }
      
      if (lastIgnored) {
        addToOutput({ PreTokenType::SPACES, {}, line });
        lastIgnored = false;
      }

      
      if (isalpha(c) || c == '_') {
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

      } else if (c == '#') {
          if (!hasPeek())
            Utils::error("Syntax Error", "Expected preProcessor name after '#'", line);
          if (!isalpha(peekValue()) && peekValue() != '_')
            Utils::error("Syntax Error", "Invalid preProcessor name", line);

          std::stringstream stream;
          while (hasPeek() && (isalnum(peekValue()) || peekValue() == '_'))
            stream << consume().value();
          addToOutput({ PreTokenType::PRE_PROCESSOR, { .string = Utils::stringToCString(stream.str()) }, line });

      } else if (c == '\\' && peekEqual('#')) {
          consume();
          addToOutput({ PreTokenType::END_PRE_PROCESSOR, {}, line });

      } else if (c == '\"' || c == '\'') {
        std::stringstream stream;
        char quote = c;

        while (!tryConsume(quote)) {
          if (!hasPeek() || peekEqual('\n'))
            Utils::error("Syntax Error", std::string("Expected ") + quote + " after " + quote, line);
          
          if (peekEqual('\\') && peekEqual('\"', 1)) {
            consume();
            consume();
            stream << '\"';
            continue;
            
          } else if (peekEqual('\\') && peekEqual('\'', 1)) {
            consume();
            consume();
            stream << '\'';
            continue;
          }

          stream << consume().value();
        }
        addToOutput({ quote == '\"' ? PreTokenType::STRING_LITERAL : PreTokenType::CHAR_LITERAL, { .string = Utils::stringToCString(stream.str()) }, line });

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
