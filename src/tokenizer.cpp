#include "tokenizer.h"
#include <vector>

using namespace std;

#define x(a, b) {#a, TOKEN_TYPE::REG},
#define y(a, b) {#a, TOKEN_TYPE::OP},
#define z(a, b) {#a, TOKEN_TYPE::TRAP},
#define d(a) {"." #a, TOKEN_TYPE::DIRECTIVE},
const unordered_map<string, TOKEN_TYPE> Tokenizer::str2tokentype = {
    REGISTER_DATA(x) OPCODE_DATA(y) BR_DATA(y) EXTRA_TRAP_DATA(z)
        DIRECTIVE_DATA(d){",", TOKEN_TYPE::COMMA}};
#undef x
#undef y
#undef z
#undef d

ostream &operator<<(ostream &os, Token &t) {
  os << t.lexme;
  // os << "("
  //<< t.line << ", " << t.col << ", "
  //<< TokenType2str.at(int(t.type)) << ")";
  return os;
}

bool Tokenizer::isNumber(string &str) {
  const auto n = str.length();
  if (n >= 2) {
    if (str[0] == 'x') {
      auto s = str.substr(1, n - 1);
      // cout << s << "\n";
      return str.find_first_not_of("0123456789abcdefABCDEF", 1) ==
             std::string::npos;
    } else if (str[0] == '#') {
      string s;
      if (str[1] == '-') {
        if (n < 3)
          return false;
        s = str.substr(2, n - 2);
      } else {
        s = str.substr(1, n - 1);
      }
      return s.find_first_not_of("0123456789") == std::string::npos;
    }
  }

  return false;
}

bool Tokenizer::isLabel(string &str) {
  if (str.length() > 0) {
    // first char alpha
    if (!isalpha(str[0]))
      return false;

    // all char alpha numeric
    for (auto c : str)
      if (!isalnum(c))
        return false;
  } else {
    return false;
  }

  return true;
}

TOKEN_TYPE Tokenizer::getTokenType(string &str) {
  auto type = TOKEN_TYPE::INVALID;
  auto tokenTypeSearch = str2tokentype.find(str);

  if (tokenTypeSearch != str2tokentype.end()) {
    // reg, opcode, directive , traps
    type = tokenTypeSearch->second;

  } else if (str.length() > 2 and str.front() == '"' and str.back() == '"') {
    // string
    type = TOKEN_TYPE::STR;
  } else if (isNumber(str)) {
    // number
    type = TOKEN_TYPE::NUM;
  } else if (isLabel(str)) {
    // label
    type = TOKEN_TYPE::LABEL;
  }

  return type;
}

vector<Token> Tokenizer::tokenize(string &buffer) {
  vector<Token> tokens;
  enum state { START, READCHAR, STRING, SKIP, COMMA, COMMENT, DUMP };

  int i = 0, j = 0, line = 1, prevNewline = 0;

  char curChar = 0, prevChar = 0;
  int n = buffer.size();
  int col = 0;

  state curState = START;
  while (i < n) {
    // read current character
    i > 0 ? prevChar = buffer[i - 1] : prevChar;
    curChar = buffer[i];
    if (prevChar == '\n' && prevNewline != i) {
      line++;
      prevNewline = i;
      col = 0;
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

    case DUMP: {
      col++;
      auto lexme = buffer.substr(i - j, j);
      tokens.push_back({lexme, line, col, getTokenType(lexme)});
      j = 0;
      curState = START;
    } break;
    }
  }

  // add last token
  if (j != 0) {
    auto lexme = buffer.substr(i - j, j);
    tokens.push_back({lexme, line, col, getTokenType(lexme)});
    j = 0;
  }

  tokensErrorCheck(tokens);
  return tokens;
}

void Tokenizer::printTokens(std::vector<Token> &tokens) {
  int line = -1;
  for (auto t : tokens) {
    // new line check
    if (line != t.line) {
      line = t.line;
      cout << "\n";
      cout << line << " :";
    }
    // print token
    if (t.type != TOKEN_TYPE::COMMA)
      cout << " ";
    cout << t;
  }
  cout << "\n";
}

void Tokenizer::printLine(int location, std::vector<Token> &tokens) {
  if (location < 0)
    return;
  const auto line = tokens[location].line;
  cout << line << " :";
  for (auto t : tokens) {
    if (t.line == line) {
      if (t.type != TOKEN_TYPE::COMMA)
        cout << " ";
      cout << t;
    }
  }
  cout << "\n";
}

void Tokenizer::tokensErrorCheck(std::vector<Token> &tokens) {
  std::vector<int> errorLocations;
  for (int i = 0; i < tokens.size(); i++) {
    if (tokens[i].type == TOKEN_TYPE::INVALID) {
      errorLocations.push_back(i);
    }
  }
  // print errors if found
  if (errorLocations.size() > 0) {
    for (auto t : errorLocations) {
      cout << "Invalid token: " << tokens[t].lexme << " at line "
           << tokens[t].line << "\n";
      printLine(t, tokens);
      cout << "\n";
    }
    exit(1);
  }
}
