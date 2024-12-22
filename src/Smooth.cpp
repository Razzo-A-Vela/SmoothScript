#include <iostream>
#include <fstream>

#include <util/StringUtils.hpp>
#include <TOML/TOML.hpp>
#include <PreTokenizer/PreTokenizer.hpp>
#include <Tokenizer/Tokenizer.hpp>
#include <PreProcessor/PreProcessor.hpp>
#include <Parser/Parser.hpp>
#include <Generator/Generator.hpp>

const std::string defaultConfigTOMLName = "smoothConfig.toml";
const std::string defaultConfigTOML = "\n\
[files]\n\
# all paths are relative to smooth.exe\n\
# main file(.smt) path (without extension)\n\
mainFile = \"\"\n\
# out file path (without extension)\n\
outFile = \"\"\n\
\n\
[options]\n\
# should generate assembly\n\
genAssembly = false\n\
# should not assemble and link (genAssembly required)\n\
noLink = false\n\
\n\
[commands]\n\
# assembler, linker and cleanup commands\n\
assembleCommand = \"\"\n\
linkCommand = \"\"\n\
cleanupCommand = \"\"\n\
";

std::string getReplacedCommand(std::string toReplace, std::string mainFile, std::string outFile) {
  return Utils::replace(Utils::replace(toReplace, "$mainFile$", mainFile), "$outFile$", outFile);
}

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
  TOML::Table* options = config->getContentOrError("options")->u.table;
  TOML::Table* commands = config->getContentOrError("commands")->u.table;

  files->setCheckType(TOML::ContentType::STRING);
  std::string mainFile = std::string(files->getContentOrError("mainFile")->u.string);
  std::string outFile = std::string(files->getContentOrError("outFile")->u.string);

  options->setCheckType(TOML::ContentType::BOOL);
  bool genAssembly = options->getContentOrError("genAssembly")->u.boolean;
  bool noLink = options->getContentOrError("noLink")->u.boolean;
  
  commands->setCheckType(TOML::ContentType::STRING);
  std::string assembleCommand = std::string(commands->getContentOrError("assembleCommand")->u.string);
  std::string linkCommand = std::string(commands->getContentOrError("linkCommand")->u.string);
  std::string cleanupCommand = std::string(commands->getContentOrError("cleanupCommand")->u.string);


  if (!genAssembly && noLink)
    Utils::error("Config Error", "genAssembly is required for noLink");

  if (mainFile == "")
    Utils::error("Config Error", "mainFile must not be empty");

  if (outFile == "")
    Utils::error("Config Error", "outFile must not be empty");
  Utils::resetErrorFileName();

  std::string mainFileName = mainFile + ".smt";
  if (!Utils::fileExists(mainFileName))
    Utils::error("File error", std::string("File: \"") + mainFileName + std::string("\" does not exist"));
  std::cout << "Reading file...\n";
  std::string file = Utils::readEntireFile(mainFileName);
  Utils::setErrorFileName(mainFileName);

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


  std::cout << "\nPreProcessing...\n";
  PreProcessor::PreProcessor preProcessor(tokenizer.getOutput());
  preProcessor.process();
  std::cout << "\nPrinting PreProcessed Tokens...\n";
  preProcessor.print(std::cout);  


  std::cout << "\nParsing...\n";
  Parser::Parser parser(preProcessor.getOutput());
  parser.process();
  std::cout << "\nPrinting Parse Tree...\n";
  parser.print(std::cout);

  Utils::resetErrorFileName();
  std::cout << "\nGenerating...\n";
  Generator::Generator generator(parser.getOutput());
  generator.process();
  
  std::string outFileName = outFile + ".asm";
  if (genAssembly)
    std::cout << "\nGenerated assembly to: " << outFileName << '\n';
  std::ofstream outFileStream;
  outFileStream.open(outFileName);

  generator.print(outFileStream);
  outFileStream.close();

  // if noLink = true -> genAssembly = true
  if (noLink) {
    std::cout << "\nDone.\n";
    return 0;
  }


  std::cout << "\nAssembling...\n";
  std::cout << "\nRunning commands from config...\n";
  const char* cCommands[] = { assembleCommand.c_str(), linkCommand.c_str(), cleanupCommand.c_str() };
  for (int i = 0; i < 3; i++) {
    std::string command = std::string(cCommands[i]);
    command = getReplacedCommand(command, mainFile, outFile);
    if (command != "") {
      std::cout << command << '\n';
      system(command.c_str());
    } else
      std::cout << '\n';  // I'm going to make the user think I run the command
  }

  if (!genAssembly)
    remove(("./" + outFileName).c_str());
  
  std::cout << "\nDone.\n";
  return 0;
}
