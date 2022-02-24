#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "opcode_data.h"

using std::ostream;

#define TOKENTYPE_DATA(d)                                                      \
  d(UNKNOWN) d(OP) d(REG) d(NUM) d(DIRECTIVE) d(LABEL) d(TRAP) d(COMMA) d(STR)

#define x(a) a,
enum class TokenType { TOKENTYPE_DATA(x) };
#undef x

#define x(a) #a,
const vector<string> TokenType2str = {TOKENTYPE_DATA(x)};
#undef x

struct Token {
  string lexme;
  int line;
  int col;
  TokenType type;
};

ostream &operator<<(ostream &os, Token &t);

class Tokenizer {
public:
  vector<Token> tokenize(string &buffer);
  void printTokens(std::vector<Token> &tokens);
  void printLine(int location, vector<Token> &tokens);

private:
  bool isWhiteSpace(char c) { return isspace(c); }
  bool isNumber(string &str);
  bool isLabel(string &str);
  TokenType getTokenType(string &str);
  static const unordered_map<string, TokenType> str2tokentype;
};
