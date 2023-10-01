#include <string>
#include <fstream>
#include <sstream>

#include "util.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "generator.hpp"


int main(int argc, char* argv[]) {
  std::string filename;
  bool outAsm = false;
  bool noLink = false;
  if (argc < 2) err("Expected filename\n\nUsage: smooth <filename> [-asm [-noLink]]\n");
  
  if (argc >= 3)
    outAsm = std::string(argv[2]) == "-asm";
  if (argc >= 4)
    noLink = std::string(argv[3]) == "-noLink";
  filename = argv[1];

  std::string contents;
  {
    std::fstream fileStream(filename, std::ios::in);
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
