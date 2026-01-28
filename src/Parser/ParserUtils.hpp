#pragma once
#include <vector>
#include <string>

#include <util/ErrorUtils.hpp>

namespace Parser {
  [[noreturn]] void syntaxError(std::string msg, int line);

  class IntStack {
  public:
    void push(int i);
    int pop();

  private:
    std::vector<int> vect;
  };

  namespace Result {
    template <typename T>
    struct inst {
      T* value;
      bool isError;
      const char* error;
      int line;
    };

    template <typename T>
    inst<T> success(T* value) { return { value, false, "", -1 }; }
    template <typename T>
    inst<T> ignore(const char* error, int line) { return { NULL, false, error, line }; }
    template <typename T>
    inst<T> error(const char* error, int line) { return { NULL, true, error, line }; }

    template <typename T>
    bool hasValue(inst<T> result) { return result.value != NULL; }
    template <typename T>
    bool isError(inst<T> result) { return !hasValue(result) && result.isError; }
    template <typename T>
    bool isIgnored(inst<T> result) { return !hasValue(result) && !result.isError; }

    template <typename T>
    T* expect(inst<T> result) {
      if (!hasValue(result))
        syntaxError(result.error, result.line);
      return result.value;
    }
  }
}
