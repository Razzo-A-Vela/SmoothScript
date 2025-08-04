#pragma once
#include <vector>

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
    template<typename T>
    struct inst {
      T* value;
      const char* error;
      int line = -1;
      int ignoredStep = -1;
    };

    template<typename T>
    inst<T> success(T* value) { return { value, "", -1, -1 }; }
    template<typename T>
    inst<T> error(const char* error, int line = -1) { return { NULL, error, line, -1 }; }
    template<typename T>
    inst<T> ignore(int ignoredStep) { return { NULL, NULL, -1, ignoredStep }; }
    template<typename T>
    inst<T> ignore() { return ignore<T>(1); }

    template<typename T>
    bool isError(inst<T> result) { return result.value == NULL && result.ignoredStep == -1; }
    template<typename T>
    bool hasValue(inst<T> result) { return result.value != NULL; }
    template<typename T>
    bool isIgnored(inst<T> result) { return result.ignoredStep != -1; }
    template<typename T>
    bool hasLine(inst<T> result) { return result.line != -1; }
  }
}
