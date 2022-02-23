#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char **argv) {

  Assembler as;
  Disassembler dis;

#if 0
  auto a = as.assemble("examples/sample.asm");
  // auto a = as.assemble("examples/DIRECTIVE_sample.asm");
  // cout << "Disassembly\n";
  // dis.disassemble(a);
  //  as.assemble("examples/DIRECTIVE_sample.asm");

#else
  auto a = as.assemble("examples/example0.asm");
  auto b = as.assemble("examples/example1.asm");
  auto c = as.assemble("examples/example2.asm");
#endif

  return 0;
}
