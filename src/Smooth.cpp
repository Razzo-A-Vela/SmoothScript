#include <iostream>

#include <TOML/TOML.hpp>
#include <util/StringUtils.hpp>

const std::string configTOMLName = "smoothConfig.toml";
const std::string defaultConfigTOML = "\n\
# all paths are relative to smooth.exe\n\
# main file path\n\
mainFile = \"\"\n\
# out file path (without extension)\n\
outFile = \"\"\n\
\n\
# should generate assembly\n\
genAssembly = false\n\
# should assemble and link (genAssembly required)\n\
noLink = false\n\
\n\
# path for files to be included\n\
includes = []\n\
\n\
# arguments for gcc assembler and linker (ex. [\"-nostdlib\", \"-lSDL_2\"])\n\
gccArgs = []\n\
";

int main(int argc, char* argv[]) {
  TOML::File config(configTOMLName);
  config.init(defaultConfigTOML);
  std::cout << "Printing...\n";
  config.print();
}
