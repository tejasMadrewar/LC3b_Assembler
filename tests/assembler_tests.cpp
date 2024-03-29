#include "catch2/catch.hpp"

#include "../src/assembler.h"

#include <bitset>
#include <iostream>

// using std::bitset;
// using std::cout;

#define BM16(size, offset) (((1 << size) - 1) << (offset))

Assembler assembler;

TEST_CASE("ADD tests") {
  string test1 = ".ORIG x3000\n ADD R2, R3, R4 \n.END";
  string test2 = ".ORIG x3000\n ADD R2, R3, #7 \n.END";
  auto i = assembler.assembleBuffer(test1);
  auto size = i.size();

  SECTION("ADD R2, R3, R4") {
    REQUIRE(size == 1);
    REQUIRE(i.at(0).OP == op2hex.at(ADD));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 3);
    REQUIRE(i.at(0).SR2 == 4);
    REQUIRE(i.at(0).b5 == false);
    REQUIRE(i.at(0).b4 == false);
    REQUIRE(i.at(0).b3 == false);
  }

  i = assembler.assembleBuffer(test2);
  size = i.size();

  SECTION("ADD R2, R3, #7") {
    REQUIRE(size == 1);
    REQUIRE(i.at(0).OP == op2hex.at(ADD));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 3);
    REQUIRE(i.at(0).IMM5 == 7);
    REQUIRE(i.at(0).b5 == true);
  }
}

TEST_CASE("AND tests") {
  string test1 = ".ORIG x3000\n AND R2, R3, R4 \n.END";
  string test2 = ".ORIG x3000\n AND R2, R3, #7 \n.END";
  auto i = assembler.assembleBuffer(test1);
  auto size = i.size();

  SECTION("AND R2, R3, R4") {
    REQUIRE(size == 1);
    REQUIRE(i.at(0).OP == op2hex.at(AND));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 3);
    REQUIRE(i.at(0).SR2 == 4);
  }

  i = assembler.assembleBuffer(test2);
  size = i.size();

  SECTION("AND R2, R3, #7") {
    REQUIRE(size == 1);
    REQUIRE(i.at(0).OP == op2hex.at(AND));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 3);
    REQUIRE(i.at(0).IMM5 == 7);
  }
}

TEST_CASE("BR tests") {

#define e(a, b)                                                                \
  string test##a = ".ORIG x3000\n LABEL1 " #a " LABEL1 \n.END";                \
  SECTION(test##a) {                                                           \
    auto i = assembler.assembleBuffer(test##a);                                \
    uint16_t mask = 0;                                                         \
    mask |= (i.at(0).n) << 2;                                                  \
    mask |= (i.at(0).z) << 1;                                                  \
    mask |= (i.at(0).p) << 0;                                                  \
    REQUIRE(i.size() == 1);                                                    \
    REQUIRE(i.at(0).OP == op2hex.at(BR));                                      \
    REQUIRE(mask == b);                                                        \
    REQUIRE(i.at(0).PCoffset9 == 0b111111111);                                 \
  }
  BR_MASK(e)
#undef e

  // todo: test labels
}

TEST_CASE("JMP, RET tests") {
  string test1 = ".ORIG x3000\n JMP R2 \n.END";
  string test2 = ".ORIG x3000\n RET \n.END";

  SECTION("JMP") {
    auto i = assembler.assembleBuffer(test1);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(JMP));
    REQUIRE(i[0].BaseR == 2);

    REQUIRE(i[0].b11 == false);
    REQUIRE(i[0].b10 == false);
    REQUIRE(i[0].b9 == false);

    REQUIRE(i[0].b5 == false);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b3 == false);
    REQUIRE(i[0].b2 == false);
    REQUIRE(i[0].b1 == false);
    REQUIRE(i[0].b0 == false);
  }

  SECTION("RET") {
    auto i = assembler.assembleBuffer(test2);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(RET));
    REQUIRE(i[0].BaseR == 0b111);

    REQUIRE(i[0].b11 == false);
    REQUIRE(i[0].b10 == false);
    REQUIRE(i[0].b9 == false);

    REQUIRE(i[0].b5 == false);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b3 == false);
    REQUIRE(i[0].b2 == false);
    REQUIRE(i[0].b1 == false);
    REQUIRE(i[0].b0 == false);
  }
}

TEST_CASE("JSR, JSRR tests") {

  string test1 = ".ORIG x3000\n LABEL1 JSR LABEL1 \n.END";
  string test2 = ".ORIG x3000\n JSRR R3 \n.END";

  SECTION("JSR LABEL") {
    auto i = assembler.assembleBuffer(test1);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(JSR));
    REQUIRE(i[0].Poffset11 == 0b11111111111);

    REQUIRE(i[0].b11 == true);
  }

  SECTION("JSRR R3") {
    auto i = assembler.assembleBuffer(test2);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(JSRR));
    REQUIRE(i[0].BaseR == 3);

    REQUIRE(i[0].b11 == false);
    REQUIRE(i[0].b10 == false);
    REQUIRE(i[0].b9 == false);

    REQUIRE(i[0].b5 == false);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b3 == false);
    REQUIRE(i[0].b2 == false);
    REQUIRE(i[0].b1 == false);
    REQUIRE(i[0].b0 == false);
  }
}

TEST_CASE("LOAD instructions tests") {

  SECTION("LDB - load byte") {
    string test = ".ORIG x3000\n LDB R4,R2,#10 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LDB));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);
  }

  SECTION("LDW - load word") {
    string test = ".ORIG x3000\n LDW R4,R2,#10 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LDW));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);
  }

  SECTION("LEA - load effective address") {
    string test = ".ORIG x3000\n TARGET LEA R4,TARGET \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LEA));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].PCoffset9 == 0b111111111);
  }
}

TEST_CASE("NOT tests") {
  SECTION("RET") {
    string test = ".ORIG x3000\n NOT R4,R2 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(NOT));
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].SR == 2);

    REQUIRE(i[0].b5 == true);
    REQUIRE(i[0].b4 == true);
    REQUIRE(i[0].b3 == true);
    REQUIRE(i[0].b2 == true);
    REQUIRE(i[0].b1 == true);
    REQUIRE(i[0].b0 == true);
  }
}

TEST_CASE("RET tests") {
  SECTION("RET") {
    string test = ".ORIG x3000\n RET \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(RET));

    REQUIRE(i[0].b11 == false);
    REQUIRE(i[0].b10 == false);
    REQUIRE(i[0].b9 == false);

    REQUIRE(i[0].b8 == true);
    REQUIRE(i[0].b7 == true);
    REQUIRE(i[0].b6 == true);

    REQUIRE(i[0].b5 == false);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b3 == false);
    REQUIRE(i[0].b2 == false);
    REQUIRE(i[0].b1 == false);
    REQUIRE(i[0].b0 == false);
  }
}

TEST_CASE("RTI tests") {
  SECTION("RTI") {
    string test = ".ORIG x3000\n RTI \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(RTI));

    REQUIRE(i[0].b11 == false);
    REQUIRE(i[0].b10 == false);
    REQUIRE(i[0].b9 == false);
    REQUIRE(i[0].b8 == false);
    REQUIRE(i[0].b7 == false);
    REQUIRE(i[0].b6 == false);
    REQUIRE(i[0].b5 == false);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b3 == false);
    REQUIRE(i[0].b2 == false);
    REQUIRE(i[0].b1 == false);
    REQUIRE(i[0].b0 == false);
  }
}

TEST_CASE("SHF - bit shift tests") {
  SECTION("LSHF") {
    string test = ".ORIG x3000\n LSHF R2,R3,#3 \n.END";
    auto i = assembler.assembleBuffer(test);

    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(LSHF));
    REQUIRE(i[0].DR == 2);
    REQUIRE(i[0].SR == 3);
    REQUIRE(i[0].amount4 == 3);
    REQUIRE(i[0].b4 == false);
    REQUIRE(i[0].b5 == false);
  }

  SECTION("RSHFL") {
    string test = ".ORIG x3000\n RSHFL R2,R3,#7 \n.END";

    auto i = assembler.assembleBuffer(test);

    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(LSHF));
    REQUIRE(i[0].DR == 2);
    REQUIRE(i[0].SR == 3);
    REQUIRE(i[0].amount4 == 7);
    REQUIRE(i[0].b4 == true);
    REQUIRE(i[0].b5 == false);
  }

  SECTION("RSHFA") {
    string test = ".ORIG x3000\n RSHFA R2,R3,#7 \n.END";
    auto i = assembler.assembleBuffer(test);

    REQUIRE(i.size() == 1);
    REQUIRE(i[0].OP == op2hex.at(LSHF));
    REQUIRE(i[0].DR == 2);
    REQUIRE(i[0].SR == 3);
    REQUIRE(i[0].amount4 == 7);
    REQUIRE(i[0].b4 == true);
    REQUIRE(i[0].b5 == true);
  }
}

TEST_CASE("STORE instructions tests") {
  SECTION("STB - store byte") {
    string test = ".ORIG x3000\n STB R4,R2,#10 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(STB));
    REQUIRE(i[0].ST.SR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);

    REQUIRE(i.size() == 1);
  }

  SECTION("STW - store byte") {
    string test = ".ORIG x3000\n STW R4,R2,#10 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(STW));
    REQUIRE(i[0].ST.SR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].offset6 == 10);

    REQUIRE(i.size() == 1);
  }
}

TEST_CASE("TRAP instructions tests") {
  SECTION("OS Call") {
    string test = ".ORIG x3000\n TRAP x23 \n.END";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(TRAP));
    REQUIRE(i[0].trapvect8 == 0x23);
  }
}

TEST_CASE("XOR tests") {
  string test1 = ".ORIG x3000\n XOR R3, R1, R2 \n.END";
  string test2 = ".ORIG x3000\n XOR R2, R1, #7 \n.END";

  SECTION(test1) {
    auto i = assembler.assembleBuffer(test1);
    REQUIRE(i.size() == 1);
    REQUIRE(i.at(0).OP == op2hex.at(XOR));
    REQUIRE(i.at(0).DR == 3);
    REQUIRE(i.at(0).SR1 == 1);
    REQUIRE(i.at(0).SR2 == 2);
    REQUIRE(i.at(0).b5 == false);
    REQUIRE(i.at(0).b4 == false);
    REQUIRE(i.at(0).b3 == false);
  }

  SECTION(test2) {
    auto i = assembler.assembleBuffer(test2);
    REQUIRE(i.size() == 1);
    REQUIRE(i.at(0).OP == op2hex.at(XOR));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 1);
    REQUIRE(i.at(0).IMM5 == 7);
    REQUIRE(i.at(0).b5 == true);
  }
}

TEST_CASE("different TRAP tests"){
#define x(a, b)                                                                \
  SECTION(#a) {                                                                \
    string str = ".ORIG x3000\n" #a "\n.END";                                  \
    auto i = assembler.assembleBuffer(str);                                    \
    REQUIRE(i.size() == 1);                                                    \
    REQUIRE(i.at(0).OP == op2hex.at(TRAP));                                    \
    REQUIRE(i.at(0).trapvect8 == b);                                           \
  }

    EXTRA_TRAP_DATA(x)
#undef x
}

TEST_CASE("NOP tests") {
  string test1 = ".ORIG x3000\n NOP .END";

  SECTION(test1) {
    auto i = assembler.assembleBuffer(test1);
    REQUIRE(i.size() == 1);
    REQUIRE(i.at(0).i == 0);
  }
}

TEST_CASE("LABEL address locations") {
  string test1 = ".ORIG x3000\n BR LABEL1 LABEL1 .END";
  string test2 = ".ORIG x3000\n BR LABEL1\
		  BR LABEL1\
		  LABEL1 .END";
  string test3 = ".ORIG x3000\n LABEL1\
		  BR LABEL1\
		  BR LABEL1\
		  .END ";

  SECTION("zero address label") {
    auto i = assembler.assembleBuffer(test1);
    REQUIRE(i.size() == 1);
    REQUIRE(i.at(0).OP == 0);
    REQUIRE(i.at(0).PCoffset9 == 0);
  }

  SECTION("multiple positive Poffsets") {
    auto i = assembler.assembleBuffer(test2);
    REQUIRE(i.size() == 2);
    REQUIRE(i.at(0).OP == 0);
    REQUIRE(i.at(0).PCoffset9 == 1);

    REQUIRE(i.at(1).OP == 0);
    REQUIRE(i.at(1).PCoffset9 == 0);
  }

  SECTION("multiple negative Poffsets") {
    auto i = assembler.assembleBuffer(test3);
    REQUIRE(i.size() == 2);
    REQUIRE(i.at(0).OP == 0);
    REQUIRE(i.at(0).PCoffset9 == 0x1ff);

    REQUIRE(i.at(1).OP == 0);
    REQUIRE(i.at(1).PCoffset9 == 0x1fe);
  }
}
