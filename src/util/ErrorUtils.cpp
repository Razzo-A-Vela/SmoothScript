#include "ErrorUtils.hpp"

namespace Utils {
  std::string errorFileName = "";

  void setErrorFileName(std::string toSet) {
    errorFileName = toSet;
  }

  std::string getErrorFileName() {
    return errorFileName;
  }

  void resetErrorFileName() {
    errorFileName = "";
  }


  void error(std::string errType, std::string msg, int line) {
    std::cout << errType << ": " << msg;
    if (line != -1)
      std::cout << " on line: " << line;
    if (errorFileName != "")
      std::cout << " in file: \"" << errorFileName << '\"';
    std::cout << '\n';
    exit(1);
  }

  void error(std::string errType, std::string msg) {
    error(errType, msg, -1);
  }

  void error(std::string msg, int line) {
    error("Error", msg, line);
  }
  
  void error(std::string msg) {
    error("Error", msg, -1);
  }
}
