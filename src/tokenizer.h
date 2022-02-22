#pragma once

#include <iostream>
#include <string>
#include <vector>

using std::ostream;
using std::string;
using std::vector;

#define TOKENTYPE_DATA(d)                                                      \
  d(UNKNOWN) d(OP) d(REG) d(NUM) d(DIRECTIVE) d(LABEL) d(COMMA)

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
};
