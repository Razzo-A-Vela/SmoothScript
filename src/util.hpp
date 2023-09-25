#pragma once
#include <iostream>
#include <string>
#include <optional>


void err(std::string msg) {
  std::cerr << msg << "\n";
  exit(EXIT_FAILURE);
}

void err(std::string msg, int line) {
  std::cerr << msg << " on line: " << line << "\n";
  exit(EXIT_FAILURE);
}


#include "tokenizer.hpp"
#include "nodes.hpp"


std::optional<ExprBinType> getBinType(TokenType type) {
  switch (type) {
    case TokenType::mod :
      return ExprBinType::mod;
    case TokenType::slash :
      return ExprBinType::div;
    case TokenType::star :
      return ExprBinType::mult;
    case TokenType::minus :
      return ExprBinType::sub;
    case TokenType::plus :
      return ExprBinType::add;
    case TokenType::double_eq :
      return ExprBinType::eq;
    case TokenType::not_eq_ :
      return ExprBinType::not_eq_;

    default :
      return {};
  }
}

bool isBinOp(TokenType type) {
  return getBinType(type).has_value();
}

int getBinExprPrec(ExprBinType type) {
  switch (type) {
    case ExprBinType::mod :   
    case ExprBinType::div :
    case ExprBinType::mult :
      return 2;
    
    default :
      return 1;
    
    case ExprBinType::eq :
    case ExprBinType::not_eq_ :
      return 0;
  }
}

bool isBoolBinExpr(ExprBinType type) {
  switch (type) {
    case ExprBinType::eq :
    case ExprBinType::not_eq_ :
      return true;

    default :
      return false;
  }
}


class Allocator {
public:
  Allocator(size_t _size) : size(_size) {
    buffer = static_cast<std::byte*>(malloc(size));
    index = buffer;
  }

  ~Allocator() {
    free(buffer);
  }

  Allocator(const Allocator& other) = delete;
  Allocator operator=(const Allocator& other) = delete;

  template<typename T>
  T* alloc() {
    void* this_index = index;
    index += sizeof(T);
    T* ret = static_cast<T*>(this_index);
    new (ret) T();
    return ret;
  }

private:
  size_t size;
  std::byte* buffer;
  std::byte* index;
};
