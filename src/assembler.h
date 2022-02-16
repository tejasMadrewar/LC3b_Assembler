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

#define BR_MASK(d)                                                             \
  d(BR, 0b000) d(BRn, 0b100) d(BRz, 0b010) d(BRp, 0b001) d(BRzp, 0b011)        \
      d(BRnp, 0b101) d(BRnz, 0b110) d(BRnzp, 0b111)

class Assembler {
public:
  vector<instruction> assemble(string filename);
  vector<instruction> assembleBuffer(string &buffer);

private:
private:
  void firstPass(vector<Token> &tokens);
  void secondPass(vector<Token> &tokens);

  void error(string erMsg, int line, vector<Token> &tokens);
  void expect(string expected, int location, vector<Token> &tokens,
              string erMsg);

  uint16_t parseNumber(Token t);
  Register parseRegister(Token t, vector<Token> &tokens);
  string parseString(Token t, vector<Token> &tokens);

private:
  instruction opcode2instruction(int &location, vector<Token> &tokens);
  typedef instruction (Assembler::*fptrOp2Inst)(int &location,
                                                vector<Token> &tokens);

  instruction assembleADD(int &location, vector<Token> &tokens);
  instruction assembleAND(int &location, vector<Token> &tokens);
  instruction assembleBR(int &location, vector<Token> &tokens);
  instruction assembleJMP(int &location, vector<Token> &tokens);
  instruction assembleJSR(int &location, vector<Token> &tokens);
  instruction assembleJSRR(int &location, vector<Token> &tokens);
  instruction assembleLDB(int &location, vector<Token> &tokens);
  instruction assembleLDW(int &location, vector<Token> &tokens);
  instruction assembleLEA(int &location, vector<Token> &tokens);
  instruction assembleNOT(int &location, vector<Token> &tokens);
  instruction assembleRET(int &location, vector<Token> &tokens);
  instruction assembleRTI(int &location, vector<Token> &tokens);
  instruction assembleLSHF(int &location, vector<Token> &tokens);
  instruction assembleRSHFL(int &location, vector<Token> &tokens);
  instruction assembleRSHFA(int &location, vector<Token> &tokens);
  instruction assembleSTB(int &location, vector<Token> &tokens);
  instruction assembleSTW(int &location, vector<Token> &tokens);
  instruction assembleTRAP(int &location, vector<Token> &tokens);
  instruction assembleXOR(int &location, vector<Token> &tokens);

private:
  vector<instruction> binaryData;
  unordered_map<string, int> symbolTable;
  Tokenizer tokenizer;
#define e(a, b) {Opcode::a, &Assembler::assemble##a},
  unordered_map<Opcode, fptrOp2Inst> op2assembly = {OPCODE_DATA(e)};
#undef e

  vector<std::pair<int, Opcode>> patchLocations;
  vector<directiveInfo> directives;
  unordered_map<Opcode, uint32_t> labelInst2mask = {
      {Opcode::JSR, 0b11111111111},
      {Opcode::LEA, 0b111111111},
      {Opcode::BR, 0b111111111},
  };
};
