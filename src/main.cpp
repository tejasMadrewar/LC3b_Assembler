#include <bitset>
#include <iostream>

#include "assembler.h"
#include "disassembler.h"

using namespace std;

#define DEBUG 1

#if DEBUG
void test();
#endif

void runWithArgs(int argc, char **argv);

int main(int argc, char **argv) {
#if DEBUG
  test();
#else
  runWithArgs(argc, argv);
#endif
  return 0;
}

void runWithArgs(int argc, char **argv) {
  if (argc < 2) {
    cout << "LC3b assembler\nUse: " << argv[0] << " file.asm\n";
    return;
  } else {
    Assembler asssembler;
    auto asm_file = string(argv[1]);
    asssembler.assemble(asm_file);
  }
}

#if DEBUG
void test() {
  Assembler as;
  Disassembler dis;

#if 1
  // auto a1 = as.assemble("examples/sample.asm");
  //  auto a2 = as.assemble("examples/DIRECTIVE_sample.asm");
  //    cout << "Disassembly\n";
  //    dis.disassemble(a);
  //     as.assemble("examples/DIRECTIVE_sample.asm");

#if 1
  auto a = as.assemble("examples/label_address.asm");
  dis.disassemble(a);
#endif
#if 0
  auto a = as.assemble("examples/example0.asm");
  auto b = as.assemble("examples/example1.asm");
  auto c = as.assemble("examples/example2.asm");
  auto d = as.assemble("examples/sample.asm");
  auto e = as.assemble("examples/label_instructions.asm");
#endif

#if 0
  auto e1 = as.assemble("examples/errorTests/ORIG_END_errors.asm");
  auto e2 = as.assemble("examples/errorTests/no_inst.asm");
  auto e3 = as.assemble("examples/errorTests/multipleORIG.asm");
  auto e4 = as.assemble("examples/errorTests/No_END_errors.asm");
  auto e5 = as.assemble("examples/errorTests/multipleLabelErrors.asm");
  auto e6 = as.assemble("examples/errorTests/undefinedLabel.asm");
  auto e7 = as.assemble("examples/errorTests/InvalidTokenErrors.asm");
#endif
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
}
#endif
