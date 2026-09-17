#include <iostream>
#include <fstream>
#include <string>

#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <util/FileUtils.hpp>
#include <util/Argumentator.hpp>

#include <PreTokenizer/PreTokenizer.hpp>
#include <Tokenizer/Tokenizer.hpp>
#include <Parser/Parser.hpp>


#define SMOOTH_VERSION_MAJOR 0
#define SMOOTH_VERSION_MINOR 8
#define SMOOTH_VERSION_PATCH 0

#define SMOOTH_VERSION_CREATE(major, minor, patch) ((major * 1000000) + (minor * 1000) + patch)
#define SMOOTH_VERSION SMOOTH_VERSION_CREATE(SMOOTH_VERSION_MAJOR, SMOOTH_VERSION_MINOR, SMOOTH_VERSION_PATCH)

#define __EXPAND_STRINGIFY_AND_ADD_DOTS(x, y, z) __STRINGIFY(x) "." __STRINGIFY(y) "." __STRINGIFY(z)
#define SMOOTH_VERSION_PRINTABLE __EXPAND_STRINGIFY_AND_ADD_DOTS(SMOOTH_VERSION_MAJOR, SMOOTH_VERSION_MINOR, SMOOTH_VERSION_PATCH)
#define SMOOTH_VERSION_PRINTABLE_PRETTY "SmoothScript v" SMOOTH_VERSION_PRINTABLE


//TODO: add consts in all the needed code
//TODO: make use of size_t instead of int when necessary
//TODO: make vector processor and single output processor different classes
//TODO: use uints instead of ints when needed (make 0 the not correct value? or use optional?)
//TODO: free memory when done with it
//TODO: Try to not heap-allocate everything
//TODO: USE CMAKE?
//TODO: Remake the extension, but better


int main(int argc, char* argv[]) {
  char* mainFileParam;
  bool showHelp = false;

  Utils::Argumentator argumentator(argc, argv);
  argumentator
    .requiredValue(mainFileParam, "mainFile")
    ->optionalFlag(showHelp, "-h")
    ->optionalFlag(showHelp, "--help")
    ->process();
  
  if (showHelp) {
    argumentator.printUsage();
    return 0;
  }

  argumentator.checkRequiredParams();
  std::string mainFile = std::string(mainFileParam);
  std::cout << SMOOTH_VERSION_PRINTABLE_PRETTY << "\n\n";

  if (!Utils::fileExists(mainFile))
    Utils::error("File error", std::string("File: \"") + mainFile + std::string("\" does not exist"));
  std::cout << "Reading file...\n";
  std::string file = Utils::readEntireFile(mainFile);
  Utils::setErrorFileName(mainFile);


  std::cout << "\nPreTokenizing...\n";
  PreTokenizer::PreTokenizer preTokenizer(file);
  preTokenizer.process();
  std::cout << "\nPrinting PreTokens...\n";
  preTokenizer.print(std::cout);

  std::cout << "\nTokenizing...\n";
  Tokenizer::Tokenizer tokenizer(preTokenizer.getOutput());
  tokenizer.process();
  std::cout << "\nPrinting Tokens...\n";
  tokenizer.print(std::cout);

  std::cout << "\nParsing...\n";
  Parser::Parser parser(tokenizer.getOutput());
  parser.process();
  std::cout << "\nPrinting Parser Output...\n";
  parser.print(std::cout);
  
  std::cout << "\n\nDone.\n";
  return 0;
}
