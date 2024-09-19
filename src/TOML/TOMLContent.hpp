#include <string>

namespace TOML {
  const int MAX_SIZE = 10;

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
    Content();
    Content(ContentType type, void* value);
    ContentType getType();
    void* getPointer();
  };


  struct Array {
  private:
    Content contents[MAX_SIZE];
    int size = 0;

  public:
    int getSize();
    Content* getContent(int index);
    bool addContent(Content* content);
  };


  struct Table {
  private:
    std::string names[MAX_SIZE];
    Array contents;
  
  public:
    int getSize();
    std::string* getNames();
    Content* getContent(std::string name);
    bool addContent(std::string name, Content* content);
  };
}
