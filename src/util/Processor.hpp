#pragma once
#include <vector>
#include <optional>
#include <functional>

#include <util/ErrorUtils.hpp>

namespace Utils {
  template<typename I, typename O>
  class Processor {
  private:
    std::vector<O> output;
    int index = 0;
    int maxIndex;

  public:
    Processor(int maxIndex) : maxIndex(maxIndex) {}
    
    virtual void process() { Utils::error("Cannot call process from processor"); }
    virtual void print(std::ostream& out) { Utils::error("Cannot call print from processor"); }
    std::vector<O> getOutput() { return output; }
    O getSingleOutput() { return output.at(0); }

  protected:
    virtual I get(int index) { Utils::error("Cannot call get from processor"); }
    void addToOutput(O out) { output.push_back(out); }
    int getIndex() { return index; }
    void resetIndex() { index = 0; }
    void resetOutput() { output = std::vector<O>(); }
    void setMaxIndex(int index) { maxIndex = index; }


    bool peekEqual(I equal, std::function<bool(I, I)> equalFunc, int offset = 0) {return hasPeek(offset) && equalFunc(peekValue(offset), equal); }
    bool peekEqual(I equal, int offset = 0) { return hasPeek(offset) && peekValue(offset) == equal; }

    bool peekNotEqual(I equal, std::function<bool(I, I)> equalFunc, int offset = 0) { return !hasPeek(offset) || !equalFunc(peekValue(offset), equal); }
    bool peekNotEqual(I notEqual, int offset = 0) { return !hasPeek(offset) || peekValue(offset) != notEqual; }

    bool hasPeek(int offset = 0) { return peek(offset).has_value(); }
    I peekValue(int offset = 0) { return peek(offset).value(); }
    std::optional<I> peek(int offset = 0) {
      int i = index + offset;

      if (i < 0 || i >= maxIndex)
        return {};

      return get(i);
    }

    
    bool tryConsume(I equal, std::function<bool(I, I)> equalFunc) { return peekEqual(equal, equalFunc) && consume().has_value(); }
    bool tryConsume(I equal) { return peekEqual(equal) && consume().has_value(); }
    std::optional<I> consume() {
      std::optional<I> t = peek();

      if (t.has_value())
        index++;

      return t;
    }
  };
}
