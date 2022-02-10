#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "instruction.h"
#include "opcode_data.h"

using namespace std;

class Disassembler {
public:
  typedef string (Disassembler::*disFun)(instruction i);

  void disassemble(vector<instruction> program) {
    for (auto i : program) {
      cout << disInst(i) << "\n";
    }
  }

  string disInst(instruction i) {

    Opcode o = Opcode(i.OP);
    auto dis = opcode2dis.find(o);

    return "";
  }

private:
  const unordered_map<Opcode, disFun> opcode2dis = {
      {ADD, &Disassembler::disADD}};

  string disADD(instruction i) {
    string d = "ADD R" + to_string(i.DR) + ", R" + to_string(i.SR1);
    (i.b5) ? d = d + ", #" + to_string(i.IMM5)
           : d = d + ", R" + to_string(i.SR2);
    return d;
  }
};
