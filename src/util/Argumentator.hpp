#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <util/Processor.hpp>
#include <util/ErrorUtils.hpp>

namespace Utils {
  class Argumentator : public Processor<char*, void> {
  protected:
    struct RequiredValue {
      char*& toSet;
      std::string valueName;
    };

    struct OptionalFlag {
      bool& toSet;
      std::string flag;
    };

    int argc;
    char** argv;
    std::string programName;
    int requiredIndex = 0;
    std::vector<RequiredValue> requiredValues;
    std::vector<OptionalFlag> optionalFlags;

  public:
    Argumentator(int argc, char* argv[]);

    Argumentator* requiredValue(char*& toSet, std::string valueName);
    Argumentator* optionalFlag(bool& toSet, std::string flag);

    void printUsage();
    void checkRequiredParams();
    virtual void process();
  
  protected:
    virtual char* get(int index) { return argv[index]; }
    [[noreturn]] void usageError(std::string msg);

  };
}
