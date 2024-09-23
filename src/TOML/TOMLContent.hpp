#pragma once
#include <string>

namespace TOML {
  const int MAX_SIZE = 25;

  struct Array;
  struct Table;

  enum class ContentType {
    null, boolean, string, array, table
  };

  struct Content {
  private:
    ContentType type;
    union {
      bool* boolean;
      char* string;
      Array* array;
      Table* table;
    } u;

  public:
    Content() : type(ContentType::null) {}
    Content(ContentType type, void* value);
    ContentType getType() { return type; }
    void* getPointer();
  };


  struct Array {
  private:
    Content contents[MAX_SIZE];
    int size = 0;

  public:
    int getSize() { return size; }
    Content* getContent(int index);
    bool addContent(Content* content);
  };


  struct Table {
  private:
    std::string names[MAX_SIZE];
    Array contents;
  
  public:
    int getSize() { return contents.getSize(); }
    std::string* getNames() { return names; }
    Content* getContent(std::string name);
    bool addContent(std::string name, Content* content);
  };
}
