#include "assembler.h"
#include <algorithm>
#include <cctype>
#include <ctype.h>
#include <string>

using namespace std;

vector<instruction> Assembler::assemble(string filename) {
  std::ifstream file(filename);
  // remove previous data
  binaryData.clear();
  symbolTable.clear();

  // read file
  if (file.is_open()) {
    string buffer((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());

    // tokenize
    auto tokens = tokenize(buffer);

    // run all passes
    firstPass(tokens);
    secondPass(tokens);
  } else {
    cout << "Unable to open file\n";
  }

  return binaryData;
}

bool Assembler::isWhiteSpace(char c) { return isspace(c); }

vector<Token> Assembler::tokenize(const std::string &buffer) {
  vector<Token> tokens;
  enum state { START, READCHAR, SKIP, COMMA, COMMENT, DUMP };

  int i = 0, j = 0, line = 1;

  char curChar = 0, prevChar = 0;
  const int n = buffer.size();

  state curState = START;
  while (i < n) {
    // read current character
    i > 0 ? prevChar = buffer[i - 1] : prevChar;
    curChar = buffer[i];
    if (prevChar == '\n')
      line++;

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

void Assembler::error(string message, int line, const vector<Token> &tokens) {

  cerr << "\nError: " << message << " at line:" << line << "\n";
  // print that line
  for (auto t : tokens)
    if (t.line == line)
      cerr << t.lexme << " ";

  exit(1);
}

void Assembler::firstPass(const vector<Token> &tokens) {}
void Assembler::secondPass(const vector<Token> &tokens) {}
