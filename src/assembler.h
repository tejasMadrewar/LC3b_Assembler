#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "instruction.h"

struct Token {
  std::string lexme;
  int line;
};

class Assembler {
public:
  std::vector<instruction> assemble(std::string filename);

private:
private:
  std::vector<Token> tokenize(const std::string &assembly);
  bool isWhiteSpace(char c);
  void firstPass(const std::vector<Token> &tokens);
  void secondPass(const std::vector<Token> &tokens);
  void error(std::string message, int line, const std::vector<Token> &tokens);

  std::vector<instruction> binaryData;
  std::unordered_map<std::string, int> symbolTable;
};
