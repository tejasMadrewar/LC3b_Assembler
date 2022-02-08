#include <bitset>
#include <iostream>

#include "disassembler.h"
#include "opcode_data.h"

using namespace std;

int main(int argc, char **argv) {

  disassembler d;

  cout << bitset<4>((1 << 3) - 1) << "\n";

#define p(a, b) cout << #a << "\t" << bitset<4>(b) << "\n";
  opcode_data(p)
#undef p

      return 0;
}
