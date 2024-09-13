#include <string>
#include <fstream>
#include <sstream>

#include "legacy/util.hpp"
#include "legacy/tokenizer.hpp"
#include "legacy/parser.hpp"
#include "legacy/generator.hpp"


std::string getUsage() {
  return "\nUsage: smooth <filename> [-asm [-noLink]]\n";
}

int main(int argc, char* argv[]) {
  std::string fileName;
  bool outAsm = false;
  bool noLink = false;
  if (argc < 2) err("Expected filename\n" + getUsage());
  
  if (argc >= 3)
    outAsm = std::string(argv[2]) == "-asm";
  if (argc >= 4)
    noLink = std::string(argv[3]) == "-noLink";
  fileName = argv[1];

  if (fileName.substr(fileName.size() - 4, fileName.size() - 1) != ".smt") err("Expected .smt file\n" + getUsage());

  std::string contents;
  {
    std::fstream fileStream(fileName, std::ios::in);
    std::stringstream stream;
    stream << fileStream.rdbuf();
    contents = stream.str();
  }

  Tokenizer tokenizer(contents);
  Parser parser(tokenizer.tokenize());
  Generator generator(parser.parse());

  // Optimizer optimizer(generator.generate());
  // std::string output = optimizer.optimize();

  std::string output = generator.generate();

  {
    std::fstream fileStream("out.asm", std::ios::out);
    fileStream.write(output.c_str(), output.length());
  }

  if (noLink) return EXIT_SUCCESS;
  system("nasm -fwin64 out.asm");
  system("gcc out.obj -o out.exe");
  system("del out.obj");
  if (!outAsm) system("del out.asm");
  return EXIT_SUCCESS;
}
