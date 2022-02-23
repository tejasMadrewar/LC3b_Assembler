#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

// clang-format off
// d(OPCODE_NAME,OPCODE_VALUE)
#define OPCODE_DATA(d)\
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

#define BR_DATA(d)\
d(BRn,   0b0000)\
d(BRz,   0b0000)\
d(BRp,   0b0000)\
d(BRzp,  0b0000)\
d(BRnp,  0b0000)\
d(BRnz,  0b0000)\
d(BRnzp, 0b0000)

#define BR_MASK(d)\
d(BR,    0b000)\
d(BRn,   0b100)\
d(BRz,   0b010)\
d(BRp,   0b001)\
d(BRzp,  0b011)\
d(BRnp,  0b101)\
d(BRnz,  0b110)\
d(BRnzp, 0b111)

#define REGISTER_DATA(d) \
d(R0,0)\
d(R1,1)\
d(R2,2)\
d(R3,3)\
d(R4,4)\
d(R5,5)\
d(R6,6)\
d(R7,7)

#define DIRECTIVE_DATA(d)\
d(ORIG)\
d(FILL)\
d(BLKW)\
d(STRINGZ)\
d(END)

#define EXTRA_TRAP_DATA(d)\
d(GETC, 0x20)\
d(OUT,  0x21)\
d(PUTS, 0x22)\
d(IN,   0x23)\
d(PUTSP,0x24)\
d(HALT, 0x25)

#define DISASSEMBLY_DATA(d)\
d(ADD)\
d(AND)\
d(BR)\
d(JMP)\
d(JSR)\
d(LDB)\
d(LDW)\
d(LEA)\
d(RTI)\
d(STB)\
d(STW)\
d(TRAP)\
d(XOR)\
d(LSHF)\
d(RSHFL)\
d(RSHFA)
//  clang-format on

// Opcode enum
#define e(a,b) a,
enum Opcode{ OPCODE_DATA(e)  };
#undef e

// opcode to hex value
#define e(a,b) {a, b},
const unordered_map<Opcode,uint16_t> op2hex ={ OPCODE_DATA(e) };
#undef e

// extra opcode for trap
#define d(a,b) {#a, b},
const unordered_map<string, uint16_t> trap2hex = { EXTRA_TRAP_DATA(d) };
#undef d

// opcode enum to str
#define e(a,b) #a,
const vector<string> op2str ={ OPCODE_DATA(e)};
#undef e

// opcode str to opcode enum
#define e(a,b) {#a, a},
#define br(a,b) {#a, BR},
const unordered_map<string,Opcode> str2op ={ OPCODE_DATA(e) BR_DATA(br) };
#undef br
#undef e

// Register enum
#define e(a,b) a,
enum Register{ REGISTER_DATA(e) };
#undef e

// Register to hex
#define e(a,b) b,
const vector<uint16_t> reg2hex ={ REGISTER_DATA(e)};
#undef e


// reg enum to str
#define e(a,b)  #a,
const vector<string> reg2str ={ REGISTER_DATA(e)};
#undef e

// reg str to enum
#define e(a,b)  {#a,a},
const unordered_map<string,Register> str2reg ={ REGISTER_DATA(e)};
#undef e

// Directive enum
#define e(a) a,
enum Directive{ DIRECTIVE_DATA(e) };
#undef e

// directive str to enum
#define e(a)  {"."#a, a},
const unordered_map<string,Directive> str2dir ={ DIRECTIVE_DATA(e) };
#undef e


struct directiveInfo{
	Directive directive;
	string str;
	uint16_t number;
};

// disassembly data
#define d(a) {op2hex.at(a), a},
const unordered_map<uint16_t, Opcode> hex2op = { DISASSEMBLY_DATA(d) };
#undef d

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
