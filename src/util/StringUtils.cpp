#include "StringUtils.hpp"

namespace Utils {
  std::vector<std::string> contextSplit(std::string toSplit, char delimiter, std::function<bool(char)> notInside) {
    std::vector<std::string> ret;
    std::stringstream stream;
    bool inside = false;
    bool backSlash = false;
    char insideChar = '\0';

    for (int i = 0; i < toSplit.size(); i++) {
      char c = toSplit.at(i);

      if (inside) {
        if (c == insideChar && !backSlash)
          inside = false;
        
        if (c == '\\') {
          backSlash = !backSlash;
        } else {
          backSlash = false;
        }
        stream << c;
        continue;
      }

      if (notInside(c)) {
        inside = true;
        insideChar = c;
        stream << c;
        continue;
      }

      if (c == delimiter) {
        ret.insert(ret.end(), stream.str());
        stream = std::stringstream();
      } else
        stream << c;
    }

    if (stream.str() != "")
      ret.insert(ret.end(), stream.str());
    return ret;
  }

  std::vector<std::string> split(std::string toSplit, char delimiter) {
    std::vector<std::string> ret;
    std::stringstream stream;
    
    for (char c : toSplit) {
      if (c == delimiter) {
        ret.insert(ret.end(), stream.str());
        stream = std::stringstream();
      } else
        stream << c;
    }

    if (stream.str() != "")
      ret.insert(ret.end(), stream.str());
    return ret;
  }


  std::string trim(std::string toTrim, std::function<bool(char)> trimChar) {
    std::stringstream stream;
    bool foundAlphaLeft = false;
    int foundAlphaRight = -1;

    for (char c : toTrim) {
      if (!trimChar(c))
        foundAlphaLeft = true;

      if (foundAlphaLeft)
        stream << c;
    }

    std::string retString = stream.str();
    for (int i = retString.size() - 1; i >= 0; i--) {
      char c = retString.at(i);
      if (!trimChar(c)) {
        foundAlphaRight = i;
        break;
      }
    }

    return retString.substr(0, foundAlphaRight + 1);
  }

  std::string trim(std::string toTrim) {
    return trim(toTrim, [](char c) { return c == ' ' || c == '\n'; });
  }

  std::vector<std::string> trimVector(std::vector<std::string> strings) {
    std::vector<std::string> ret;
    for (std::string string : strings)
      ret.insert(ret.end(), trim(string));
    return ret;
  }
}
