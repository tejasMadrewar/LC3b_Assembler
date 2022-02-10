#pragma once

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#include "instruction.h"

class Assembler {
public:
  std::vector<instruction> assemble(std::string filename);

private:
  std::vector<std::string> tokenize(const std::string &assembly);
  bool isWhiteSpace(char c);
  void firstPass(const std::vector<std::string> &tokens);
  void secondPass(const std::vector<std::string> &tokens);

  std::vector<instruction> binaryData;
  std::unordered_map<std::string, int> symbolTable;
};
