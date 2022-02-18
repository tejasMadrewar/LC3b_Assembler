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

  inline bool isRegValid(Register r) { return (r >= R0 and r <= R7); }

  inline string stringOpNum(string &op, uint16_t num) {
    return op + " " + std::to_string(num);
  }

  inline string stringOpRegReg(string &op, Register r1, Register r2) {
    return op + " " + reg2str[r1] + ", " + reg2str[r2];
  }

  inline string stringOpRegNum(string &op, Register r1, uint16_t num) {
    return op + " " + reg2str[r1] + ", " + std::to_string(num);
  }

  inline string stringOpRegRegNum(string &op, Register r1, Register r2,
                                  uint16_t num) {
    return op + " " + reg2str[r1] + ", " + reg2str[r2] + ", " +
           std::to_string(num);
  }

  string disInst(instruction i) {

    auto opSearch = hex2op.find(i.OP);
    if (opSearch == hex2op.end()) {
      cout << "Unknown instruction" << hex << i.i << "\n";
      exit(0);
    }
    auto op = opSearch->second;
    // auto dis = opcode2dis.find(o);

    switch (op) {
    case ADD:
      return disADD(i);
    case AND:
      return disAND(i);
    case BR:
      return disBR(i);
    case JMP:
    case RET:
      return disJMP(i);
    case JSR:
    case JSRR:
      return disJSR(i);
    case LDB:
      return disLDB(i);
    case LDW:
      return disLDW(i);
    case LEA:
      return disLEA(i);
    case RTI:
      return disRTI(i);
    case LSHF:
    case RSHFL:
    case RSHFA:
      return disSHF(i);
    case STB:
      return disSTB(i);
    case STW:
      return disSTW(i);
    case TRAP:
      return disTRAP(i);
    case XOR:
    case NOT:
      return disXOR(i);
    }

    return "Not implemented";
  }

private:
  string disADD(instruction i);
  string disAND(instruction i);
  string disBR(instruction i);
  string disJMP(instruction i);
  string disJSR(instruction i);
  string disLDB(instruction i);
  string disLDW(instruction i);
  string disLEA(instruction i);
  string disRTI(instruction i);
  string disSHF(instruction i);
  string disSTB(instruction i);
  string disSTW(instruction i);
  string disTRAP(instruction i);
  string disXOR(instruction i);
};
