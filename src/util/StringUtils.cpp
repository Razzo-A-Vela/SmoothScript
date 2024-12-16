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
        ret.push_back(stream.str());
        stream = std::stringstream();
      } else
        stream << c;
    }

    if (stream.str() != "")
      ret.push_back(stream.str());
    return ret;
  }

  std::vector<std::string> split(std::string toSplit, char delimiter) {
    std::vector<std::string> ret;
    std::stringstream stream;
    
    for (char c : toSplit) {
      if (c == delimiter) {
        ret.push_back(stream.str());
        stream = std::stringstream();
      } else
        stream << c;
    }

    if (stream.str() != "")
      ret.push_back(stream.str());
    return ret;
  }


  std::string replace(std::string toReplace, std::string toFind, std::string replaceValue) {
    std::stringstream retStream;

    for (int i = 0; i < toReplace.size(); i++) {
      if (toReplace.at(i) == toFind.at(0)) {
        bool found = true;
        for (int j = 0; j < toFind.size(); j++) {
          if (i + j >= toReplace.size() || toReplace.at(i + j) != toFind.at(j)) {
            found = false;
            break;
          }
        }

        if (found) {
          retStream << replaceValue;
          i += toFind.size() - 1;
          continue;
        }
      }
      
      retStream << toReplace.at(i);
    }

    return retStream.str();
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
      ret.push_back(trim(string));
    return ret;
  }

  char charUpper(char c) {
    if (c >= 0x61 && c <= 0x7A)
      return c - 0x20;
    return c;
  }

  char charLower(char c) {
    if (c >= 0x41 && c <= 0x5A)
      return c + 0x20;
    return c;
  }

  char* stringToCString(std::string str) {
    const int retSize = str.size() + 1;
    char* ret = (char*) malloc(retSize * sizeof(char)); //? sizeof(char) = 1
    memcpy(ret, str.c_str(), retSize);
    return ret;
  }
}
