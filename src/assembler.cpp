#include "assembler.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctype.h>

#include <bitset>

using namespace std;

#define DEBUG_LOG(OPCODE)                                                      \
  tokenizer.printLine(loc, tokens);                                            \
  cout << "OPCODE -> 0x" << hex << i.i << " -> " << bitset<16>(i.i) << "\n\n";

ostream &operator<<(ostream &os, Token &t) {
  os << t.lexme;
  return os;
}

vector<instruction> Assembler::assembleBuffer(string &buffer) {
  // remove previous data
  binaryData.clear();
  symbolTable.clear();
  patchLocations.clear();

  // tokenize
  auto tokens = tokenizer.tokenize(buffer);
  tokenizer.printTokens(tokens);

  // run all passes
  firstPass(tokens);
  secondPass(tokens);

  return binaryData;
}

vector<instruction> Assembler::assemble(string filename) {
  ifstream file(filename);
  auto outputFile = filename + ".bin";
  ofstream ofile(outputFile);

  // remove previous data
  binaryData.clear();
  symbolTable.clear();
  patchLocations.clear();

  // read file
  if (file.is_open()) {
    cout << "Reading from " << filename << " \n";
    string buffer((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());
    assembleBuffer(buffer);
    // write data to output file
    if (ofile.is_open()) {
      cout << "Writing to " << outputFile << " \n";

      // write size
      // typename vector<instruction>::size_type size = binaryData.size();
      // ofile.write((char *)&binaryData, sizeof(size));

      // write data
      ofile.write((char *)&binaryData[0],
                  binaryData.size() * sizeof(instruction));

    } else {
      cout << "Unable to open file" << outputFile << "\n";
    }
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

  if (number == UINT16_MAX) {
    // cerr << "invalid number " << t << "\n";
  }

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
      // tokenizer.printLine(i, tokens);
      auto inst = opcode2instruction(i, tokens);
      binaryData.push_back(inst);
      // save locations to patch
      auto isInsPatched = labelInst2mask.find(f->second);
      if (isInsPatched != labelInst2mask.end()) {
        // save location
        patchLocations.push_back({binaryData.size() - 1, f->second});
      }
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

  auto opSearch = str2op.find(token.lexme);

  if (opSearch != str2op.end()) {
    // check all other instructions
    auto op = opSearch->second;
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

instruction Assembler::assembleADD(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "ADD") {
    i.OP = op2hex.at(ADD);
    loc++;
    // first argument
    auto r2 = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // second argument
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

  DEBUG_LOG(ADD)
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

instruction Assembler::assembleBR(int &loc, vector<Token> &tokens) {

#define m(a, b) {#a, b},
  const unordered_map<string, uint16_t> br2mask = {BR_MASK(m)};
#undef m

  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  auto result = br2mask.find(t.lexme);
  if (result != br2mask.end()) {
    i.OP = op2hex.at(BR);
    loc++;
    auto mask = result->second;
    i.n = (mask >> 2) & 0b1;
    i.z = (mask >> 1) & 0b1;
    i.p = (mask >> 0) & 0b1;
    // first argument is symbolic
    // PCoffset9
    i.PCoffset9 = 0b111111111;
  } else {
    error("BR sub instruction not Implemnted", tokens[loc].line, tokens);
  }

  DEBUG_LOG(BR)
  return i;
}

instruction Assembler::assembleJMP(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "JMP") {
    i.OP = op2hex.at(JMP);
    // loc++;

    loc++;
    // first argument
    auto r2 = parseRegister(tokens[loc], tokens);
    i.BaseR = reg2hex[r2] & 0b111;

    i.b11 = false;
    i.b10 = false;
    i.b9 = false;

    i.b5 = false;
    i.b4 = false;
    i.b3 = false;
    i.b2 = false;
    i.b1 = false;
    i.b0 = false;
  }
  DEBUG_LOG(JMP)
  return i;
}

instruction Assembler::assembleJSR(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "JSR") {
    i.OP = op2hex.at(JSR);

    // skip label
    i.Poffset11 = 0b11111111111;
    loc++;
    i.b11 = true;
  }

  DEBUG_LOG(JSR)
  return i;
}

instruction Assembler::assembleJSRR(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "JSRR") {
    i.OP = op2hex.at(JSRR);

    // first argument baseR
    loc++;
    auto baseR = parseRegister(tokens[loc], tokens);
    i.BaseR = baseR & 0b111;

    i.b11 = false;
    i.b10 = false;
    i.b9 = false;

    i.b5 = false;
    i.b4 = false;
    i.b3 = false;
    i.b2 = false;
    i.b1 = false;
    i.b0 = false;
  }
  DEBUG_LOG(JSRR)
  return i;
}

instruction Assembler::assembleLDB(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "LDB") {
    i.OP = op2hex.at(LDB);
    loc++;
    // first argument
    auto dr = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[dr] & 0b111;
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // second argument
    auto baseR = parseRegister(tokens[loc], tokens);
    i.BaseR = reg2hex[baseR] & 0b111;
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;

    // third argument
    auto boffset6 = parseNumber(tokens[loc]);
    if (boffset6 != UINT16_MAX) {
      i.boffset6 = boffset6 & 0b111111;
    }
  }

  DEBUG_LOG(LDB)
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

instruction Assembler::assembleNOT(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];

  if (t.lexme == "NOT") {
    i.OP = op2hex.at(NOT);
    loc++;
    // first register
    auto dr = parseRegister(tokens[loc], tokens);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, tokens, "Expected ','");
    loc++;
    // second argument
    auto sr = parseRegister(tokens[loc], tokens);
    i.SR = reg2hex.at(sr);
    i.b5 = true;
    i.b4 = true;
    i.b3 = true;
    i.b2 = true;
    i.b1 = true;
    i.b0 = true;
  }

  DEBUG_LOG(NOT)
  return i;
}

instruction Assembler::assembleRET(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "RET") {
    i.OP = op2hex.at(RET);
    i.BaseR = 0b111;

    i.b11 = false;
    i.b10 = false;
    i.b9 = false;

    i.b5 = false;
    i.b4 = false;
    i.b3 = false;
    i.b2 = false;
    i.b1 = false;
    i.b0 = false;
  }
  DEBUG_LOG(RET)
  return i;
}

instruction Assembler::assembleRTI(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "RTI") {
    i.OP = op2hex.at(RTI);

    i.b11 = false;
    i.b10 = false;
    i.b9 = false;
    i.b8 = false;
    i.b7 = false;
    i.b6 = false;
    i.b5 = false;
    i.b4 = false;
    i.b3 = false;
    i.b2 = false;
    i.b1 = false;
    i.b0 = false;
  }
  DEBUG_LOG(RTI)
  return i;
}

instruction Assembler::assembleLSHF(int &loc, vector<Token> &tokens) {
  instruction i;
  i.i = UINT16_MAX;
  auto t = tokens[loc];
  if (t.lexme == "LSHF") {
    i.OP = op2hex.at(LSHF);

    loc++;
    auto dr = parseRegister(tokens[loc], tokens);
    i.DR = dr & 0b111;

    loc++;
    expect(",", loc, tokens, "Expected ','");

    loc++;
    auto sr = parseRegister(tokens[loc], tokens);
    i.SR = sr & 0b111;

    loc++;
    expect(",", loc, tokens, "Expected ','");

    loc++;
    auto amount4 = parseRegister(tokens[loc], tokens);
    i.amount4 = amount4 & 0b1111;
  }

  error("Not Implemnted", tokens[loc].line, tokens);
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
