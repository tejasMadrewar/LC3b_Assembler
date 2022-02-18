#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"
#include "opcode_data.h"
#include "tokenizer.h"

using namespace std;

int main(int argc, char **argv) {

  Assembler as;
  Disassembler dis;

#if 1
  as.assemble("examples/sample.asm");
  // as.assemble("examples/DIRECTIVE_sample.asm");
#else

#endif

  return 0;
}
