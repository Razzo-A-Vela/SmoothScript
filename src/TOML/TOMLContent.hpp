#pragma once
#include <string>

#include <util/ErrorUtils.hpp>
#include <util/Map.hpp>

namespace TOML {
  struct Table;
  struct List;

  enum class ContentType {
    TABLE, LIST, STRING, BOOL
  };

  struct Content {
    ContentType type;
    union {
      Table* table;
      List* list;
      const char* string;
      bool boolean;
    } u;

    void print(std::ostream& out);
    static std::string getTypeAsString(ContentType type);
  };
  

  struct Table {
    Map<std::string, Content*> map;

    void setCheckType(ContentType toCheck) {
      checkType = toCheck;
    }

    Content* getContent(std::string contentName) {
      return getContent(contentName, checkType);
    }
    
    Content* getContentOrError(std::string contentName);
    Content* getContent(std::string contentName, ContentType toCheck);
    void print(std::ostream& out);
    
  private:
    ContentType checkType = ContentType::TABLE;
  };

  struct List {
    std::vector<Content*> list;
    
    void setCheckType(ContentType toCheck) {
      checkType = toCheck;
    }
    
    Content* getContent(int index) {
      return getContent(index, checkType);
    }
    
    Content* getContentOrError(int index);
    Content* getContent(int index, ContentType toCheck);
    void print(std::ostream& out);
    
  private:
    ContentType checkType = ContentType::TABLE;
  };
}
