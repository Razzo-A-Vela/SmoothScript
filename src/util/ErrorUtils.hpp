#pragma once
#include <iostream>
#include <string>

namespace Utils {
  struct Error {
    const char* errType = "";
    const char* msg = "";
    int line = -1;
  };

  void setErrorFileName(std::string toSet);
  std::string getErrorFileName();
  void resetErrorFileName();

  [[noreturn]] void error(std::string errType, std::string msg, int line);
  [[noreturn]] void error(std::string errType, std::string msg);
  [[noreturn]] void error(std::string msg, int line);
  [[noreturn]] void error(std::string msg);
  [[noreturn]] void error(Error err);
}
