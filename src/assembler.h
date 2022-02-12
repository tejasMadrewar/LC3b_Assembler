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

private:
private:
  void firstPass(vector<Token> &tokens);
  void secondPass(vector<Token> &tokens);

  void error(string erMsg, int line, vector<Token> &tokens);
  void expect(string expected, int location, vector<Token> &tokens,
              string erMsg);

  uint16_t parseNumber(Token t);
  Register parseRegister(Token t, vector<Token> &tokens);

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
#define e(a, b) {Opcode::a, &Assembler::assemble##a},
  unordered_map<Opcode, fptrOp2Inst> op2assembly = {OPCODE_DATA(e)};
#undef e
};
