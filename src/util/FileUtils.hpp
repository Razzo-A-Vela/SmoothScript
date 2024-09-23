#pragma once
#include <string>
#include <sstream>
#include <fstream>

namespace Utils {
  std::string readEntireFile(std::fstream* file);
  std::string readEntireFile(std::string fileName);

  bool fileExists(std::string fileName);
}
