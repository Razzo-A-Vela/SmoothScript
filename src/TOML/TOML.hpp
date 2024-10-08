#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include <util/StringUtils.hpp>
#include <util/FileUtils.hpp>
#include "TOMLContent.hpp"

namespace TOML {
  class File {
  private:
    std::string fileName;
    std::fstream file;
    TOML::Table contents;
    bool exists;

  public:
    File(std::string fileName);
    const std::function<bool(char)> notInside = [](char c) { return c == '\"' || c == '\''; };

    bool doesExist() { return exists; }
    void createIfNonExistent(std::string defaultContent = "");
    void printContent(Content* content);
    void print();
    Content* calculateContent(std::string string);
    void read();
    void init(std::string defaultContent = "");
    Content* getContent(std::string name) { return contents.getContent(name); }
  };
}
