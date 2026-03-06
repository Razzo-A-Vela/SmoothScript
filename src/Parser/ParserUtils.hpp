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

  Utils::Error syntaxError(const char* msg, int line);


  namespace Result {
    using Utils::Error;
    typedef int None;

    template <typename T>
    struct inst {
      T* value;
      bool isError;
      Error error;
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

    template <typename T>
    bool hasValue(inst<T> result) { return result.value != NULL; }
    template <typename T>
    bool isError(inst<T> result) { return !hasValue(result) && result.isError; }
    template <typename T>
    bool isIgnored(inst<T> result) { return !hasValue(result) && !result.isError; }

    template <typename T>
    T* expect(inst<T> result) {
      if (!hasValue(result))
        Utils::error(result.error);
      return result.value;
    }

    template <typename T>
    inst<T> throwErr(inst<T> result) {
      if (isError(result))
        Utils::error(result.error);
      return result;
    }
  }
}
