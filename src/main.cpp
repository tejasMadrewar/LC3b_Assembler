#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"

using namespace std;

void runWithArgs(int argc, char **argv);

int main(int argc, char **argv) {
  runWithArgs(argc, argv);
  return 0;
}

void runWithArgs(int argc, char **argv) {
  if (argc < 2) {
    cout << "LC3b assembler\nUse: " << argv[0] << " file.asm\n";
  } else {
    Assembler asssembler;
    auto asm_file = string(argv[1]);
    asssembler.assemble(asm_file);
  }
}
