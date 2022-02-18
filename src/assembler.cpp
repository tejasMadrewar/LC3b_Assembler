#include "assembler.h"
#include "opcode_data.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctype.h>

#include <bitset>

using namespace std;

#if 1
#define DEBUG_LOG(OPCODE)                                                      \
  tokenizer.printLine(loc, tokens);                                            \
  cout << "OPCODE -> 0x" << hex << i.i << " -> " << bitset<16>(i.i) << "\n\n";
#else
#define DEBUG_LOG(OPCODE) ;
#endif

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
  tokens = tokenizer.tokenize(buffer);
  tokenizer.printTokens(tokens);

  // run all passes
  firstPass();
  secondPass();

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

void Assembler::error(string message, int line) {

  cout << "\nError: " << message;
  cout << " at line:" << line << "\n";
  // print that line
  for (auto t : tokens)
    if (t.line == line)
      cout << t.lexme << " ";

  exit(1);
}

void Assembler::expect(string expected, int location, string erMsg) {
  const auto &t = tokens[location];
  if (t.lexme != expected) {
    error(erMsg, t.line);
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
  } else if (t.lexme.find_first_not_of("0123456789") == string::npos) {
    // number
    number = stoi(t.lexme);
  }

  if (number == UINT16_MAX) {
    // cerr << "invalid number " << t << "\n";
  }

  return number;
}

Register Assembler::parseRegister(Token t) {

  Register r = R1;
  auto f = str2reg.find(t.lexme);
  if (f != str2reg.end()) {
    r = f->second;
  } else {
    error("Expected register at ", t.line);
  }
  return r;
}

string Assembler::parseString(Token t) {
  string str;

  const int n = t.lexme.length();
  // check for valid string
  if (t.lexme[0] == '"' and t.lexme[n - 1] == '"') {
    // cout << "valid: " << t.lexme << "->" << t.line << "\n";
  } else {
    // cout << "Invalid: " << t.lexme << "\n";
    error("Not vaild string", t.line);
  }
  return str;
}

void Assembler::parseRegRegNum(int &loc, Register &r1, Register &r2,
                               uint16_t &num) {
  r1 = parseRegister(tokens[loc]);
  loc++;

  expect(",", loc, "Expected ','");

  loc++;
  r2 = parseRegister(tokens[loc]);

  loc++;
  expect(",", loc, "Expected ','");

  loc++;
  num = parseNumber(tokens[loc]);
}

void Assembler::firstPass() {

  for (int i = 0; i < tokens.size(); i++) {
    auto t = tokens[i];

    // check for opcode
    auto f = str2op.find(t.lexme);
    // check for directive
    auto d = str2dir.find(t.lexme);
    if (f != str2op.end()) {
      // tokenizer.printLine(i, tokens);
      auto inst = opcode2instruction(i);
      binaryData.push_back(inst);
      // save locations to patch
      auto isInsPatched = labelInst2mask.find(f->second);
      if (isInsPatched != labelInst2mask.end()) {
        // save location
        patchLocations.push_back({binaryData.size() - 1, f->second});
      }
    } else if (d != str2dir.end()) {
      // save location and directive
      auto dir = d->second;
      switch (dir) {

      case END: {
        directiveInfo info;
        info.directive = dir;
        break;
      }

      case ORIG:
      case FILL:
      case BLKW: {
        directiveInfo info;
        info.directive = dir;
        i++;
        info.number = parseNumber(tokens[i]);

        if (info.number == UINT16_MAX) {
          error("Expected number", tokens[i].line);
        }
        directives.push_back(info);
        break;
      }

      case STRINGZ: {
        directiveInfo info;
        info.directive = dir;
        i++;
        info.str = parseString(tokens[i]);
        directives.push_back(info);
        break;
      }
      }
    } else {
      // add to symbol Table
      symbolTable[t.lexme] = binaryData.size();
    }
  }
}

instruction Assembler::opcode2instruction(int &location) {

  instruction i;
  auto token = tokens[location];
  auto opSearch = str2op.find(token.lexme);

  if (opSearch != str2op.end()) {
    // check all other instructions
    auto op = opSearch->second;
#define d(x, y)                                                                \
  case Opcode::x:                                                              \
    i = assemble##x(location);                                                 \
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

void Assembler::secondPass() {
  cout << "Labels \n";
  for (auto s : symbolTable) {
    cout << "Label: " << s.first << " -> " << s.second << "\n";
  }
}

instruction Assembler::assembleADD(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "ADD") {
    i.OP = op2hex.at(ADD);
    loc++;
    // first argument
    auto r2 = parseRegister(tokens[loc]);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second argument
    auto r3 = parseRegister(tokens[loc]);
    i.SR1 = reg2hex[r3];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(tokens[loc]);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto r4 = parseRegister(tokens[loc]);
      i.SR2 = reg2hex[r4];
      i.b5 = false;
      i.b4 = false;
      i.b3 = false;
    }
  }

  DEBUG_LOG(ADD)
  return i;
}

instruction Assembler::assembleAND(int &loc) {
  instruction i;
  i.i = UINT16_MAX;

  if (tokens[loc].lexme == "AND") {
    i.OP = op2hex.at(AND);
    loc++;
    // first register
    auto r2 = parseRegister(tokens[loc]);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second register
    auto r3 = parseRegister(tokens[loc]);
    i.SR1 = reg2hex[r3];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(tokens[loc]);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto r4 = parseRegister(tokens[loc]);
      i.SR2 = reg2hex[r4];
      i.b5 = false;
      i.b4 = false;
      i.b3 = false;
    }
  }

  DEBUG_LOG(AND)
  return i;
}

instruction Assembler::assembleBR(int &loc) {

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
    error("BR sub instruction not Implemnted", tokens[loc].line);
  }

  DEBUG_LOG(BR)
  return i;
}

instruction Assembler::assembleJMP(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "JMP") {
    i.OP = op2hex.at(JMP);
    // loc++;

    loc++;
    // first argument
    auto r2 = parseRegister(tokens[loc]);
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

instruction Assembler::assembleJSR(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "JSR") {
    i.OP = op2hex.at(JSR);

    // skip label
    i.Poffset11 = 0b11111111111;
    loc++;
    i.b11 = true;
  }

  DEBUG_LOG(JSR)
  return i;
}

instruction Assembler::assembleJSRR(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "JSRR") {
    i.OP = op2hex.at(JSRR);

    // first argument baseR
    loc++;
    auto baseR = parseRegister(tokens[loc]);
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

instruction Assembler::assembleLDB(int &loc) {
  instruction i;
  Register dr, baseR;
  uint16_t boffset6;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "LDB") {
    i.OP = op2hex.at(LDB);
    loc++;

    parseRegRegNum(loc, dr, baseR, boffset6);
    i.DR = reg2hex[dr] & 0b111;
    i.BaseR = reg2hex[baseR] & 0b111;
    if (boffset6 != UINT16_MAX) {
      i.boffset6 = boffset6 & 0b111111;
    }
  }

  DEBUG_LOG(LDB)
  return i;
}

instruction Assembler::assembleLDW(int &loc) {
  instruction i;
  Register dr, baseR;
  uint16_t boffset6;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "LDW") {
    i.OP = op2hex.at(LDW);
    loc++;

    parseRegRegNum(loc, dr, baseR, boffset6);
    i.DR = reg2hex[dr] & 0b111;
    i.BaseR = reg2hex[baseR] & 0b111;
    if (boffset6 != UINT16_MAX) {
      i.boffset6 = boffset6 & 0b111111;
    }
  }
  DEBUG_LOG(LDW)
  return i;
}

instruction Assembler::assembleLEA(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "LEA") {
    i.OP = op2hex.at(LEA);
    loc++;
    // first register
    auto dr = parseRegister(tokens[loc]);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;
    // third argument is symbolic writed in second pass
    i.PCoffset9 = 0b111111111;
  }

  // error("Not Implemnted", tokens[location].line, tokens);
  DEBUG_LOG(LEA)
  return i;
}

instruction Assembler::assembleNOT(int &loc) {
  instruction i;
  i.i = UINT16_MAX;

  if (tokens[loc].lexme == "NOT") {
    i.OP = op2hex.at(NOT);
    loc++;
    // first register
    auto dr = parseRegister(tokens[loc]);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;
    // second argument
    auto sr = parseRegister(tokens[loc]);
    i.SR = reg2hex.at(sr);
    i.IMM5 = 0b11111;
  }

  DEBUG_LOG(NOT)
  return i;
}

instruction Assembler::assembleRET(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "RET") {
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

instruction Assembler::assembleRTI(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "RTI") {
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

instruction Assembler::assembleLSHF(int &loc) {
  instruction i;
  Register dr, sr;
  uint16_t amount4;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "LSHF") {
    i.OP = op2hex.at(LSHF);
    loc++;

    parseRegRegNum(loc, dr, sr, amount4);
    i.DR = dr & 0b111;
    i.SR = sr & 0b111;
    i.amount4 = amount4 & 0b1111;

    i.b4 = false;
    i.b5 = false;
  }

  DEBUG_LOG(LSHF)
  return i;
}

instruction Assembler::assembleRSHFL(int &loc) {
  instruction i;
  Register dr, sr;
  uint16_t amount4;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "RSHFL") {
    i.OP = op2hex.at(RSHFL);
    loc++;

    parseRegRegNum(loc, dr, sr, amount4);
    i.DR = dr & 0b111;
    i.SR = sr & 0b111;
    i.amount4 = amount4 & 0b1111;

    i.b4 = true;
    i.b5 = false;
  }

  DEBUG_LOG(RSHFL)
  return i;
}

instruction Assembler::assembleRSHFA(int &loc) {
  instruction i;
  Register dr, sr;
  uint16_t amount4;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "RSHFA") {
    i.OP = op2hex.at(RSHFA);
    loc++;

    parseRegRegNum(loc, dr, sr, amount4);
    i.DR = dr & 0b111;
    i.SR = sr & 0b111;
    i.amount4 = amount4 & 0b1111;

    i.b4 = true;
    i.b5 = true;
  }

  DEBUG_LOG(RSHFA)
  return i;
}

instruction Assembler::assembleSTB(int &loc) {
  instruction i;
  Register sr, baseR;
  uint16_t boffset6;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "STB") {
    i.OP = op2hex.at(STB);
    loc++;

    parseRegRegNum(loc, sr, baseR, boffset6);
    i.ST.SR = sr & 0b111;
    i.BaseR = baseR & 0b111;
    i.boffset6 = boffset6 & 0b1111;
  }

  DEBUG_LOG(STB)
  return i;
}

instruction Assembler::assembleSTW(int &loc) {
  instruction i;
  Register sr, baseR;
  uint16_t offset6;

  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "STW") {
    i.OP = op2hex.at(STW);
    loc++;

    parseRegRegNum(loc, sr, baseR, offset6);
    i.ST.SR = sr & 0b111;
    i.BaseR = baseR & 0b111;
    i.offset6 = offset6 & 0b1111;
  }

  DEBUG_LOG(STB)
  return i;
}

instruction Assembler::assembleTRAP(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "TRAP") {
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

instruction Assembler::assembleXOR(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "XOR") {
    i.OP = op2hex.at(XOR);
    loc++;
    // first argument
    auto dr = parseRegister(tokens[loc]);
    i.DR = reg2hex[dr];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second argument
    auto sr1 = parseRegister(tokens[loc]);
    i.SR1 = reg2hex[sr1];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(tokens[loc]);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto sr2 = parseRegister(tokens[loc]);
      i.SR2 = reg2hex[sr2];
      i.b5 = false;
      i.b4 = false;
      i.b3 = false;
    }
  }

  DEBUG_LOG(ADD)
  return i;
}
