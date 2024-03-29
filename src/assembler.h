#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "errors.h"
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
  void errorChecks();
  void writeToFile(vector<instruction> data, string filename);
  void writeToHex(vector<instruction> &data, string &filename);
  void writeToBin(vector<instruction> &data, string &filename);
  void writeToObj(vector<instruction> data, string &filename);
  void writeToDebug(vector<instruction> data, string &filename);
  void writeSymbolTable(unordered_map<string, int> symTab, string filename);

  void error(string erMsg, int location);
  void error(string erMsg);
  void expect(string expected, int location, string erMsg);

  uint16_t parseNumber(int location);
  Register parseRegister(int location);
  string parseString(int location);
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
  instruction assembleNOP(int &location);

  vector<instruction> directive2instructions(int &location);

  vector<instruction> assembleORIG(int &location);
  vector<instruction> assembleEND(int &location);
  vector<instruction> assembleFILL(int &location);
  vector<instruction> assembleBLKW(int &location);
  vector<instruction> assembleSTRINGZ(int &location);

private:
  typedef instruction (Assembler::*fptrOp2Inst)(int &location);
  vector<instruction> binaryData;
  unordered_map<string, int> symbolTable;
  Tokenizer tokenizer;
  vector<Token> tokens = {};

  struct instLabelData {
    int offset;
    int location;
  };
  vector<instLabelData> patchLocations;
  vector<directiveInfo> directives;
  vector<ErrorInfo> errors;
  unordered_map<int, int> offset2line;
  static const unordered_map<Opcode, uint32_t> labelInst;
};
