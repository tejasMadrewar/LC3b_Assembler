#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "instruction.h"
#include "opcode_data.h"

using namespace std;

template <typename T> std::string int_to_hex(T i) {
  std::stringstream stream;
  stream << "0x" << std::hex << i;
  return stream.str();
}

ostream &operator<<(ostream &os, const instruction &i);

class Disassembler {

public:
  typedef string (Disassembler::*disFun)(instruction i);
  typedef Register Reg;

  void disassemble(vector<instruction> program);
  string disInst(instruction i);

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
  inline bool isRegValid(Reg r) { return (r >= R0 and r <= R7); }

  inline string stringOpN(const string &op, uint16_t num) {
    return op + " " + int_to_hex(num);
  }

  inline string stringOpR(const string &op, Reg r1) {
    return op + " " + reg2str[r1];
  }

  inline string stringOpRR(const string &op, Reg r1, Reg r2) {
    return op + " " + reg2str[r1] + ", " + reg2str[r2];
  }

  inline string stringOpRRR(const string &op, Reg r1, Reg r2, Reg r3) {
    return op + " " + reg2str[r1] + ", " + reg2str[r2] + ", " + reg2str[r3];
  }

  inline string stringOpRN(const string &op, Reg r1, uint16_t num) {
    return op + " " + reg2str[r1] + ", " + int_to_hex(num);
  }

  inline string stringOpRRN(const string &op, Reg r1, Reg r2, uint16_t num) {
    return op + " " + reg2str[r1] + ", " + reg2str[r2] + ", " + int_to_hex(num);
  }

private:
  static const unordered_map<uint32_t, string> mask2br; // = {BR_MASK(x)};
};
