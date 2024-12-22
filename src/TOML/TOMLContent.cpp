#include "TOMLContent.hpp"

namespace TOML {
  void Content::print(std::ostream& out) {
    switch (type) {
      case ContentType::TABLE :
        u.table->print(out);
        break;

      case ContentType::BOOL :
        out << (u.boolean ? "true" : "false");
        break;
      
      case ContentType::STRING :
        out << '\"' << u.string << '\"';
        break;
    }
  }

  std::string Content::getTypeAsString(ContentType type) {
    switch (type) {
      case ContentType::TABLE :
        return std::string("table");

      case ContentType::BOOL :
        return std::string("boolean");
      
      case ContentType::STRING :
        return std::string("string");
    }

    return std::string("");
  }


  Content* Table::getContentOrError(std::string contentName) {
    Content* ret = getContent(contentName);
    if (ret == NULL)
      Utils::error("TOML Error", std::string("Could not find content: \"") + contentName + std::string("\" of type: \"") + Content::getTypeAsString(checkType) + std::string("\" inside table"));
    
    return ret;
  }

  Content* Table::getContent(std::string contentName, ContentType toCheck) {
    if (!map.hasKey(contentName))
      return NULL;
    
    Content* ret = map.getValue(contentName).value();
    if (ret->type != toCheck)
      return NULL;
    
    return ret;
  }

  void Table::print(std::ostream& out) {
    out << "{\n";

    for (int i = 0; i < map.size(); i++) {
      std::string name = map.getKey(i).value();
      Content* value = map.at(i).value();
      
      out << name << " = ";
      value->print(out);
      out << '\n';
    }

    out << '}';
  }


  // Content::Content(ContentType type, void* value) : type(type) {
  //   switch(type) {
  //     case ContentType::boolean :
  //       u.boolean = (bool*) value;
  //       break;
  //     case ContentType::string :
  //       u.string = (char*) value;
  //       break;
  //     case ContentType::array :
  //       u.array = (Array*) value;
  //       break;
  //     case ContentType::table :
  //       u.table = (Table*) value;
  //       break;
  //   }
  // }

  // void* Content::getPointer() {
  //   ContentType type = getType();

  //   switch (type) {
  //     case ContentType::array :
  //       return u.array;
      
  //     case ContentType::boolean :
  //       return u.boolean;
      
  //     case ContentType::string :
  //       return u.string;
      
  //     case ContentType::table :
  //       return u.table;
      
  //     default :
  //       return nullptr;
  //   }
  // }


  // Content* Array::getContent(int index) {
  //   if (index < 0 || index >= size)
  //     return nullptr;
    
  //   Content* ret = contents + index;
  //   if (ret->getType() == ContentType::null)
  //     return nullptr;
  //   return ret;
  // }

  // bool Array::addContent(Content* content) {
  //   if (content == nullptr)
  //     return false;
  //   if (content->getType() == ContentType::null)
  //     return false;
  //   if (size == MAX_SIZE)
  //     return false;
  //   contents[size++] = *content;
  //   return true;
  // }


  // Content* Table::getContent(std::string name) {
  //   int index = -1;
  //   for (int i = 0; i < MAX_SIZE; i++) {
  //     if (i >= getSize())
  //       break;
  //     if (names[i] == name)
  //       index = i;
  //   }

  //   if (index == -1)
  //     return nullptr;
  //   return contents.getContent(index);
  // }

  // bool Table::addContent(std::string name, Content* content) {
  //   bool ret = contents.addContent(content);
  //   if (ret)
  //     names[contents.getSize() - 1] = name;

  //   return ret;
  // }
}
