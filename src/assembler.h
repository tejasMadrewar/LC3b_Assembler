#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "instruction.h"
#include "opcode_data.h"
#include "tokenizer.h"

class Assembler {
public:
  vector<instruction> assemble(string filename);
  vector<instruction> assembleBuffer(string &buffer);

private:
  void firstPass();
  void secondPass();

  void error(string erMsg, int line);
  void expect(string expected, int location, string erMsg);

  uint16_t parseNumber(Token t);
  Register parseRegister(Token t);
  string parseString(Token t);
  void parseRegRegNum(int &loc, Register &r1, Register &r2, uint16_t &num);

  instruction opcode2instruction(int &location);

  instruction assembleADD(int &location);
  instruction assembleAND(int &location);
  instruction assembleBR(int &location);
  instruction assembleJMP(int &location);
  instruction assembleJSR(int &location);
  instruction assembleJSRR(int &location);
  instruction assembleLDB(int &location);
  instruction assembleLDW(int &location);
  instruction assembleLEA(int &location);
  instruction assembleNOT(int &location);
  instruction assembleRET(int &location);
  instruction assembleRTI(int &location);
  instruction assembleLSHF(int &location);
  instruction assembleRSHFL(int &location);
  instruction assembleRSHFA(int &location);
  instruction assembleSTB(int &location);
  instruction assembleSTW(int &location);
  instruction assembleTRAP(int &location);
  instruction assembleXOR(int &location);

private:
  typedef instruction (Assembler::*fptrOp2Inst)(int &location);
  vector<instruction> binaryData;
  unordered_map<string, int> symbolTable;
  Tokenizer tokenizer;
  vector<Token> tokens = {};

  vector<std::pair<int, Opcode>> patchLocations;
  vector<directiveInfo> directives;
  unordered_map<Opcode, uint32_t> labelInst2mask = {
      {Opcode::JSR, 0b11111111111},
      {Opcode::LEA, 0b111111111},
      {Opcode::BR, 0b111111111},
  };
};
