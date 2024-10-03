#pragma once
#include <vector>
#include <functional>

namespace Utils {
  template<typename T>
  std::vector<T> insertInIndex(std::vector<T> toInsert, int index, std::function<void(std::vector<T>*)> addFunc) {
    std::vector<T> ret;

    for (int i = 0; i < toInsert.size(); i++) {
      if (i == index)
        addFunc(&ret);
      ret.push_back(toInsert.at(i));
    }

    return ret;
  }

  template<typename T>
  std::vector<T> insertInIndex(std::vector<T> toInsert, int index, T toAdd) {
    std::vector<T> ret;

    for (int i = 0; i < toInsert.size(); i++) {
      if (i == index)
        ret.push_back(toAdd);
      ret.push_back(toInsert.at(i));
    }

    return ret;
  }

  template<typename T>
  int getVectorIndex(std::vector<T> vec, T toCheck, std::function<bool(T, T)> equalFunc) {
    for (int i = 0; i < vec.size(); i++) {
      if (equalFunc(vec.at(i), toCheck))
        return i;
    }
    return -1;
  }

  template<typename T>
  int getVectorIndex(std::vector<T> vec, T toCheck) {
    for (int i = 0; i < vec.size(); i++) {
      if (vec.at(i) == toCheck)
        return i;
    }
    return -1;
  }

  template<typename T>
  bool isInVector(std::vector<T> vec, T toCheck, std::function<bool(T, T)> equalFunc) {
    return getVectorIndex(vec, toCheck, equalFunc) != -1;
  }

  template<typename T>
  bool isInVector(std::vector<T> vec, T toCheck) {
    return getVectorIndex(vec, toCheck) != -1;
  }
}
