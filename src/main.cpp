#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char **argv) {

  Assembler as;
  Disassembler dis;

#if 1
  auto a = as.assemble("examples/sample.asm");
  // auto a = as.assemble("examples/DIRECTIVE_sample.asm");
  // cout << "Disassembly\n";
  // dis.disassemble(a);
  //  as.assemble("examples/DIRECTIVE_sample.asm");
#else
  Tokenizer tokenizer;
  auto a = as.assemble("examples/sample.asm");
  fstream f("examples/sample.asm");
  string buffer((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
  auto tokens = tokenizer.tokenize(buffer);
  tokenizer.printTokens(tokens);

#endif

  return 0;
}
