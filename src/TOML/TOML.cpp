#include "TOML.hpp"

namespace TOML {
  File::File(std::string fileName_) {
    fileName = fileName_;
    file = std::fstream(fileName, std::ios::in);
    exists = file.good();
  }

  void File::createIfNonExistent(std::string defaultContent) {
    if (exists)
      return;
    
    file.close();
    std::ofstream out = std::ofstream(fileName);
    out << defaultContent;
    out.close();
    file = std::fstream(fileName, std::ios::in);
    exists = file.good();
  }

  void File::printContent(Content* content) {
    ContentType type = content->getType();
    void* pointer = content->getPointer();
    Array* arr = (Array*) pointer;
    Table* table = (Table*) pointer;

    switch (type) {
      case ContentType::null :
        std::cout << "null\n";
        break;
      
      case ContentType::boolean :
        std::cout << "bool=" << (*((bool*) pointer) ? "true" : "false");
        break;
      
      case ContentType::string :
        std::cout << "string=\"" << std::string((char*) pointer) << '\"';
        break;
      
      case ContentType::array :
        std::cout << "array=[ ";

        for (int i = 0; i < arr->getSize(); i++) {
          printContent(arr->getContent(i));

          if (i != arr->getSize() - 1)
            std::cout << ", ";
        }
        std::cout << " ]";
        break;
      
      case ContentType::table :
        std::cout << "table={ ";
        std::string* names = table->getNames();

        for (int i = 0; i < table->getSize(); i++) {
          std::string name = names[i];
          std::cout << name << ": ";
          printContent(table->getContent(name));
        }
        std::cout << " }";
        break;
    }
  }

  void File::print() {
    if (!exists) {
      std::cout << "File doesn't exist\n";
      return;
    }

    std::string* names = contents.getNames();
    for (int i = 0; i < contents.getSize(); i++) {
      std::string name = names[i];
      Content* content = contents.getContent(name);

      std::cout << name << ": ";
      printContent(content);
      std::cout << '\n';
    }
  }

  Content* File::calculateContent(std::string string) {
    Content* ret;

    if (string.at(0) == '\"') {
      char* str = Utils::stringToCString(string.substr(1, string.size() - 1 - 1));
      ret = new Content(TOML::ContentType::string, str);
    
    } else if (string.at(0) == '[') {
      std::string toSplit = string.substr(1, string.size() - 1 - 1);
      std::vector<std::string> split = Utils::contextSplit(toSplit, ',', notInside);
      std::vector<std::string> splitTrim = Utils::trimVector(split);
      Array* arr = new Array();

      for (std::string str : splitTrim)
        arr->addContent(calculateContent(str));
      ret = new Content(TOML::ContentType::array, arr);
      
    } else if (string == "true" || string == "false") {
      bool* boolean = new bool(string == "true");
      ret = new Content(TOML::ContentType::boolean, boolean);

    } else
      ret = new Content(TOML::ContentType::null, nullptr);
    return ret;
  }

  void File::read() {
    std::string fileString = Utils::readEntireFile(&file);

    std::vector<std::string> lineSplit = Utils::split(fileString, '\n');
    for (std::string line : lineSplit) {
      if (line.size() == 0)
        continue;

      std::vector<std::string> commentSplit = Utils::contextSplit(line, '#', notInside);
      std::vector<std::string> equalSplit = Utils::contextSplit(commentSplit.at(0), '=', notInside);
      std::vector<std::string> equalSplitTrim = Utils::trimVector(equalSplit);

      if (equalSplitTrim.size() < 2)
        continue;
      std::string name = equalSplitTrim.at(0);
      Content* content = calculateContent(equalSplitTrim.at(1));
      contents.addContent(name, content);
    }
  }

  void File::init(std::string defaultContent) {
    createIfNonExistent(defaultContent);
    read();
  }
}
