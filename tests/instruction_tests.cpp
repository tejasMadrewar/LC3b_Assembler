#include "catch2/catch.hpp"

#include "../src/instruction.h"

#include <bitset>
#include <iostream>

// using std::bitset;
// using std::cout;

#define BM16(size, offset) (((1 << size) - 1) << (offset))

TEST_CASE("Instruction structure") {
  instruction i;

  SECTION("OP") {
    i.i = 0 | ((1 << 4) - 1) << (16 - 4);
    // cout << bitset<16>(BM16(4, 16 - 4)) << "\n";
    // cout << bitset<16>(BM16(4, 5)) << "\n";
    REQUIRE(i.OP == 0b1111);
  }

  SECTION("DR") {
    i.i = BM16(3, 9);
    REQUIRE(i.DR == 0b111);
  }

  SECTION("SR1") {
    i.i = BM16(3, 6);
    REQUIRE(i.SR1 == 0b111);
  }

  SECTION("SR2") {
    i.i = BM16(3, 0);
    REQUIRE(i.SR2 == 0b111);
  }

  SECTION("IMM5") {
    i.i = BM16(5, 0);
    REQUIRE(i.IMM5 == 0b11111);
  }

  SECTION("OFFSET") {
    i.i = BM16(11, 0);
    REQUIRE(i.Poffset11 == 0b11111111111);

    i.i = BM16(9, 0);
    REQUIRE(i.PCoffset9 == 0b111111111);

    i.i = BM16(6, 0);
    REQUIRE(i.offset6 == 0b111111);

    i.i = BM16(6, 0);
    REQUIRE(i.boffset6 == 0b111111);
  }

  SECTION("NZP") {
    i.i = BM16(1, 9);
    REQUIRE(i.p == 0b1);

    i.i = BM16(1, 10);
    REQUIRE(i.z == 0b1);

    i.i = BM16(1, 11);
    REQUIRE(i.n == 0b1);
  }

  SECTION("BASER") {
    i.i = BM16(3, 6);
    REQUIRE(i.BaseR == 0b111);
    i.i = BM16(6, 0);
    REQUIRE(i.offset6 == 0b111111);
  }

  SECTION("ST.SR") {
    i.i = BM16(9, 0);
    REQUIRE(i.ST.PCoffset9 == 0b111111111);

    i.i = BM16(3, 9);
    REQUIRE(i.ST.SR == 0b111);
  }

  SECTION("SR") {
    i.i = BM16(3, 6);
    REQUIRE(i.SR == 0b111);
  }

  SECTION("TRAPVECT8") {
    i.i = BM16(8, 0);
    REQUIRE(i.trapvect8 == 0b11111111);
  }

  SECTION("amount4") {
    i.i = BM16(4, 0);
    REQUIRE(i.amount4 == 0b1111);
  }

  SECTION("BITS") {

// clang-format off
#define BITLIST(d) d(0) d(1) d(2) d(3) d(4) d(5) d(6) d(7) d(8) d(9) d(10) d(11) d(12) d(13) d(14) d(15)
    // clang-format on
#define t(d)                                                                   \
  i.i = BM16(1, d);                                                            \
  REQUIRE(i.b##d == true);

    BITLIST(t)

#undef BITLIST
#undef t
  }
}
