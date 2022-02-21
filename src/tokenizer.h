#pragma once

#include <iostream>
#include <string>
#include <vector>

using std::ostream;
using std::string;
using std::vector;

struct Token {
  string lexme;
  int line;
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
