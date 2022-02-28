#include "assembler.h"
#include "disassembler.h"
#include "opcode_data.h"
#include "tokenizer.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctype.h>
#include <set>

#include <bitset>

using namespace std;

#if 0
#define DEBUG_LOG(OPCODE)                                                      \
  tokenizer.printLine(loc, tokens);                                            \
  cout << "OPCODE -> 0x" << hex << i.i << " -> " << bitset<16>(i.i) << "\n\n";

#define DEBUG_LOG_DIR(DIRECTIVE)                                               \
  tokenizer.printLine(loc, tokens);                                            \
  for (auto d : data)                                                          \
    cout << #DIRECTIVE " -> 0x" << hex << d.i << " -> " << bitset<16>(d.i)     \
         << "\n";                                                              \
  cout << "\n";
#else
#define DEBUG_LOG(OPCODE) ;
#define DEBUG_LOG_DIR(DIRECTIVE) ;
#endif

const unordered_map<Opcode, uint32_t> Assembler::labelInst = {
    {Opcode::JSR, 0b11111111111},
    {Opcode::LEA, 0b111111111},
    {Opcode::BR, 0b111111111},
};

vector<instruction> Assembler::assembleBuffer(string &buffer) {
  // remove previous data
  binaryData.clear();
  symbolTable.clear();
  patchLocations.clear();
  directives.clear();
  errors.clear();

  // tokenize
  tokens = tokenizer.tokenize(buffer);
  tokenizer.printTokens(tokens);

  // run all passes
  firstPass();
  errorChecks();
  secondPass();

  return binaryData;
}

vector<instruction> Assembler::assemble(string filename) {
  ifstream file(filename);

  // read file
  if (file.is_open()) {
    cout << "Reading from " << filename << " \n";
    string buffer((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());
    assembleBuffer(buffer);
    // write data to output file
    writeToFile(binaryData, filename);
    // writeToHex(binaryData, hexFile);
  } else {
    cout << "Unable to open file\n";
  }

  return binaryData;
}

void Assembler::error(string message, int loc) {

  auto t = tokens[loc];
  cout << "\nError: " << message;
  cout << " in line " << t.line;
  cout << " at " << t.lexme << "\n";
  // cout << " on word:" << t.col << "\n";
  //  print that line
  tokenizer.printLine(loc, tokens);

  exit(1);
}

void Assembler::error(string erMsg) {
  cout << "\nError: " << erMsg << "\n";
  exit(1);
}

void Assembler::expect(string expected, int location, string erMsg) {
  const auto &t = tokens[location];
  if (t.lexme != expected) {
    error(erMsg, location);
  }
}

uint16_t Assembler::parseNumber(int loc) {
  uint16_t number = UINT16_MAX;
  auto &t = tokens[loc];

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
  } else {
  }

  return number;
}

Register Assembler::parseRegister(int loc) {
  auto &t = tokens[loc];

  Register r = R1;
  auto f = str2reg.find(t.lexme);
  if (f != str2reg.end()) {
    r = f->second;
  } else {
    error("Expected register at ", loc);
  }
  return r;
}

string Assembler::parseString(int loc) {
  string str;
  auto &t = tokens[loc];

  const int n = t.lexme.length();
  // check for valid string
  if (t.lexme[0] == '"' and t.lexme[n - 1] == '"') {
    auto n = t.lexme.length();
    str = t.lexme.substr(1, n - 2);
    // cout << "Parsed string : (" << str.length() << ") " << str << "\n";
  } else {
    // cout << "Invalid: " << t.lexme << "\n";
    error("Not vaild string", loc);
  }
  return str;
}

void Assembler::parseRegRegNum(int &loc, Register &r1, Register &r2,
                               uint16_t &num) {
  r1 = parseRegister(loc);
  loc++;

  expect(",", loc, "Expected ','");

  loc++;
  r2 = parseRegister(loc);

  loc++;
  expect(",", loc, "Expected ','");

  loc++;
  num = parseNumber(loc);
}

void Assembler::firstPass() {

  for (int i = 0; i < tokens.size(); i++) {
    auto t = tokens[i];

    if (t.type == TOKEN_TYPE::OP) {
      // opcode
      // tokenizer.printLine(i, tokens);
      auto inst = opcode2instruction(i);
      binaryData.push_back(inst);
      // save locations to patch
      auto opcode = str2op.find(t.lexme);
      if (opcode != str2op.end()) {
        auto isInsPatched = labelInst.find(opcode->second);
        if (isInsPatched != labelInst.end()) {
          // save location
          instLabelData d;
          d.location = i;
          d.offset = binaryData.size() - 1;
          patchLocations.push_back(d);
        }
      }
    } else if (t.type == TOKEN_TYPE::DIRECTIVE) {
      // directive
      auto data = directive2instructions(i);
      binaryData.insert(binaryData.end(), data.begin(), data.end());
    } else if (t.type == TOKEN_TYPE::TRAP) {
      auto trapSearch = trap2hex.find(t.lexme);
      if (trapSearch == trap2hex.end())
        error("TRAP not implemented", i);
      instruction inst;
      inst.OP = op2hex.at(TRAP);
      inst.trapvect8 = trapSearch->second & 0xff;
      inst.b8 = false;
      inst.b9 = false;
      inst.b10 = false;
      inst.b11 = false;
      binaryData.push_back(inst);
    } else if (t.type == TOKEN_TYPE::LABEL) {
      // add to symbol Table

      // check if label present of not
      if (symbolTable.find(t.lexme) == symbolTable.end()) {
        symbolTable[t.lexme] = binaryData.size();
      } else {
        errors.push_back({Error::labelDefinedOnlyOnce, i});
      }
    } else {
      error("Expected Opcode", i);
    }
  }
  // tokenizer.printTokens(tokens);
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
      cout << "Instruction not implemented\n";
    }
#undef d
  } else {
    error("Instruction not implemented", location);
  }

  return i;
}

vector<instruction> Assembler::directive2instructions(int &location) {
  vector<instruction> data = {};

  auto token = tokens[location];
  auto directiveSearch = str2dir.find(token.lexme);

  if (directiveSearch != str2dir.end()) {
    auto dir = directiveSearch->second;

    switch (dir) {
    case ORIG:
      data = assembleORIG(location);
      break;
    case FILL:
      data = assembleFILL(location);
      break;
    case BLKW:
      data = assembleBLKW(location);
      break;
    case STRINGZ:
      data = assembleSTRINGZ(location);
      break;
    case END: {
      directiveInfo info;
      info.directive = END;
      info.location = location;
      directives.push_back(info);
    } break;
    default:
      error("Directive not implemented\n", location);
    }
  } else {
    error("Directive not implemented", location);
  }

  return data;
}

void Assembler::secondPass() {
#if 0
  cout << "Labels \n";
  for (auto s : symbolTable)
    cout << "Label: " << s.first << " -> 0x" << hex << (s.second * 2) << "\n";
#endif

  for (const auto &l : patchLocations) {
    auto &inst = binaryData[l.offset];
    auto opcode = inst.OP;
    auto t = tokens[l.location];
    // cout << "Patch location: " << (l.first * 2) << " -> " << l.second <<
    // "\n";

    switch (opcode) {
      // BRANCH
    case 0b0000: {
      auto address = symbolTable.find(t.lexme);
      if (address != symbolTable.end()) {
        inst.PCoffset9 = address->second * 2;
      }
    } break;
      // JSR
    case 0b0100: {
      if (inst.b11) {
        auto address = symbolTable.find(t.lexme);
        if (address != symbolTable.end()) {
          inst.Poffset11 = address->second * 2;
        }
      }
    } break;
      // LEA
    case 0b1110: {
      auto address = symbolTable.find(t.lexme);
      if (address != symbolTable.end()) {
        inst.PCoffset9 = address->second * 2;
      }
    } break;
    }
  }
}

void Assembler::errorChecks() {
  int n = tokens.size();
  int orig_count = 0;
  int end_count = 0;
  for (auto d : directives) {
    switch (d.directive) {
    case ORIG: {
      orig_count++;
      if (d.location != 0) {
        errors.push_back({Error::assemblyBeforeORIG, d.location});
        errors.push_back({Error::ORIG_END_location, d.location});
        errors.push_back({Error::nothingBeforeORIG, d.location});
      }
      break;
    }

    case END: {
      end_count++;
      if (d.location != n - 1) {
        errors.push_back({Error::ORIG_END_location, d.location});
      }
      if (d.location < n - 1) {
        cout << err2msg.at(Error::ignoreAfterEND) << "\n";
        // errors.push_back({Error::ignoreAfterEND, d.location});
      }
      break;
    }
    case FILL:
    case BLKW:
    case STRINGZ:
      break;
    }
  }
  // no end error
  if (end_count == 0) {
    errors.push_back({Error::mustEndWithEND, -1});
  }

  // no assembly inst check
  if (binaryData.size() == 0) {
    errors.push_back({Error::noAssembly, -1});
  }

  // only one orig
  if (orig_count > 1) {
    errors.push_back({Error::onlyOneORIG, -1});
  }

  // check for undefined Label
  for (auto p : patchLocations) {
    auto t = tokens[p.location];
    // check if label is defined of not
    auto s = symbolTable.find(t.lexme);
    if (s == symbolTable.end()) {
      errors.push_back({Error::unknownLabel, p.location});
    }
  }

  // PRINT ERRORS IF FOUND
  if (errors.size() != 0) {
    for (auto e : errors) {
      cout << "\n";
      cout << "    Error: " << err2msg.at(e.er) << "\n    ";
      tokenizer.printLine(e.location, tokens);
    }
    // exit(1);
  }
}

void Assembler::writeToFile(vector<instruction> data, string fileName) {

  auto hexFile = fileName.substr(0, fileName.find_last_of('.')) + ".hex";
  auto binFile = fileName.substr(0, fileName.find_last_of('.')) + ".bin";
  auto objFile = fileName.substr(0, fileName.find_last_of('.')) + ".obj";
  auto symFile = fileName.substr(0, fileName.find_last_of('.')) + ".symtab";

  auto endianAdjusted = data;
  instruction address;
  address.i = UINT16_MAX;
  for (auto d : directives) {
    if (d.directive == ORIG) {
      address.i = d.number;
      break;
    }
  }

  // add ORIG address
  endianAdjusted.insert(endianAdjusted.begin(), address);

  writeToObj(endianAdjusted, objFile);
  writeToHex(endianAdjusted, hexFile);
  writeToBin(endianAdjusted, binFile);
  writeSymbolTable(symbolTable, symFile);
}

void Assembler::writeToHex(vector<instruction> &data, string &hexFile) {
  fstream ofile(hexFile, fstream::out);
  // write data
  if (ofile.is_open()) {
    cout << "Writing to " << hexFile << " \n";
    for (auto i : data) {
      ofile << "0x" << hex << setw(4) << setfill('0') << i.i << "\n";
    }
  } else {
    cout << "Unable to open file" << hexFile << "\n";
  }
}

void Assembler::writeToObj(vector<instruction> data, string &filename) {
  fstream ofile(filename, fstream::out);
  //// change to big endianess
  for (auto &i : data) {
    auto c1 = i.c1;
    i.c1 = i.c2;
    i.c2 = c1;
  }
  // write data
  if (ofile.is_open()) {
    cout << "Writing to " << filename << " \n";
    ofile.write((char *)&data[0], data.size() * sizeof(instruction));
  } else {
    cout << "Unable to open file" << filename << "\n";
  }
}

void Assembler::writeToBin(vector<instruction> &data, string &filename) {
  fstream ofile(filename, fstream::out);
  // write data
  if (ofile.is_open()) {
    cout << "Writing to " << filename << " \n";
    for (auto i : data) {
      ofile << "0b" << bitset<16>(i.i) << "\n";
    }
  } else {
    cout << "Unable to open file" << filename << "\n";
  }
}

void Assembler::writeSymbolTable(unordered_map<string, int> symTab,
                                 string filename) {
  fstream ofile(filename, fstream::out);
  uint16_t ORIG_ADDR = UINT16_MAX;
  for (auto d : directives) {
    if (d.directive == ORIG) {
      ORIG_ADDR = d.number;
      break;
    }
  }

  if (ofile.is_open()) {
    cout << "Writing to " << filename << " \n";
    for (auto s : symTab) {
      ofile << s.first << "\t-> 0x" << hex << setw(4) << setfill('0')
            << (s.second + ORIG_ADDR) << "\n";
    }
  } else {
    cout << "Unable to open file" << filename << "\n";
  }
}

instruction Assembler::assembleADD(int &loc) {
  instruction i;
  i.i = UINT16_MAX;
  if (tokens[loc].lexme == "ADD") {
    i.OP = op2hex.at(ADD);
    loc++;
    // first argument
    auto r2 = parseRegister(loc);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second argument
    auto r3 = parseRegister(loc);
    i.SR1 = reg2hex[r3];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(loc);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto r4 = parseRegister(loc);
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
    auto r2 = parseRegister(loc);
    i.DR = reg2hex[r2];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second register
    auto r3 = parseRegister(loc);
    i.SR1 = reg2hex[r3];
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(loc);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto r4 = parseRegister(loc);
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
    error("BR sub instruction not Implemnted", loc);
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
    auto r2 = parseRegister(loc);
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
    auto baseR = parseRegister(loc);
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
    i.DR = dr & 0b111;
    i.BaseR = baseR & 0b111;
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
    i.DR = dr & 0b111;
    i.BaseR = baseR & 0b111;
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
    auto dr = parseRegister(loc);
    i.DR = dr & 0b111;
    loc++;

    expect(",", loc, "Expected ','");
    loc++;
    // third argument is written in second pass
    i.PCoffset9 = 0b111111111;
  }

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
    auto dr = parseRegister(loc);
    i.DR = dr;
    loc++;

    expect(",", loc, "Expected ','");
    loc++;
    // second argument
    auto sr = parseRegister(loc);
    i.SR = sr & 0b111;
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
    auto trapvect8 = parseNumber(loc);
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
    auto dr = parseRegister(loc);
    i.DR = dr & 0b111;
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // second argument
    auto sr1 = parseRegister(loc);
    i.SR1 = sr1 & 0b111;
    loc++;

    expect(",", loc, "Expected ','");
    loc++;

    // third argument either register or 5 bit number
    auto imm5 = parseNumber(loc);
    if (imm5 != UINT16_MAX) {
      // third number
      i.b5 = true;
      i.IMM5 = imm5 & 0b11111;
    } else {
      // third register
      auto sr2 = parseRegister(loc);
      i.SR2 = sr2 & 0b111;
      i.b5 = false;
      i.b4 = false;
      i.b3 = false;
    }
  }

  DEBUG_LOG(ADD)
  return i;
}

vector<instruction> Assembler::assembleORIG(int &loc) {
  vector<instruction> data;
  directiveInfo info;

  if (tokens[loc].lexme == ".ORIG") {
    info.directive = ORIG;
    info.location = loc;
    loc++;
    info.number = parseNumber(loc);

    if (info.number == UINT16_MAX) {
      error("Expected number", loc);
    }

    directives.push_back(info);
  }
  DEBUG_LOG_DIR(ORIG)
  return data;
}

vector<instruction> Assembler::assembleFILL(int &loc) {
  vector<instruction> data;
  directiveInfo info;

  if (tokens[loc].lexme == ".FILL") {
    info.directive = FILL;
    info.location = loc;
    loc++;
    info.number = parseNumber(loc);

    if (info.number == UINT16_MAX) {
      error("Expected number", loc);
    }
    directives.push_back(info);

    instruction i;
    i.i = info.number;
    data.push_back(i);
  }

  DEBUG_LOG_DIR(FILL)
  return data;
}

vector<instruction> Assembler::assembleBLKW(int &loc) {
  vector<instruction> data;
  directiveInfo info;

  if (tokens[loc].lexme == ".BLKW") {
    directiveInfo info;
    instruction i;
    info.directive = BLKW;
    info.location = loc;
    loc++;
    info.number = parseNumber(loc);

    if (info.number == UINT16_MAX) {
      error("Expected number", loc);
    }
    directives.push_back(info);

    i.i = 0;
    data.resize(info.number, i);
  }

  DEBUG_LOG_DIR(BLKW)
  return data;
}

vector<instruction> Assembler::assembleSTRINGZ(int &loc) {
  vector<instruction> data;

  if (tokens[loc].lexme == ".STRINGZ") {
    instruction i, a;
    i.i = 0;
    directiveInfo info;
    info.directive = STRINGZ;
    info.location = loc;

    loc++;
    info.str = parseString(loc);
    directives.push_back(info);

    const auto n = info.str.length();

    // save data upto last even length
    for (int j = 0; j < n / 2; j++) {
      a.c1 = info.str[j * 2 + 0];
      a.c2 = info.str[j * 2 + 1];
      data.push_back(a);
    }

    if (n % 2 == 1) {
      // last odd char
      a.c1 = info.str[n - 1];
      a.c2 = 0;
      data.push_back(a);
    } else {
      a.c1 = 0;
      a.c2 = 0;
      data.push_back(a);
    }
  }

  DEBUG_LOG_DIR(STRINGZ)
  return data;
}
