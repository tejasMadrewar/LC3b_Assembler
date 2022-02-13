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
  }

  i = assembler.assembleBuffer(test2);
  size = i.size();

  SECTION("ADD R2, R3, #7") {
    REQUIRE(size == 1);
    REQUIRE(i.at(0).OP == op2hex.at(ADD));
    REQUIRE(i.at(0).DR == 2);
    REQUIRE(i.at(0).SR1 == 3);
    REQUIRE(i.at(0).IMM5 == 7);
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
