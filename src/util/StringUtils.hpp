#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <malloc.h>
#include <string.h>

namespace Utils {
  std::vector<std::string> contextSplit(std::string toSplit, char delimiter, std::function<bool(char)> notInside);
  std::vector<std::string> split(std::string toSplit, char delimiter);

  std::string trim(std::string toTrim, std::function<bool(char)> trimChar);
  std::string trim(std::string toTrim);

  std::vector<std::string> trimVector(std::vector<std::string> strings);
  char* stringToCString(std::string str);
  char charUpper(char c);
  char charLower(char c);
}
