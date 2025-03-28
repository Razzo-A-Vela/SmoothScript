#include <iostream>
#include <fstream>

#include <util/ErrorUtils.hpp>
#include <util/StringUtils.hpp>
#include <TOML/TOML.hpp>
#include <PreTokenizer/PreTokenizer.hpp>
#include <Tokenizer/Tokenizer.hpp>

const std::string defaultConfigTOMLName = "smoothConfig.toml";
const std::string defaultConfigTOML = "\n\
[files]\n\
# all paths are relative to smooth.exe\n\
# main file path\n\
mainFile = \"\"\n\
# out file path (without extension)\n\
outFile = \"\"\n\
outFileExt = \"\"\n\
\n\
[commands]\n\
# run commands (can use $outFile$ to replace with outFile)\n\
run = false\n\
list = []\n\
";

//TODO: make use of size_t instead of int when necessary

// std::string getReplacedCommand(std::string toReplace, std::string outFile) {
//   return Utils::replace(toReplace, "$outFile$", outFile);
// }

int main(int argc, char* argv[]) {
  std::string configTOMLName;

  if (argc == 1)
    configTOMLName = std::string(defaultConfigTOMLName);
  else if (argc == 2) 
    configTOMLName = std::string(argv[1]);
  else
    Utils::error("Usage Error", "Expected only one argument.\n Usage: smooth [configTOMLName]");

  std::cout << "\nReading config...\n";
  Utils::setErrorFileName(configTOMLName);
  //! TOML DEBUG CODE
  // TOML::Tokenizer tomlTokenizer = TOML::Tokenizer(Utils::readEntireFile(configTOMLName));
  // tomlTokenizer.process();
  // std::cout << "TOML TOKENS:\n";
  // tomlTokenizer.print(std::cout);
  // std::cout << '\n';

  // TOML::Parser tomlParser = TOML::Parser(tomlTokenizer.getOutput());
  // tomlParser.process();
  // TOML::Table* config = tomlParser.getSingleOutput();
  TOML::Parser* tomlParser = TOML::Parser::readOrCreate(configTOMLName, defaultConfigTOML);
  TOML::Table* config = tomlParser->getSingleOutput();
  std::cout << "\nPrinting config...\n";
  config->print(std::cout);
  std::cout << '\n';

  config->setCheckType(TOML::ContentType::TABLE);
  TOML::Table* files = config->getContentOrError("files")->u.table;
  // TOML::Table* commands = config->getContentOrError("commands")->u.table;

  files->setCheckType(TOML::ContentType::STRING);
  std::string mainFile = std::string(files->getContentOrError("mainFile")->u.string);
  std::string outFile = std::string(files->getContentOrError("outFile")->u.string);
  std::string outFileExt = std::string(files->getContentOrError("outFileExt")->u.string);
  std::string outFileName = outFile + outFileExt;

  // commands->setCheckType(TOML::ContentType::BOOL);
  // bool runCommands = commands->getContentOrError("run")->u.boolean;
  // commands->setCheckType(TOML::ContentType::LIST);
  // TOML::List* commandsList = commands->getContentOrError("list")->u.list;
  // commandsList->setCheckType(TOML::ContentType::STRING);

  if (mainFile == "")
    Utils::error("Config Error", "mainFile must not be empty");
  
  if (outFile == "")
    Utils::error("Config Error", "outFile must not be empty");
  Utils::resetErrorFileName();

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

  // if (runCommands) {
  //   std::cout << "\nRunning commands from config...\n";

  //   for (int i = 0; i < commandsList->list.size(); i++) {
  //     std::string command = std::string(commandsList->getContentOrError(i)->u.string);

  //     if (command != "") {
  //       std::cout << command << '\n';
  //       system(command.c_str());
  //     } else
  //       std::cout << '\n';  // I'm going to make the user think I run the command
  //   }
  // }
    
  // std::cout << "\nDone.\n";
  return 0;
}
