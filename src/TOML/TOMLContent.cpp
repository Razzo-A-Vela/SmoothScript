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
}
