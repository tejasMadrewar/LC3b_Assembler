#include "assembler.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctype.h>

#include <bitset>

using namespace std;

#define DEBUG_LOG(OPCODE) cout << "OPCODE -> " << bitset<16>(i.i) << "\n";

ostream &operator<<(ostream &os, Token &t) {
  os << t.lexme;
  return os;
}

vector<instruction> Assembler::assemble(string filename) {
  ifstream file(filename);
  // remove previous data
  binaryData.clear();
  symbolTable.clear();
  Tokenizer tokenizer;

  // read file
  if (file.is_open()) {
    string buffer((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());

    // tokenize
    auto tokens = tokenizer.tokenize(buffer);
    tokenizer.printTokens(tokens);

    // run all passes
    firstPass(tokens);
    secondPass(tokens);
  } else {
    cout << "Unable to open file\n";
  }

  return binaryData;
}

void Assembler::error(string message, int line, vector<Token> &tokens) {

  cout << "\nError: " << message;
  cout << " at line:" << line << "\n";
  // print that line
  for (auto t : tokens)
    if (t.line == line)
      cout << t.lexme << " ";

  exit(1);
}

void Assembler::expect(string expected, int location, vector<Token> &tokens,
                       string erMsg) {
  const auto &t = tokens[location];
  if (t.lexme != expected) {
    error(erMsg, t.line, tokens);
  }
}

uint16_t Assembler::parseNumber(Token t) {
  uint16_t number = UINT16_MAX;

  if (t.lexme[0] == 'x') {
    // hex
    auto s = t.lexme.length();
    auto str = t.lexme.substr(1, s - 1);
    // cout << s << " " << str << "\n";
    number = strtoul(str.c_str(), NULL, 16);
  } else if (t.lexme[0] == '#') {
    // decimal
    auto s = t.lexme.length();
    auto str = t.lexme.substr(1, s - 1);
    number = strtoul(str.c_str(), NULL, 10);
  }

  if (number == UINT16_MAX)
    cerr << "invalid number " << t << "\n";

  return number;
}

Register Assembler::parseRegister(Token t, vector<Token> &tokens) {
  Register r = R1;

  auto f = str2reg.find(t.lexme);
  if (f != str2reg.end()) {
    r = f->second;
  } else {
    error("Expected register at ", t.line, tokens);
  }
  return r;
}

void Assembler::firstPass(vector<Token> &tokens) {

  for (int i = 0; i < tokens.size(); i++) {
    auto t = tokens[i];

    // check for opcode
    auto f = str2op.find(t.lexme);
    if (f != str2op.end()) {
      cout << "found " << f->first << "\n";
      auto inst = opcode2instruction(i, tokens);
      binaryData.push_back(inst);
    } else {
      // add to symbol Table
      symbolTable[t.lexme] = binaryData.size();
    }
  }
}

instruction Assembler::opcode2instruction(int &location,
                                          vector<Token> &tokens) {

  instruction i;
  auto token = tokens[location];
  auto f = str2op.find(token.lexme);

  if (f != str2op.end()) {
    auto op = f->second;
#define d(x, y)                                                                \
  case Opcode::x:                                                              \
    i = assemble##x(location, tokens);                                         \
    break;
    switch (op) {
      OPCODE_DATA(d)
    default:
      cout << "Instruction not implemneted\n";
    }
#undef d
  }
  return i;
}

void Assembler::secondPass(vector<Token> &tokens) {}

instruction Assembler::assembleADD(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleAND(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;

  auto t = tokens[loc];
  if (t.lexme == "AND") {
    i.OP = op2hex.at(AND);
    loc++;
    // first register
    auto r2 = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // second register
    auto r3 = parseRegister(tokens[loc], tokens);
    i.SR1 = reg2hex[r3];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(tokens[loc]);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto r4 = parseRegister(tokens[loc], tokens);
      i.SR2 = reg2hex[r4];
      i.b5 = false;
      i.b4 = false;
      i.b3 = false;
    }
  }

  DEBUG_LOG(AND)
  return i;
}

instruction Assembler::assembleBR(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleJMP(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleJSR(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleJSRR(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleLDB(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleLDW(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "LDW") {
    i.OP = op2hex.at(LDW);
    loc++;
    // first register
    auto dr = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // second register
    auto baseR = parseRegister(tokens[loc], tokens);
    i.BaseR = reg2hex[baseR];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // third argument
    auto offset6 = parseNumber(tokens[loc]);
    if (offset6 != UINT16_MAX) {
      i.offset6 = offset6 & 0b111111;
    }
  }
  // error("Not Implemnted", tokens[location].line, tokens);
  DEBUG_LOG(LDW)
  return i;
}

instruction Assembler::assembleLEA(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "LEA") {
    i.OP = op2hex.at(LEA);
    loc++;
    // first register
    auto dr = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;
    // third argument is symbolic writed in second pass
    i.PCoffset9 = 0b111111111;
  }

  // error("Not Implemnted", tokens[location].line, tokens);
  DEBUG_LOG(LEA)
  return i;
}

instruction Assembler::assembleNOT(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleRET(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleRTI(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleLSHF(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleRSHFL(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleRSHFA(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleSTB(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleSTW(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}

instruction Assembler::assembleTRAP(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "TRAP") {
    i.OP = op2hex.at(TRAP);
    loc++;
    // first number
    auto trapvect8 = parseNumber(tokens[loc]);
    i.trapvect8 = trapvect8 & 0b11111111;
    i.b8 = false;
    i.b9 = false;
    i.b10 = false;
    i.b11 = false;
  }

  DEBUG_LOG(TRAP)
  return i;
}

instruction Assembler::assembleXOR(int &location, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  error("Not Implemnted", tokens[location].line, tokens);
  return i;
}
