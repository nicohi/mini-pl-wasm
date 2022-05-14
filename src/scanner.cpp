#include "scanner.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace Scanner {

#define F(name, desc) #name,
std::string TokenName[]{TOKEN_TYPES(F)};
#undef F

struct Scanner {
  char *src;
  const char *start;
  const char *current;
  int line;
};

static Scanner scanner;

void init(const std::string source) {
  scanner.src = (char *)std::malloc(source.size() + 1);
  std::memcpy(scanner.src, source.c_str(), source.size());
  scanner.start = scanner.src;
  scanner.current = scanner.src;
  scanner.line = 1;
}

std::string getName(Token *t) { return TokenName[static_cast<int>(t->type)]; }
std::string getName(TokenType t) { return TokenName[static_cast<int>(t)]; }

static bool isEnd() { return *scanner.current == '\0'; }

static Token *makeToken(TokenType type) {
  Token *t = new Token();
  t->type = type;
  t->start = scanner.start;
  t->length = (int)(scanner.current - scanner.start);
  t->line = scanner.line;
  t->message = "";
  return t;
}

Token *copyToken(Token *t) {
  Token *tn = new Token();
  tn->type = t->type;
  tn->start = t->start;
  tn->message = t->message;
  tn->length = t->length;
  tn->line = t->line;
  return tn;
}

Token *errorToken(const char *msg) {
  Token *t = makeToken(TokenType::SCAN_ERROR);
  t->message = msg;
  return t;
}

static char peek() { return *scanner.current; }

static char advance() {
  scanner.current++;
  return scanner.current[-1];
}

static bool match(char expected) {
  if (isEnd())
    return false;
  if (*scanner.current != expected)
    return false;
  advance();
  return true;
}

static bool matchS(std::string e) {
  if (isEnd())
    return false;
  const char *expected = e.c_str();
  int count = e.length();
  // printf("Count:%d\n", count);
  for (int i = 0; i < count - 1; i++) {
    // printf("Current:%c Expected:%c\n", scanner.current[i], expected[i]);
    if (scanner.current[i] != expected[i])
      return false;
  }
  scanner.current += count;
  return true;
}

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      scanner.line++;
      advance();
      break;
    default:
      return;
    }
  }
}

static bool gotoChar(char c) {
  while (!isEnd()) {
    if (peek() == c) {
      advance();
      return true;
    }
    advance();
  }
  return false;
}

static Token *string() {
  while (peek() != '"' && !isEnd()) {
    if (peek() == '\\') {
      if (scanner.current[1] == '\"') {
        advance();
      }
    }
    if (peek() == '\n')
      scanner.line++;
    advance();
  }
  if (isEnd())
    return errorToken("Unterminated string.");
  advance();
  return makeToken(TokenType::STR_LIT);
}

static bool isDigit(char c) { return '0' <= c && c <= '9'; }

static bool isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static Token *number() {
  while (isDigit(peek()))
    advance();
  if (match('.')) {
    if (!isDigit(peek()))
      return errorToken("Expected digit after '.'");
    while (isDigit(peek()))
      advance();
    if (match('e')) {
      match('+');
      match('-');
      if (!isDigit(peek()))
        return errorToken("Expected at least one digit after 'e'");
      while (isDigit(peek()))
        advance();
      return makeToken(TokenType::REAL_LIT);
    }
  }
  return makeToken(TokenType::INT_LIT);
}

static Token *identifier() {
  while (isAlpha(peek()) || isDigit(peek()) || peek() == '_')
    advance();
  return makeToken(TokenType::ID);
}

Token *scanToken() {
  skipWhitespace();
  scanner.start = scanner.current;
  if (isEnd())
    return makeToken(TokenType::SCAN_EOF);
  char c = advance();
  switch (c) {
  case '/':
    if (peek() == '/') {
      gotoChar('\n');
      return makeToken(TokenType::COMMENT);
    }
    return makeToken(TokenType::DIV);
  case '{':
    if (peek() == '*') {
      advance();
      while (gotoChar('*')) {
        if (match('}'))
          return makeToken(TokenType::COMMENT);
      }
    }
    return errorToken("Expected '*}'");
  case '+':
    return makeToken(TokenType::PLUS);
  case '-':
    return makeToken(TokenType::MINUS);
  case '*':
    return makeToken(TokenType::MUL);
  case '%':
    return makeToken(TokenType::MOD);
  case '=':
    return makeToken(TokenType::EQ);
  case '<':
    if (match('>'))
      return makeToken(TokenType::NEQ);
    if (match('='))
      return makeToken(TokenType::LTE);
    return makeToken(TokenType::LT);
  case '>':
    if (match('='))
      return makeToken(TokenType::GTE);
    return makeToken(TokenType::GT);
  case '&':
    return makeToken(TokenType::AND);
  case '(':
    return makeToken(TokenType::LEFT_PAREN);
  case ')':
    return makeToken(TokenType::RIGHT_PAREN);
  case '[':
    return makeToken(TokenType::LEFT_BRACKET);
  case ']':
    return makeToken(TokenType::RIGHT_BRACKET);
  case ':':
    return makeToken(match('=') ? TokenType::ASSIGN : TokenType::COLON);
  case '.':
    return makeToken(TokenType::DOT);
  case ',':
    return makeToken(TokenType::COMMA);
  case ';':
    return makeToken(TokenType::SEMICOLON);
  case 'o':
    if (matchS("r ")) {
      scanner.current--;
      return makeToken(TokenType::OR);
    }
    if (matchS("f ")) {
      scanner.current--;
      return makeToken(TokenType::OF);
    }
    break;
  case 'a':
    if (matchS("nd ")) {
      scanner.current--;
      return makeToken(TokenType::AND);
    }
    if (matchS("rray ")) {
      scanner.current--;
      return makeToken(TokenType::ARRAY);
    }
    if (matchS("ssert ")) {
      scanner.current--;
      return makeToken(TokenType::ASSERT);
    }
    break;
  case 'n':
    if (matchS("ot ")) {
      scanner.current--;
      return makeToken(TokenType::NOT);
    }
    break;
  case 'i':
    if (matchS("f ")) {
      scanner.current--;
      return makeToken(TokenType::IF);
    }
    break;
  case 't':
    if (matchS("hen ")) {
      scanner.current--;
      return makeToken(TokenType::THEN);
    }
    break;
  case 'e':
    if (matchS("lse ")) {
      scanner.current--;
      return makeToken(TokenType::ELSE);
    }
    if (matchS("nd ")) {
      scanner.current--;
      return makeToken(TokenType::END);
    }
    break;
  case 'w':
    if (matchS("hile ")) {
      scanner.current--;
      return makeToken(TokenType::WHILE);
    }
    break;
  case 'd':
    if (matchS("o ")) {
      scanner.current--;
      return makeToken(TokenType::WHILE);
    }
    break;
  case 'b':
    if (matchS("egin ")) {
      scanner.current--;
      return makeToken(TokenType::BEGIN);
    }
    break;
  case 'v':
    if (matchS("ar ")) {
      scanner.current--;
      return makeToken(TokenType::VAR);
    }
    break;
  case 'p':
    if (matchS("rocedure ")) {
      scanner.current--;
      return makeToken(TokenType::PROCEDURE);
    }
    if (matchS("rogram ")) {
      scanner.current--;
      return makeToken(TokenType::PROGRAM);
    }
    break;
  case 'r':
    if (matchS("eturn ")) {
      scanner.current--;
      return makeToken(TokenType::RETURN);
    }
    break;
  case '"':
    return string();
  }
  if (isDigit(c))
    return number();
  if (isAlpha(c))
    return identifier();
  return errorToken("Unexpected character.");
}

} // namespace Scanner
