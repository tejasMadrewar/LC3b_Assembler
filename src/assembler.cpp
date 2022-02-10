#include "assembler.h"
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
    string assembly((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    // tokenize
    auto tokens = tokenize(assembly);

    // run all passes
    firstPass(tokens);
    secondPass(tokens);
  } else {
    cout << "Unable to open file\n";
  }

  return binaryData;
}

vector<string> Assembler::tokenize(const std::string &assembly) {
  vector<string> tokens;
  enum state { START, READCHAR, SKIP, COMMA, COMMENT, DUMP };

  int i = 0, j = 0;
  char curChar = 0;
  const int n = assembly.size();

  state curState = START;
  while (i < n) {
    // read current character
    curChar = assembly[i];

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
      auto t = assembly.substr(i - j, j);
      // cout << t << " ";
      tokens.push_back(t);
      j = 0;
      curState = START;
      break;
    }
  }

  // todo: add last token

  cout << "Tokens\n";
  for (auto &t : tokens)
    cout << t << " ";

  return tokens;
}

void Assembler::firstPass(const vector<string> &tokens) {}
void Assembler::secondPass(const vector<string> &tokens) {}

bool Assembler ::isWhiteSpace(char c) { return isspace(c); }
