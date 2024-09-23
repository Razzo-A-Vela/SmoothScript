#include "FileUtils.hpp"

namespace Utils {
  std::string readEntireFile(std::fstream* file) {
    std::stringstream stream;
    stream << file->rdbuf();
    return stream.str();
  }
  
  std::string readEntireFile(std::string fileName) {
    std::fstream file = std::fstream(fileName, std::ios::in);
    std::string ret = readEntireFile(&file);
    file.close();
    return ret;
  }

  bool fileExists(std::string fileName) {
    std::fstream file = std::fstream(fileName, std::ios::in);
    bool ret = file.good();
    file.close();
    return ret;
  }
}
