#include <iostream>

#include <TOML/TOML.hpp>
#include <util/StringUtils.hpp>

const std::string configTOMLName = "smoothConfig.toml";
const std::string defaultConfigTOML = "\n\
# main file name\n\
mainFile = \"\"\n\
# out file name (without extension)\n\
outFile = \"\"\n\
\n\
# should generate assembly\n\
genAssembly = false\n\
# should assemble and link (genAssembly required)\n\
noLink = false\n\
\n\
# files to include (relative to smooth.exe)\n\
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
