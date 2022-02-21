#include "tokenizer.h"

using namespace std;

ostream &operator<<(ostream &os, Token &t) {
  os << t.lexme;
  return os;
}

vector<Token> Tokenizer::tokenize(string &buffer) {
  vector<Token> tokens;
  enum state { START, READCHAR, STRING, SKIP, COMMA, COMMENT, DUMP };

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
      } else if (curChar == ',') {
        curState = COMMA;
      } else if (curChar == '"') {
        i++;
        j++;
        curState = STRING;
      } else if (isWhiteSpace(curChar)) {
        curState = SKIP;
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

    case STRING:
      if (curChar == '"') {
        i++;
        j++;
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

  // add last token
  if (j != 0) {
    auto t = buffer.substr(i - j, j);
    tokens.push_back({t, line});
    j = 0;
  }
  // cout << "i :" << i << " j :" << j << " n" << n << " \n";

  return tokens;
}

void Tokenizer::printTokens(std::vector<Token> &tokens) {
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

void Tokenizer::printLine(int location, std::vector<Token> &tokens) {
  const auto line = tokens[location].line;
  for (auto t : tokens)
    if (t.line == line)
      cout << t.lexme << " ";
  cout << "\n";
}
