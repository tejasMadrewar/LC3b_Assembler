#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "opcode_data.h"

using std::ostream;

#define TOKENTYPE_DATA(d)                                                      \
  d(INVALID) d(OP) d(REG) d(NUM) d(DIRECTIVE) d(LABEL) d(TRAP) d(COMMA) d(STR)

#define x(a) a,
enum class TOKEN_TYPE { TOKENTYPE_DATA(x) };
#undef x

#define x(a) #a,
const vector<string> TokenType2str = {TOKENTYPE_DATA(x)};
#undef x

struct Token {
  string lexme;
  int line;
  int col;
  TOKEN_TYPE type;
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
  void tokensErrorCheck(vector<Token> &tokens);
  TOKEN_TYPE getTokenType(string &str);
  static const unordered_map<string, TOKEN_TYPE> str2tokentype;
};
