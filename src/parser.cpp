#include "parser.h"
#include "compiler.h"
#include "parser_utils.h"
#include <cstdio>
#include <iostream>
#include <map>

namespace Parser {

struct ParserState {
  Scanner::Token *current;
  Scanner::Token *previous;
  bool hadError = false;
  bool panicMode = false;
};

ParserState parser;

Program *prog;

enum class Precedence {
  NONE,
  ASSIGN, // :=
  AND,    // &
  EQUAL,  // =
  TERM,   // + -
  FACTOR, // * /
  UNARY,  // - !
  PRIMARY
};

typedef void (*ParseFn)();

struct ParseRule {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
};
std::map<Scanner::TokenType, ParseRule> rules;

static void print(Scanner::Token *t) {
  fprintf(stderr, "'%.*s'", t->length, t->start);
}

static void printCurrent(std::string msg) {
  std::cout << msg << Scanner::getName(parser.current) << std::endl;
}

static bool isCurrent(Scanner::TokenType t) {
  return parser.current->type == t;
}

static void errorAt(Scanner::Token *t, std::string msg) {
  if (parser.panicMode)
    return;
  parser.panicMode = true;
  fprintf(stderr, "[line %d] Error", t->line);

  if (t->type == Scanner::TokenType::SCAN_EOF) {
    fprintf(stderr, " at end");
  } else if (t->type == Scanner::TokenType::SCAN_ERROR) {
    fprintf(stderr, " %s", t->message);
  } else {
    fprintf(stderr, " at '%.*s'", t->length, t->start);
  }

  fprintf(stderr, ": %s\n", msg.c_str());
  parser.hadError = true;
}

static void advance() {
  parser.previous = parser.current;
  for (;;) {
    parser.current = Scanner::scanToken();
    if (!isCurrent(Scanner::TokenType::SCAN_ERROR))
      break;
    errorAt(parser.current, "Scanner error");
  }
}

static void consume(Scanner::TokenType type, std::string msg) {
  if (parser.current->type == type) {
    advance();
    return;
  }
  errorAt(parser.current, msg);
}

static void exitPanic() {
  while (!isCurrent(Scanner::TokenType::SEMICOLON)) {
    if (isCurrent(Scanner::TokenType::SCAN_EOF))
      break;
    std::cout << "Skipping token:" << Scanner::getName(parser.current)
              << std::endl;
    advance();
  }
  parser.panicMode = false;
}

static bool isBinaryOp() {
  return isCurrent(Scanner::TokenType::PLUS) ||
         isCurrent(Scanner::TokenType::MINUS) ||
         isCurrent(Scanner::TokenType::MUL) ||
         isCurrent(Scanner::TokenType::DIV) ||
         isCurrent(Scanner::TokenType::LT) ||
         isCurrent(Scanner::TokenType::GT) ||
         isCurrent(Scanner::TokenType::LTE) ||
         isCurrent(Scanner::TokenType::GTE) ||
         isCurrent(Scanner::TokenType::EQ) ||
         isCurrent(Scanner::TokenType::NEQ) ||
         isCurrent(Scanner::TokenType::AND) ||
         isCurrent(Scanner::TokenType::OR);
}

static bool isUnaryOp() { return isCurrent(Scanner::TokenType::NOT); }

// static bool isType() {
//   return isCurrent(Scanner::TokenType::INT) ||
//          isCurrent(Scanner::TokenType::STRING) ||
//          isCurrent(Scanner::TokenType::BOOL);
// }
// static Expr *expression();

// static Opnd *operand() {
//   if (isCurrent(Scanner::TokenType::INT_LIT)) {
//     advance();
//     return new Int(parser.previous);
//   }
//   if (isCurrent(Scanner::TokenType::STR_LIT)) {
//     advance();
//     return new String(parser.previous);
//   }
//   if (isCurrent(Scanner::TokenType::ID)) {
//     advance();
//     return new Ident(parser.previous);
//   }
//   consume(Scanner::TokenType::LEFT_PAREN,
//           "Expected literal, identifier, or '('");
//   Expr *e = expression();
//   consume(Scanner::TokenType::RIGHT_PAREN, "Expected ')'");
//   return e;
// }

// static Expr *expression() {
//   if (isUnaryOp()) {
//     advance();
//     Scanner::Token *op = Scanner::copyToken(parser.previous);
//     return new Unary(op, operand());
//   }
//   Opnd *left = operand();
//   if (isBinaryOp()) {
//     advance();
//     // print(parser.previous);
//     // printCurrent("");
//     // std::cout << "\n";
//     Scanner::Token *op = Scanner::copyToken(parser.previous);
//     return new Binary(left, op, expression());
//   } else
//     return new Single(left);
// }

// static Var *var() {
//   advance();
//   Var *v = new Var();
//   consume(Scanner::TokenType::ID, "Expected an identifier after 'var'");
//   v->ident = parser.previous;
//   consume(Scanner::TokenType::COLON, "Expected an ':' after identifier");
//   // if (isType()) {
//   //   v->type = parser.current;
//   //   advance();
//   // } else {
//   //   errorAt(parser.current, "Expected type after ':'");
//   // }
//   if (isCurrent(Scanner::TokenType::ASSIGN)) {
//     advance();
//     v->expr = expression();
//   }
//   return v;
// }

// static Assign *assign() {
//   advance();
//   Scanner::Token *id = parser.previous;
//   consume(Scanner::TokenType::ASSIGN, "Expected ':=' after identifier");
//   Expr *e = expression();
//   return new Assign(id, e);
// }

// static Print *print() {
//   advance();
//   Print *p = new Print();
//   p->expr = expression();
//   return p;
// }

// static Read *read() {
//   advance();
//   consume(Scanner::TokenType::ID, "Expected identifier after read");
//   return new Read(parser.previous);
// }

// static Assert *assert() {
//   advance();
//   consume(Scanner::TokenType::LEFT_PAREN, "Expected '(' after assert");
//   Expr *e = expression();
//   consume(Scanner::TokenType::RIGHT_PAREN,
//           "Expected ')' after assert expression");
//   return new Assert(e);
// }

// static Stmts *statements();
// // static For *forLoop() {
// //   advance();
// //   consume(Scanner::TokenType::IDENTIFIER, "Expected identifier after
// for");
// //   Scanner::Token *id = parser.previous;
// //   consume(Scanner::TokenType::IN, "Expected 'in' after identifier");
// //   Expr *from = expression();
// //   consume(Scanner::TokenType::RANGE, "Expected '..' after expression");
// //   Expr *to = expression();
// //   consume(Scanner::TokenType::DO, "Expected 'do' after expression");
// //   Stmts *body = statements();
// //   consume(Scanner::TokenType::END, "Expected 'end' after loop body");
// //   consume(Scanner::TokenType::FOR, "Expected 'for' after end");
// //   return new For(id, from, to, body);
// // }

// static Stmt *statement() {
//   Stmt *s = new Stmt();
//   if (isCurrent(Scanner::TokenType::VAR)) {
//     s = var();
//   } else if (isCurrent(Scanner::TokenType::ID)) {
//     s = assign();
//   } else if (isCurrent(Scanner::TokenType::ASSERT)) {
//     s = assert();
//   } else
//     exitPanic();
//   consume(Scanner::TokenType::SEMICOLON, "Expected ';' at end of statement");
//   return s;
// }

static Program *program() {
  Program *p = new Program();
  for (;;) {
    if (isCurrent(Scanner::TokenType::COMMENT)) {
      advance();
      continue;
    }
    if (isCurrent(Scanner::TokenType::SCAN_ERROR)) {
      errorAt(parser.current, parser.current->message);
      exitPanic();
      continue;
    }
    if (isCurrent(Scanner::TokenType::SCAN_EOF) ||
        isCurrent(Scanner::TokenType::END)) {
      break;
    }
    advance();
  }
  return p;
}

bool parse(const std::string source) {
  Scanner::init(source);
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  prog = program();
  consume(Scanner::TokenType::SCAN_EOF, "");
  ParserUtils::pprint(prog);
  return !parser.hadError;
}

void parseAndWalk(const std::string source, TreeWalker *tw) {
  Scanner::init(source);
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  prog = program();
  consume(Scanner::TokenType::SCAN_EOF, "");
  if (!parser.hadError)
    prog->accept(tw);
}

} // namespace Parser
