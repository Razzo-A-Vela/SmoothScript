#include "TOMLContent.hpp"

namespace TOML {
  Content::Content() {
    type = ContentType::null;
  }

  Content::Content(ContentType type, void* value) : type(type) {
    switch(type) {
      case ContentType::boolean :
        u.boolean = (bool*) value;
        break;
      case ContentType::string :
        u.string = (char*) value;
        break;
      case ContentType::array :
        u.array = (Array*) value;
        break;
      case ContentType::table :
        u.table = (Table*) value;
        break;
    }
  }

  ContentType Content::getType() {
    return type;
  }

  void* Content::getPointer() {
    ContentType type = getType();

    switch (type) {
      case ContentType::array :
        return u.array;
      
      case ContentType::boolean :
        return u.boolean;
      
      case ContentType::string :
        return u.string;
      
      case ContentType::table :
        return u.table;
      
      default :
        return nullptr;
    }
  }


  int Array::getSize() {
    return size;
  }

  Content* Array::getContent(int index) {
    if (index < 0 || index >= size)
      return nullptr;
    
    Content* ret = contents + index;
    if (ret->getType() == ContentType::null)
      return nullptr;
    return ret;
  }

  bool Array::addContent(Content* content) {
    if (content == nullptr)
      return false;
    if (content->getType() == ContentType::null)
      return false;
    if (size == MAX_SIZE)
      return false;
    contents[size++] = *content;
    return true;
  }


  int Table::getSize() {
    return contents.getSize();
  }

  std::string* Table::getNames() {
    return names;
  }

  Content* Table::getContent(std::string name) {
    int index = -1;
    for (int i = 0; i < MAX_SIZE; i++) {
      if (i >= getSize())
        break;
      if (names[i] == name)
        index = i;
    }

    if (index == -1)
      return nullptr;
    return contents.getContent(index);
  }

  bool Table::addContent(std::string name, Content* content) {
    bool ret = contents.addContent(content);
    if (ret)
      names[contents.getSize() - 1] = name;

    return ret;
  }
}
