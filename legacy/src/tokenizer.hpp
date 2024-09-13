#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <optional>

#include "util.hpp"


enum class TokenType {
  exit, int_lit, semi, var, open_paren, closed_paren, ident, eq, plus, minus, star, slash, open_curly, closed_curly,
  mod, if_, double_eq, not_eq_, while_, break_, continue_, for_, char_lit, put, get, dollar, main, colon, comma,
  return_, less_eq, extend, dot, as, greater_eq, greater, less, question
};

struct Token {
  TokenType type;
  int line;
  std::optional<std::string> value;
};


class Tokenizer {
public:
  Tokenizer(const std::string _contents) : contents(_contents) {}

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    int line = 1;
    bool ignore = false;

    while (peek().has_value()) {
      std::stringstream buf;

      if (peek().value() == '#') {
        consume();
        ignore = true;

      } else if (std::isspace(peek().value())) {
        char c = consume();
        if (c == '\n')  {
          ignore = false;
          line++;
        }

      } else if (ignore)
        consume();
      

      else if (try_consume(';'))
        tokens.push_back({ .type = TokenType::semi, .line = line });
      
      else if (try_consume('('))
        tokens.push_back({ .type = TokenType::open_paren, .line = line });
      
      else if (try_consume(')'))
        tokens.push_back({ .type = TokenType::closed_paren, .line = line });

      else if (try_consume('=')) {
        if (try_consume('=')) tokens.push_back({ .type = TokenType::double_eq, .line = line });
        else tokens.push_back({ .type = TokenType::eq, .line = line });
      }

      else if (try_consume('!')) {
        if (try_consume('=')) tokens.push_back({ .type = TokenType::not_eq_, .line = line });
        else err("Syntax error", line);
      }

      else if (try_consume('+'))
        tokens.push_back({ .type = TokenType::plus, .line = line });

      else if (try_consume('-'))
        tokens.push_back({ .type = TokenType::minus, .line = line });

      else if (try_consume('*'))
        tokens.push_back({ .type = TokenType::star, .line = line });

      else if (try_consume('/'))
        tokens.push_back({ .type = TokenType::slash, .line = line });
      
      else if (try_consume('{'))
        tokens.push_back({ .type = TokenType::open_curly, .line = line });
      
      else if (try_consume('}'))
        tokens.push_back({ .type = TokenType::closed_curly, .line = line });
      
      else if (try_consume('%'))
        tokens.push_back({ .type = TokenType::mod, .line = line });
      
      else if (try_consume('$'))
        tokens.push_back({ .type = TokenType::dollar, .line = line });
      
      else if (try_consume(':'))
        tokens.push_back({ .type = TokenType::colon, .line = line });
      
      else if (try_consume(','))
        tokens.push_back({ .type = TokenType::comma, .line = line });

      else if (try_consume('<')) {
        if (try_consume('=')) tokens.push_back({ .type = TokenType::less_eq, .line = line });
        else tokens.push_back({ .type = TokenType::less, .line = line });

      } else if (try_consume('>')) {
        if (try_consume('=')) tokens.push_back({ .type = TokenType::greater_eq, .line = line });
        else tokens.push_back({ .type = TokenType::greater, .line = line });
      }
      
      else if (try_consume('.'))
        tokens.push_back({ .type = TokenType::dot, .line = line });
      
      else if (try_consume('?'))
        tokens.push_back({ .type = TokenType::question, .line = line });


      else if (try_consume('\'')) {
        if (try_consume('\\')) {
          if (try_consume('n')) buf << '\n';
          else if (try_consume('\\')) buf << '\\';
          else err("Expected character after '\\'", line);

        } else buf << consume();

        if (!try_consume('\'')) err("Character literals can only have one character", line);
        tokens.push_back({ .type = TokenType::char_lit, .line = line, .value = buf.str() });

      } else if (std::isalpha(peek().value())) {
        while (peek().has_value() && std::isalnum(peek().value()))
          buf << consume();

        if (buf.str() == "exit")
          tokens.push_back({ .type = TokenType::exit, .line = line });
        else if (buf.str() == "var")
          tokens.push_back({ .type = TokenType::var, .line = line });
        else if (buf.str() == "if")
          tokens.push_back({ .type = TokenType::if_, .line = line });
        else if (buf.str() == "while")
          tokens.push_back({ .type = TokenType::while_, .line = line });
        else if (buf.str() == "break")
          tokens.push_back({ .type = TokenType::break_, .line = line });
        else if (buf.str() == "continue")
          tokens.push_back({ .type = TokenType::continue_, .line = line });
        else if (buf.str() == "for")
          tokens.push_back({ .type = TokenType::for_, .line = line });
        else if (buf.str() == "put")
          tokens.push_back({ .type = TokenType::put, .line = line });
        else if (buf.str() == "get")
          tokens.push_back({ .type = TokenType::get, .line = line });
        else if (buf.str() == "main")
          tokens.push_back({ .type = TokenType::main, .line = line });
        else if (buf.str() == "return")
          tokens.push_back({ .type = TokenType::return_, .line = line });
        else if (buf.str() == "extend")
          tokens.push_back({ .type = TokenType::extend, .line = line });
        else if (buf.str() == "as")
          tokens.push_back({ .type = TokenType::as, .line = line });
        else
          tokens.push_back({ .type = TokenType::ident, .line = line, .value = buf.str() });

      } else {
        while (peek().has_value() && std::isdigit(peek().value())) {
          buf << consume();
        }

        if (buf.str() == "") err("Syntax error", line);
        tokens.push_back({ .type = TokenType::int_lit, .line = line, .value = buf.str() });
      }
    }

    return tokens;
  }

private:
  std::optional<char> peek(int offset = 0) {
    if (index + offset >= contents.length()) {
      return {};
    }
    return contents.at(index + offset);
  }

  char consume() {
    return contents.at(index++);
  }

  bool try_consume(char c) {
    if (peek().has_value() && peek().value() == c) {
      consume();
      return true;
    }
    return false;
  }

  const std::string contents;
  std::size_t index = 0;
};
