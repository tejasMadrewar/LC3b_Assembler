#include "disassembler.h"

#define n2r(n) Register(n & 0b111)

// todo: handle negative number
#define mask(n, size) (n & ((1 << size) - 1))
string Disassembler::disInst(instruction i) {

  auto opSearch = hex2op.find(i.OP);
  if (opSearch == hex2op.end()) {
    cout << "Error: Unknown instruction" << hex << i.i << "\n";
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

string Disassembler::disADD(instruction i) {
  const string opName = "ADD";
  if (i.b5) {
    return stringOpRRN(opName, n2r(i.DR), n2r(i.SR1), mask(i.IMM5, 5));
  } else {
    return stringOpRRR(opName, n2r(i.DR), n2r(i.SR1), n2r(i.SR2));
  }
}

string Disassembler::disAND(instruction i) {
  const string opName = "AND";
  if (i.b5) {
    return stringOpRRN(opName, n2r(i.DR), n2r(i.SR1), mask(i.IMM5, 5));
  } else {
    return stringOpRRR(opName, n2r(i.DR), n2r(i.SR1), n2r(i.SR2));
  }
}

string Disassembler::disBR(instruction i) {
  uint32_t mask = 0;
  mask |= i.n << 2;
  mask |= i.z << 1;
  mask |= i.p << 0;

  auto brSearch = mask2br.find(mask);
  if (brSearch != mask2br.end()) {
    return stringOpN(brSearch->second, mask(i.PCoffset9, 9));
  } else {
    return "Unknown Branch instruction type";
  }
}

string Disassembler::disJMP(instruction i) {
  if (i.BaseR == 0b111) {
    return "RET";
  } else {
    return stringOpR("JMP", n2r(i.BaseR));
  }
}

string Disassembler::disJSR(instruction i) {
  if (i.b11) {
    return stringOpN("JSR", mask(i.Poffset11, 11));
  } else {
    return stringOpN("JSRR", n2r(i.BaseR));
  }
}

string Disassembler::disLDB(instruction i) {
  return stringOpRRN("LDB", n2r(i.DR), n2r(i.BaseR), mask(i.boffset6, 6));
}

string Disassembler::disLDW(instruction i) {
  return stringOpRRN("LDW", n2r(i.DR), n2r(i.BaseR), mask(i.boffset6, 6));
}

string Disassembler::disLEA(instruction i) {
  return stringOpRN("LEA", n2r(i.DR), mask(i.PCoffset9, 6));
}

string Disassembler::disRTI(instruction i) {
  if (i.Poffset11 == 0) {
    return "RTI";
  } else {
    return "Invalid RTI instruction";
  }
}

string Disassembler::disSHF(instruction i) {
  if (i.b4 == false) {
    return stringOpRRN("LSHF", n2r(i.DR), n2r(i.SR), mask(i.amount4, 4));
  } else {
    if (i.b5 == false) {
      return stringOpRRN("RSHFL", n2r(i.DR), n2r(i.SR), mask(i.amount4, 4));
    } else {
      return stringOpRRN("RSHFA", n2r(i.DR), n2r(i.SR), mask(i.amount4, 4));
    }
  }
}

string Disassembler::disSTB(instruction i) {
  return stringOpRRN("STB", n2r(i.SR), n2r(i.BaseR), mask(i.boffset6, 6));
}

string Disassembler::disSTW(instruction i) {
  return stringOpRRN("STW", n2r(i.SR), n2r(i.BaseR), mask(i.offset6, 6));
}

string Disassembler::disTRAP(instruction i) {
  return stringOpN("TRAP", mask(i.trapvect8, 8));
}

string Disassembler::disXOR(instruction i) {
  if (i.b5) {
    if (i.IMM5 == 0b11111) {
      return stringOpRR("NOT", n2r(i.DR), n2r(i.SR));
    } else {
      return stringOpRRR("XOR", n2r(i.DR), n2r(i.SR1), n2r(i.SR2));
    }
  } else {
    return stringOpRRN("XOR", n2r(i.DR), n2r(i.SR1), mask(i.IMM5, 5));
  }
}
