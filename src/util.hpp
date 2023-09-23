#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>


void err(std::string msg) {
  std::cerr << msg << "\n";
  exit(EXIT_FAILURE);
}

void err(std::string msg, int line) {
  std::cerr << msg << " on line: " << line << "\n";
  exit(EXIT_FAILURE);
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
