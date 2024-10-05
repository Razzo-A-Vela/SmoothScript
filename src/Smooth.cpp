#include <iostream>

#include <util/StringUtils.hpp>
#include <TOML/TOML.hpp>
#include <PreTokenizer/PreTokenizer.hpp>
#include <Tokenizer/Tokenizer.hpp>
#include <PreProcessor/PreProcessor.hpp>

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
# should assemble and link (genAssembly required)\n\
noLink = false\n\
\n\
# arguments for gcc assembler and linker (ex. [\"-nostdlib\", \"-lSDL_2\"])\n\
gccArgs = []\n\
";

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

  if (!Utils::fileExists(mainFile + ".smt"))
    Utils::error("File error", "File does not exist");

  std::cout << "\nReading file...\n";
  std::string file = Utils::readEntireFile(mainFile + ".smt");

  std::cout << "\nPreTokenizing...\n";
  PreTokenizer::PreTokenizer preTokenizer(file);
  preTokenizer.process();
  std::cout << "\nPrinting PreTokens...\n";
  preTokenizer.print();

  std::cout << "\nTokenizing...\n";
  Tokenizer::Tokenizer tokenizer(preTokenizer.getOutput());
  tokenizer.process();
  std::cout << "\nPrinting Tokens...\n";
  tokenizer.print();

  std::cout << "\nPreProcessing...\n";
  PreProcessor::PreProcessor preProcessor(tokenizer.getOutput());
  preProcessor.process();
  std::cout << "\nPrinting PreProcessed Tokens...\n";
  preProcessor.print();
}
