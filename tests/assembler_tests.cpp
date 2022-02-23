#include "catch2/catch.hpp"

#include "../src/assembler.h"

#include <bitset>
#include <iostream>

// using std::bitset;
// using std::cout;

#define BM16(size, offset) (((1 << size) - 1) << (offset))

Assembler assembler;

TEST_CASE("ADD tests") {
  string test1 = "ADD R2, R3, R4";
  string test2 = "ADD R2, R3, #7";
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
  string test1 = "AND R2, R3, R4";
  string test2 = "AND R2, R3, #7";
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
  string test##a = #a " LABEL";                                                \
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
  string test1 = "JMP R2";
  string test2 = "RET";

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

  string test1 = "JSR LABEL";
  string test2 = "JSRR R3";

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
    string test = "LDB R4,R2,#10";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LDB));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);
  }

  SECTION("LDW - load word") {
    string test = "LDW R4,R2,#10";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LDW));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);
  }

  SECTION("LEA - load effective address") {
    string test = "LEA R4,TARGET";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(LEA));
    REQUIRE(i.size() == 1);
    REQUIRE(i[0].DR == 4);
    REQUIRE(i[0].PCoffset9 == 0b111111111);
  }
}

TEST_CASE("NOT tests") {
  SECTION("RET") {
    string test = "NOT R4,R2";
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
    string test = "RET";
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
    string test = "RTI";
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
    string test = "LSHF R2,R3,#3";
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
    string test = "RSHFL R2,R3,#7";

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
    string test = "RSHFA R2,R3,#7";
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
    string test = "STB R4,R2,#10";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(STB));
    REQUIRE(i[0].ST.SR == 4);
    REQUIRE(i[0].BaseR == 2);
    REQUIRE(i[0].boffset6 == 10);

    REQUIRE(i.size() == 1);
  }

  SECTION("STW - store byte") {
    string test = "STW R4,R2,#10";
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
    string test = "TRAP x23";
    auto i = assembler.assembleBuffer(test);
    REQUIRE(i[0].OP == op2hex.at(TRAP));
    REQUIRE(i[0].trapvect8 == 0x23);
  }
}

TEST_CASE("XOR tests") {
  string test1 = "XOR R3, R1, R2";
  string test2 = "XOR R2, R1, #7";

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

TEST_CASE("different TRAP tests") {
#define x(a, b)                                                                \
  SECTION(#a) {                                                                \
    string str = #a;                                                           \
    auto i = assembler.assembleBuffer(str);                                    \
    REQUIRE(i.size() == 1);                                                    \
    REQUIRE(i.at(0).OP == op2hex.at(TRAP));                                    \
    REQUIRE(i.at(0).trapvect8 == b);                                           \
  }

  EXTRA_TRAP_DATA(x)
#undef x
}
