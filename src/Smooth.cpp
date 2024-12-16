#include <iostream>
#include <fstream>

#include <util/StringUtils.hpp>
#include <TOML/TOML.hpp>
#include <PreTokenizer/PreTokenizer.hpp>
#include <Tokenizer/Tokenizer.hpp>
#include <PreProcessor/PreProcessor.hpp>
#include <Parser/Parser.hpp>
#include <Generator/Generator.hpp>

const std::string configTOMLName = "smoothConfig.toml";
const std::string defaultConfigTOML = "\n\
# all paths are relative to smooth.exe\n\
# main file(.smt) path (without extension)\n\
mainFile = \"\"\n\
# out file path (without extension)\n\
outFile = \"\"\n\
\n\
# should generate assembly\n\
genAssembly = false\n\
# should not assemble and link (genAssembly required)\n\
noLink = false\n\
\n\
# assembler, linker and cleanup commands\n\
assembleCommand = ""\n\
linkCommand = ""\n\
cleanupCommand = ""\n\
\n\
";

std::string getReplaced(std::string toReplace, std::string mainFile, std::string outFile) {
  return Utils::replace(Utils::replace(toReplace, "$mainFile$", mainFile), "$outFile$", outFile);
}

int main(int argc, char* argv[]) {
  std::cout << "\nReading config...\n";
  TOML::File config(configTOMLName);
  config.init(defaultConfigTOML);
  std::cout << "\nPrinting config...\n";
  config.print();
  std::string mainFile = std::string((char*) config.getContent("mainFile")->getPointer());
  std::string outFile = std::string((char*) config.getContent("outFile")->getPointer());
  bool genAssembly = *((bool*) config.getContent("genAssembly")->getPointer());
  bool noLink = *((bool*) config.getContent("noLink")->getPointer());
  std::string assembleCommand = std::string((char*) config.getContent("assembleCommand")->getPointer());
  std::string linkCommand = std::string((char*) config.getContent("linkCommand")->getPointer());
  std::string cleanupCommand = std::string((char*) config.getContent("cleanupCommand")->getPointer());

  if (!genAssembly && noLink)
    Utils::error("\nConfig Error", "genAssembly is required for noLink");

  if (outFile == "")
    Utils::error("\nConfig Error", "outFile must not be empty");

  std::string mainFileName = mainFile + ".smt";
  if (!Utils::fileExists(mainFileName))
    Utils::error("\nFile error", std::string("File: ") + mainFileName + std::string(" does not exist"));
  

  std::cout << "\nReading file...\n";
  std::string file = Utils::readEntireFile(mainFileName);

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
  const char* commands[] = { assembleCommand.c_str(), linkCommand.c_str(), cleanupCommand.c_str() };
  for (int i = 0; i < 3; i++) {
    std::string command = std::string(commands[i]);
    command = getReplaced(command, mainFile, outFile);
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
