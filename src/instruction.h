#pragma once

#include <cstdint>

struct instruction {
  union {
    uint16_t i;

    struct {
      uint16_t SR2 : 3;
      uint16_t p2 : 3;
      uint16_t SR1 : 3;
      uint16_t DR : 3;
      uint16_t OP : 4;
    };

    struct {
      uint16_t IMM5 : 5;
      uint16_t p3 : 11;
    };

    struct {
      uint16_t PCoffset9 : 9;
      uint16_t p : 1;
      uint16_t z : 1;
      uint16_t n : 1;
      uint16_t p4 : 4;
    };

    struct {
      uint16_t offset6 : 6;
      uint16_t BaseR : 3;
      uint16_t p5 : 7;
    };

    struct {
      uint16_t boffset6 : 6;
      uint16_t p6 : 10;
    };

    struct {
      uint16_t Poffset11 : 11;
      uint16_t p7 : 5;
    };

    struct {
      uint16_t trapvect8 : 8;
      uint16_t p8 : 8;
    };

    struct {
      uint16_t p9 : 6;
      uint16_t SR : 3;
    };

    struct {
      uint16_t amount4 : 4;
      uint16_t p10 : 12;
    };

    struct {
      uint16_t PCoffset9 : 9;
      uint16_t SR : 3;
    } ST;

    struct {
      bool b0 : 1;
      bool b1 : 1;
      bool b2 : 1;
      bool b3 : 1;
      bool b4 : 1;
      bool b5 : 1;
      bool b6 : 1;
      bool b7 : 1;
      bool b8 : 1;
      bool b9 : 1;
      bool b10 : 1;
      bool b11 : 1;
      bool b12 : 1;
      bool b13 : 1;
      bool b14 : 1;
      bool b15 : 1;
    };

    struct {
      char c2;
      char c1;
    };
  };
};
