#ifndef SCANNER_H_
#define SCANNER_H_

#include "string"

namespace Scanner {

#define TOKEN_TYPES(F)                                                         \
  F(ID, "<letter> { <letter> | <digit> | '_' }")                               \
  F(INT_LIT, "<digits>")                                                       \
  F(REAL_LIT, "<digits>'.' <digits> [ 'e' [ <sign> ] <digits>]")               \
  F(STR_LIT, "\"<chars>\"")                                                    \
  F(PLUS, "+")                                                                 \
  F(MINUS, "-")                                                                \
  F(MUL, "*")                                                                  \
  F(DIV, "/")                                                                  \
  F(MOD, "%")                                                                  \
  F(EQ, "=")                                                                   \
  F(NEQ, "<>")                                                                 \
  F(LT, "<")                                                                   \
  F(GT, ">")                                                                   \
  F(LTE, "<=")                                                                 \
  F(GTE, ">=")                                                                 \
  F(LEFT_PAREN, "(")                                                           \
  F(RIGHT_PAREN, ")")                                                          \
  F(LEFT_BRACKET, "[")                                                         \
  F(RIGHT_BRACKET, "]")                                                        \
  F(ASSIGN, ":=")                                                              \
  F(DOT, ".")                                                                  \
  F(COMMA, ",")                                                                \
  F(SEMICOLON, ";")                                                            \
  F(COLON, ":")                                                                \
  F(OR, "or")                                                                  \
  F(AND, "and")                                                                \
  F(NOT, "not")                                                                \
  F(IF, "if")                                                                  \
  F(THEN, "then")                                                              \
  F(ELSE, "else")                                                              \
  F(OF, "of")                                                                  \
  F(WHILE, "while")                                                            \
  F(DO, "do")                                                                  \
  F(BEGIN, "begin")                                                            \
  F(END, "end")                                                                \
  F(VAR, "var")                                                                \
  F(ARRAY, "array")                                                            \
  F(PROCEDURE, "procedure")                                                    \
  F(FUNCTION, "function")                                                      \
  F(PROGRAM, "program")                                                        \
  F(ASSERT, "assert")                                                          \
  F(RETURN, "return")                                                          \
  F(COMMENT, "// | {* ... *}")                                                 \
  F(SCAN_ERROR, "")                                                            \
  F(SCAN_EOF, "")

#define F(name, desc) name,
enum class TokenType { TOKEN_TYPES(F) };
#undef F

struct Token {
  TokenType type;
  const char *start;
  const char *message;
  int length;
  int line;
};

void init(const std::string source);
std::string getName(Token *t);
std::string getName(TokenType t);
Token *scanToken();
Token *errorToken(const char *msg);
Token *copyToken(Token *t);

} // namespace Scanner

#endif // SCANNER_H_
