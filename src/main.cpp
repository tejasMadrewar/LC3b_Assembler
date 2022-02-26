#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char **argv) {

  Assembler as;
  Disassembler dis;

#if 1
  auto a1 = as.assemble("examples/sample.asm");
  // auto a2 = as.assemble("examples/DIRECTIVE_sample.asm");
  //   cout << "Disassembly\n";
  //   dis.disassemble(a);
  //    as.assemble("examples/DIRECTIVE_sample.asm");

  // auto a = as.assemble("examples/example0.asm");
  // auto b = as.assemble("examples/example1.asm");
  // auto c = as.assemble("examples/example2.asm");
  //  auto d = as.assemble("examples/code.asm");

  // auto e1 = as.assemble("examples/errorTests/ORIG_END_errors.asm");
  //  auto e2 = as.assemble("examples/errorTests/no_inst.asm");
  //  auto e3 = as.assemble("examples/errorTests/multipleORIG.asm");
  // auto e4 = as.assemble("examples/errorTests/No_END_errors.asm");
  // auto e5 = as.assemble("examples/errorTests/multipleLabelErrors.asm");
  // auto e6 = as.assemble("examples/errorTests/undefinedLabel.asm");
#else
  fstream f("examples/example2.asm");
  string buffer((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
  Tokenizer tokenizer;
  string s = "#985 #-69 #-4 #0 #1 \n # #- x \n x3490fe \"Hello world\"\n R1 R8 "
             "HALT ADD XOR\n"
             "asdf 2sadf 5555 a444\n";
  auto a = tokenizer.tokenize(buffer);
  tokenizer.printTokens(a);
#endif

  return 0;
}
