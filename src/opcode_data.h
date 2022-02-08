#pragma once

// clang-format off
// d(OPCODE_NAME,OPCODE_VALUE)

#include<unordered_map>
#include<string>

using std::unordered_map;
using std::string;

#define opcode_data(d)\
d(ADD,  0b0001)\
d(AND,  0b0101)\
d(BR,   0b0000)\
d(JMP,  0b1100)\
d(JSR,  0b0100)\
d(LD,   0b0010)\
d(LDI,  0b1010)\
d(LDR,  0b0110)\
d(LEA,  0b1110)\
d(NOT,  0b1001)\
d(RET,  0b1100)\
d(RTI,  0b1000)\
d(ST,   0b0011)\
d(STI,  0b1011)\
d(STR,  0b0111)\
d(TRAP, 0b1111)
//d(JSRR, 0b0100)
//  clang-format on

// enum of opcode
#define e(a,b) a = b,
enum Opcode{ opcode_data(e) };
#undef e

// enum to str
#define e(a,b) {a, #a},
const unordered_map<Opcode,string> enum2str ={ opcode_data(e)};
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
