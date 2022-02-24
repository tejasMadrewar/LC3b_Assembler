#include "tokenizer.h"

using namespace std;

#define x(a, b) {#a, TokenType::REG},
#define y(a, b) {#a, TokenType::OP},
#define z(a, b) {#a, TokenType::TRAP},
#define d(a) {"." #a, TokenType::DIRECTIVE},
const unordered_map<string, TokenType> Tokenizer::str2tokentype = {
    REGISTER_DATA(x) OPCODE_DATA(y) BR_DATA(y) EXTRA_TRAP_DATA(z)
        DIRECTIVE_DATA(d){",", TokenType::COMMA}};
#undef x
#undef y
#undef z
#undef d

ostream &operator<<(ostream &os, Token &t) {
  os << t.lexme;
  os << "("
     //<< t.line << ", " << t.col << ", "
     << TokenType2str.at(int(t.type)) << ")";
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

TokenType Tokenizer::getTokenType(string &str) {
  auto type = TokenType::UNKNOWN;
  auto tokenTypeSearch = str2tokentype.find(str);

  if (tokenTypeSearch != str2tokentype.end()) {
    // reg, opcode, directive , traps
    type = tokenTypeSearch->second;

  } else if (str.length() > 2 and str.front() == '"' and str.back() == '"') {
    // string
    type = TokenType::STR;
  } else if (isNumber(str)) {
    // number
    type = TokenType::NUM;
  } else if (isLabel(str)) {
    // label
    type = TokenType::LABEL;
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

  return tokens;
}

void Tokenizer::printTokens(std::vector<Token> &tokens) {
  int line = 0;
  for (auto t : tokens) {
    if (line != t.line) {
      line = t.line;
      cout << "\n";
    }
    cout << t << " ";
  }
  cout << "\n";
}

void Tokenizer::printLine(int location, std::vector<Token> &tokens) {
  const auto line = tokens[location].line;
  for (auto t : tokens)
    if (t.line == line)
      cout << t << " ";
  cout << "\n";
}
