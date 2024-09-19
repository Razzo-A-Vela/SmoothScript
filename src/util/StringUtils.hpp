#include <string>
#include <sstream>
#include <vector>
#include <functional>

namespace Utils {
  std::vector<std::string> contextSplit(std::string toSplit, char delimiter, std::function<bool(char)> notInside);
  std::vector<std::string> split(std::string toSplit, char delimiter);

  std::string trim(std::string toTrim, std::function<bool(char)> trimChar);
  std::string trim(std::string toTrim);

  std::vector<std::string> trimVector(std::vector<std::string> strings);
}
