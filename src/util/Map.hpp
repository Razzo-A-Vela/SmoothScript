#pragma once
#include <vector>
#include <optional>
#include <functional>

#include <util/VectorUtils.hpp>

template<typename Key, typename Value>
class Map {
private:
  std::vector<Key> keys;
  std::vector<Value> values;

public:
  Map() {}
  
  void add(Key key, Value value) {
    keys.push_back(key);
    values.push_back(value);
  }

  void removeLast() {
    keys.pop_back();
    values.pop_back();
  }

  int size() {
    return values.size();
  }

  std::optional<Value> at(int index) {
    if (index >= 0 && index < size())
      return values.at(index);
    return {};
  }
  
  std::vector<Key> getKeys() {
    return keys;
  }

  std::vector<Value> getValues() {
    return values;
  }
  
  bool hasKey(Key key) {
    return Utils::isInVector(keys, key);
  }

  int getIndex(Key key) {
    return Utils::getVectorIndex(keys, key);
  }
  
  std::optional<Value> getValue(Key key) {
    if (hasKey(key))
      return at(getIndex(key));
    return {};
  }
  
  std::optional<Value> last() {
    return at(size() - 1);
  }
};
