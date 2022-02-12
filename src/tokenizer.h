#pragma once

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::string;
using std::vector;

struct Token {
  std::string lexme;
  int line;
};

class Tokenizer {
public:
  vector<Token> tokenize(string &buffer) {
    vector<Token> tokens;
    enum state { START, READCHAR, SKIP, COMMA, COMMENT, DUMP };

    int i = 0, j = 0, line = 1, prevNewline = 0;

    char curChar = 0, prevChar = 0;
    int n = buffer.size();

    state curState = START;
    while (i < n) {
      // read current character
      i > 0 ? prevChar = buffer[i - 1] : prevChar;
      curChar = buffer[i];
      if (prevChar == '\n' && prevNewline != i) {
        line++;
        prevNewline = i;
      }

      switch (curState) {
      case START:
        if (curChar == ';') {
          curState = COMMENT;
          break;
        } else if (curChar == ',') {
          curState = COMMA;
        } else if (isWhiteSpace(curChar)) {
          curState = SKIP;
          break;
        } else {
          // cout << curChar;
          // i++;
          curState = READCHAR;
        }
        break;

      case SKIP:
        if (isWhiteSpace(curChar)) {
          i++;
          j = 0;
        } else {
          curState = START;
        }
        break;

      case COMMA:
        if (curChar == ',') {
          i++;
          j++;
          curState = DUMP;
        } else {
          curState = START;
        }
        break;

      case COMMENT:
        if (curChar == '\n') {
          curState = START;
          j = 0;
        }
        i++;
        break;

      case READCHAR:
        if (isWhiteSpace(curChar)) {
          curState = DUMP;
        } else if (curChar == ',') {
          curState = DUMP;
        } else if (curChar == ';') {
          curState = DUMP;
        } else {
          i++;
          j++;
        }
        break;

      case DUMP:
        auto t = buffer.substr(i - j, j);
        // cout << t << " ";
        tokens.push_back({t, line});
        j = 0;
        curState = START;
        break;
      }
    }

    // todo: add last token

    return tokens;
  }

  void printTokens(std::vector<Token> &tokens) {
    int line = 0;
    for (auto t : tokens) {
      if (line != t.line) {
        line = t.line;
        cout << "\n";
      }
      cout << t.lexme << " ";
    }
    cout << "\n";
  }

private:
  bool isWhiteSpace(char c) { return isspace(c); }
};
