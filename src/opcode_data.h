#pragma once

// clang-format off
// d(OPCODE_NAME,OPCODE_VALUE)

#include<unordered_map>
#include<string>
#include<vector>

using std::unordered_map;
using std::vector;
using std::string;

#define opcode_data(d)\
d(ADD,  0b0001)\
d(AND,  0b0101)\
d(BR,   0b0000)\
d(JMP,  0b1100)\
d(JSR,  0b0100)\
d(JSRR, 0b0100)\
d(LDB,  0b0010)\
d(LDW,  0b0110)\
d(LEA,  0b1110)\
d(NOT,  0b1001)\
d(RET,  0b1100)\
d(RTI,  0b1000)\
d(LSHF, 0b1101)\
d(RSHFL,0b1101)\
d(RSHFA,0b1101)\
d(STB,  0b0011)\
d(STW,  0b0111)\
d(TRAP, 0b1111)\
d(XOR,  0b1001)

#define register_data(d) \
d(R0)\
d(R1)\
d(R2)\
d(R3)\
d(R4)\
d(R5)\
d(R6)\
d(R7)\
d(REG_COUNT)

//  clang-format on

// Opcode enum
#define e(a,b) a,
enum Opcode{ opcode_data(e) };
#undef e

// opcode str to hex value
#define e(a,b) {a, b},
const unordered_map<Opcode,uint16_t> op2hex ={ opcode_data(e)};
#undef e

// opcode enum to str
#define e(a,b) #a,
const vector<string> op2str ={ opcode_data(e)};
#undef e

// opcode str to enum
#define e(a,b) {#a, a},
const unordered_map<string,Opcode> str2op ={ opcode_data(e)};
#undef e

// Register enum
#define e(a) a,
enum Register{ register_data(e) };
#undef e

// reg enum to str
#define e(a)  #a,
const vector<string> reg2str ={ register_data(e)};
#undef e

// reg str to enum
#define e(a)  {#a,a},
const unordered_map<string,Register> str2reg ={ register_data(e)};
#undef e


#if 0
#define BMASK(n) ((1 << n) - 1)
#define OP(i) ((i >> 12) & BMASK(4))
#define DR(i) ((i >> 9) & BMASK(3))
#define SR1(i) ((i >> 6) & BMASK(3))
#define SR2(i) ((i >> 0) & BMASK(3))
#define IMM5(i) ((i >> 0) & BMASK(5))
#define BASER(i) ((i >> 0) & BMASK(3))
#define OFFSET6(i) ((i >> 0) & BMASK(6))
#define PCOFFSET9(i) ((i >> 0) & BMASK(9))
#define PCOFFSET11(i) ((i >> 0) & BMASK(11))
#endif
