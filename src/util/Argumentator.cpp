#include "Argumentator.hpp"

namespace Utils {
  void Argumentator::usageError(std::string msg) {
    printUsage();
    Utils::error("Usage Error", msg);
  }


  Argumentator* Argumentator::requiredValue(char*& toSet, std::string valueName) {
    requiredValues.push_back({ toSet, valueName });
    return this;
  }

  Argumentator* Argumentator::optionalFlag(bool& toSet, std::string flag) {
    optionalFlags.push_back({ toSet, flag });
    return this;
  }


  void Argumentator::printUsage() {
    std::cout << "\nUsage: " << programName << ' ';

    for (RequiredValue requiredValue : requiredValues)
      std::cout << requiredValue.valueName << ' ';

    for (OptionalFlag optionalFlag : optionalFlags)
      std::cout << '[' << optionalFlag.flag << ']' << ' ';
    std::cout << "\n\n";
  }

  void Argumentator::checkRequiredParams() {
    if (requiredIndex < requiredValues.size())
      usageError("Expected param: " + requiredValues[requiredIndex].valueName);
  }

  void Argumentator::process() {
    consume();  // To remove the first argument

    while (hasPeek()) {
      char* arg = consume().value();

      for (OptionalFlag flag : optionalFlags) {
        if (arg == flag.flag) {
          flag.toSet = true;
          goto whileContinue;
        }
      }
      
      if (requiredIndex >= requiredValues.size())
        usageError("Unexpected param: " + std::string(arg));
      requiredValues[requiredIndex++].toSet = arg;
      
    whileContinue:
      int unused = 0;  // Added this line to make the compiler shut up
    }
  }
}
