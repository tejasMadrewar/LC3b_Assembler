#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"
#include "opcode_data.h"

using namespace std;

int main(int argc, char **argv) {

  Assembler as;
  Disassembler dis;

  as.assemble("examples/sample.asm");

  return 0;
}
