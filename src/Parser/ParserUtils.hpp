#pragma once
#include <vector>
#include <string>

#include <util/ErrorUtils.hpp>

namespace Parser {
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
      bool _isError;
      Error error;

      bool hasValue() { return value != NULL; }
      bool isError() { return !hasValue() && _isError; }
      bool isIgnored() { return !hasValue() && !_isError; }

      T* expect() {
        if (!hasValue())
          Utils::error(error);
        return value;
      }

      inst<T> throwErr() {
        if (isError())
          Utils::error(error);
        return *this;
      }
    };

    template <typename T>
    inst<T> success(T* value) { return { value, false, {} }; }
    inst<None> success();

    template <typename T>
    inst<T> ignore(Error err) { return { NULL, false, err }; }
    inst<None> ignore(Error err);

    template <typename T>
    inst<T> error(Error err) { return { NULL, true, err }; }
    inst<None> error(Error err);
  }
}
