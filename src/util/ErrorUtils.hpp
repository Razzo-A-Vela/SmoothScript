#include <iostream>
#include <string>

namespace Utils {
  [[noreturn]] void error(std::string errType, std::string msg, int line);
  [[noreturn]] void error(std::string errType, std::string msg);
  [[noreturn]] void error(std::string msg, int line);
  [[noreturn]] void error(std::string msg);
}
