#pragma once
#include <vector>
#include <string>

#include <util/ErrorUtils.hpp>

namespace Parser {
  using index_t = int;  //* Represents an index; if the value is -1 then it means "not found" or "invalid"
  const index_t INDEX_T_NOT_FOUND = -1;

  class IntStack {
  public:
    void push(int i);
    int pop();

  private:
    std::vector<int> vect;
  };

  [[nodiscard]] Utils::Error syntaxError(const char* msg, int line);


  namespace Result {
    using Utils::Error;
    typedef int None;

    template <typename T>
    struct inst {
      T* value;
      Error error;

      bool hasValue() { return value != NULL; }
      bool isError() { return value == NULL; }

      T* expectValue() {
        if (isError())
          Utils::error(error);
        return value;
      }
    };

    template <typename T>
    inst<T> success(T* value) { return { value, {} }; }
    inst<None> success();

    template <typename T>
    inst<T> error(Error err) { return { NULL, err }; }
    inst<None> error(Error err);
  }
}
