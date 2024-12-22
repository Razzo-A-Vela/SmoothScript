#include "TOML.hpp"

namespace TOML {
  void Token::print(std::ostream& out) {
    switch (type) {
      case TokenType::IDENTIFIER :
        out << "IDENTIFIER(" << u.string << ')';
        break;
      
      case TokenType::EQUALS :
        out << "EQUALS";
        break;
      
      case TokenType::TRUE :
        out << "TRUE";
        break;

      case TokenType::FALSE :
        out << "FALSE";
        break;
      
      case TokenType::STRING :
        out << "STRING(" << u.string << ')';
        break;
      
      case TokenType::NEW_LINE :
        out << "NEW_LINE";
        break;
      
      case TokenType::DOT :
        out << "DOT";
        break;
      
      case TokenType::OPEN_SQUARE :
        out << "OPEN_SQUARE";
        break;
      
      case TokenType::CLOSED_SQUARE :
        out << "CLOSED_SQUARE";
        break;
    }
  }

  const std::function<bool(Token, Token)> Token::typeEqual = [](Token token, Token toEqual) { return token.type == toEqual.type; };


  void Tokenizer::process() {
    while (hasPeek()) {
      char c = consume().value();
      Token token;
      token.line = line;

      if (c == '#') {
        while (hasPeek()) {
          char c = consume().value();
          if (c == '\n')
            break;
        }
        continue;
      }

      if (c == '\r' || c == '\t' || c == ' ')
        continue;
      
      
      if (c == '\n')
        token.type = TokenType::NEW_LINE;

      else if (c == '=')
        token.type = TokenType::EQUALS;
      
      else if (c == '.')
        token.type = TokenType::DOT;
      
      else if (c == '[')
        token.type = TokenType::OPEN_SQUARE;
      
      else if (c == ']')
        token.type = TokenType::CLOSED_SQUARE;

      //TODO: Numbers
      else if (isdigit(c)) {
        Utils::error("TOML Internal Error", "Numbers are not done yet", line);
      
      } else if (c == '"' || c == '\'') {
        std::stringstream buff;
        char quote = c;
        char quoteC = 0;

        while (true) {
          if (!hasPeek())
            Utils::error("TOML Error", "Invalid String", line);
          quoteC = consume().value();

          //TODO: Add triple quotes
          if (quoteC == quote)
            break;
          else if (quoteC == '\n')
            Utils::error("TOML Error", "Invalid String", line);
          
          if (quoteC == '\\') {
            if (!hasPeek())
              Utils::error("TOML Error", "Invalid String", line);
            char escapeC = consume().value();
            
            //TODO: All escape characters
            if (escapeC == quote)
              buff << escapeC;
            else if (escapeC == 'n')
              buff << '\n';
            else if (escapeC == 't')
              buff << '\t';
            else
              Utils::error("TOML Error", "Invalid escape character", line);
            
          } else
            buff << quoteC;
        }

        //TODO: Literal strings
        if (quote == '\'') {
          Utils::error("TOML Internal Error", "Literal strings are not done yet", line);

        } else {
          token.type = TokenType::STRING;
          token.u.string = Utils::stringToCString(buff.str());
        }

      } else if (isalnum(c) || c == '_' || c == '-') {
        std::stringstream buff;
        buff << c;

        while (hasPeek() && (isalnum(peekValue()) || peekValue() == '_' || peekValue() == '-'))
          buff << consume().value();
        std::string s = buff.str();

        if (s == "true")
          token.type = TokenType::TRUE;
        
        else if (s == "false")
          token.type = TokenType::FALSE;

        else {
          token.type = TokenType::IDENTIFIER;
          token.u.string = Utils::stringToCString(s);
        }

      } else
        Utils::error("TOML Error", "Invalid token", line);
      
      addToOutput(token);
    }
  }

  void Tokenizer::print(std::ostream& out) {
    std::vector<Token> output = getOutput();
    for (Token token : output) {
      token.print(out);
      out << '\n';
    }
  }

  std::optional<char> Tokenizer::consume() {
    std::optional<char> ret = Processor::consume();
    if (ret.has_value() && ret.value() == '\n')
      line++;
    return ret;
  }


  Table* Parser::getTable(Table* table, std::string name) {
    if (table->map.hasKey(name)) {
      Content* content = table->map.getValue(name).value();

      if (content->type == ContentType::TABLE)
        return content->u.table;
    }

    return NULL;
  }

  Table* Parser::addTableTo(Table* toAdd, std::string tableName) {
    Table* table = new Table();
    Content* content = new Content();
    content->type = ContentType::TABLE;
    content->u.table = table;
    toAdd->map.add(tableName, content);

    return table;
  }

  Content* Parser::processValue() {
    Content* ret = new Content();

    //TODO: Add inline tables, arrays and numbers
    if (peekEqual({ TokenType::TRUE }, Token::typeEqual) || peekEqual({ TokenType::FALSE }, Token::typeEqual)) {
      ret->type = ContentType::BOOL;
      ret->u.boolean = Token::typeEqual(consume().value(), { TokenType::TRUE });
    
    } else if (peekEqual({ TokenType::STRING }, Token::typeEqual)) {
      ret->type = ContentType::STRING;
      ret->u.string = consume().value().u.string;

    } else
      Utils::error("TOML Error", "Invalid content", getErrLine());

    return ret;
  }

  std::string Parser::processKey() {
    //TODO: Quoted keys
    if (peekNotEqual({ TokenType::IDENTIFIER }, Token::typeEqual))
      Utils::error("TOML Error", "Invalid key", getErrLine());
    std::string tokenString = std::string(consume().value().u.string);
    
    if (peekEqual({ TokenType::DOT }, Token::typeEqual)) {
      consume();
      TOML::Table* table = getTable(toModifyTable, tokenString);

      if (table == NULL)
        toModifyTable = addTableTo(toModifyTable, tokenString);
      else
        toModifyTable = table;

      return processKey();
    }
    
    return tokenString;
  }

  void Parser::process() {
    root = new Table();
    currTable = root;
    toModifyTable = root;

    while (hasPeek()) {
      if (peekEqual({ TokenType::NEW_LINE }, Token::typeEqual)) {
        consume();
        continue;
      }

      if (peekEqual({ TokenType::OPEN_SQUARE }, Token::typeEqual)) {
        consume();
        
        //TODO: Array of tables
        if (peekEqual({ TokenType::OPEN_SQUARE }, Token::typeEqual))
          Utils::error("TOML Internal Error", "Arrays of tables are not done yet", getErrLine());

        toModifyTable = root;
        std::string key = processKey();
        if (getTable(toModifyTable, key) != NULL)
          Utils::error("TOML Error", "Redefining table", getErrLine());

        currTable = addTableTo(toModifyTable, key);
        toModifyTable = currTable;

        if (peekNotEqual({ TokenType::CLOSED_SQUARE }, Token::typeEqual))
          Utils::error("TOML Error", "Expected ']' after '['", getErrLine());
        consume();
        continue;
      }

      std::string key = processKey();
      if (peekNotEqual({ TokenType::EQUALS }, Token::typeEqual))
        Utils::error("TOML Error", "Expected '=' after table content identifier", getErrLine());
      consume();

      Content* value = processValue();
      toModifyTable->map.add(key, value);
      toModifyTable = currTable;
    }

    addToOutput(root);
  }

  void Parser::print(std::ostream& out) {
    Table* output = getSingleOutput();
    output->print(out);
    out << '\n';
  }

  
  Parser* Parser::read(std::string fileContent) {
    Tokenizer* tokenizer = new Tokenizer(std::string(fileContent));
    tokenizer->process();

    Parser* parser = new Parser(tokenizer->getOutput());
    parser->process();

    return parser;
  }

  Parser* Parser::readOrError(std::string fileName) {
    if (!Utils::fileExists(fileName)) {
      Utils::resetErrorFileName();
      Utils::error("TOML File Error", std::string("File: \"") + fileName + std::string("\" does not exist"));
    }
    return read(Utils::readEntireFile(fileName));
  }

  Parser* Parser::readOrCreate(std::string fileName, std::string defaultFileString) {
    if (!Utils::fileExists(fileName)) {
      std::ofstream file;
      file.open(fileName);
      file << defaultFileString;
      file.close();
      return read(defaultFileString);
    }
    return readOrError(fileName);
  }
}
