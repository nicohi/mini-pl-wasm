#include "parser.h"
#include "compiler.h"
#include "parser_utils.h"
#include <cstdio>
#include <iostream>
#include <map>

// TMP
#include <unistd.h>

#define S Scanner
#define T Scanner::TokenType

namespace Parser {

struct ParserState {
  Scanner::Token *current;
  Scanner::Token *previous;
  bool hadError = false;
  bool panicMode = false;
};

ParserState parser;

Program *prog;

static std::string readCurrent() {
  std::string c(parser.current->start, parser.current->length);
  return c;
}

static std::string readPrevious() {
  std::string c(parser.previous->start, parser.previous->length);
  return c;
}

static void print(Scanner::Token *t) {
  fprintf(stderr, "'%.*s'", t->length, t->start);
}

static void printCurrent(std::string msg) {
  std::cout << msg << Scanner::getName(parser.current) << std::endl;
}

static bool isPrevious(Scanner::TokenType t) {
  return parser.previous->type == t;
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

static bool consume(Scanner::TokenType type, std::string msg) {
  if (parser.current->type == type) {
    advance();
    return true;
  }
  errorAt(parser.current, msg);
  return false;
}

static void exitPanic() {
  while (!isCurrent(Scanner::TokenType::SEMICOLON)) {
    if (isCurrent(Scanner::TokenType::SCAN_EOF))
      break;
    std::cout << "Skipping token:" << Scanner::getName(parser.current)
              << std::endl;
    advance();
  }
  advance();
  parser.panicMode = false;
}

static bool isSign() { return isCurrent(T::PLUS) || isCurrent(T::MINUS); }
static bool isLiteral() {
  return isCurrent(T::INT_LIT) || isCurrent(T::REAL_LIT) ||
         isCurrent(T::STR_LIT);
}
static bool isRelational() {
  return isCurrent(T::EQ) || isCurrent(T::NEQ) || isCurrent(T::LT) ||
         isCurrent(T::LTE) || isCurrent(T::GTE) || isCurrent(T::GT);
}
static bool isAdding() {
  return isCurrent(T::PLUS) || isCurrent(T::MINUS) || isCurrent(T::OR);
}
static bool isMultiplying() {
  return isCurrent(T::MUL) || isCurrent(T::DIV) || isCurrent(T::MOD) ||
         isCurrent(T::AND);
}

static Factor *factor();

static std::list<std::pair<MultiplyingOperator *, Factor *>> factors() {
  std::list<std::pair<MultiplyingOperator *, Factor *>> fs({});
  // std::cout << "AA";
  while (isMultiplying()) {
    MultiplyingOperator *mo = new MultiplyingOperator();
    mo->op = readCurrent();
    advance();
    fs.push_back(std::make_pair(mo, factor()));
  }
  return fs;
}

static Term *term() {
  Term *t = new Term();
  // std::cout << "BB";
  t->factor = factor();
  t->factors = factors();
  return t;
}

static std::list<std::pair<AddingOperator *, Term *>> terms() {
  std::list<std::pair<AddingOperator *, Term *>> ts({});
  while (isAdding()) {
    AddingOperator *ao = new AddingOperator();
    ao->op = readCurrent();
    advance();
    // std::cout << "BB";
    // ParserUtils::pprint(ao);
    ts.push_back(std::make_pair(ao, term()));
  }
  return ts;
}

static SimpleExpr *simpleExpression() {
  SimpleExpr *e = new SimpleExpr();
  if (isSign()) {
    advance();
    e->sign = readPrevious();
  }
  e->term = term();
  e->terms = terms();
  // for (auto a : e->terms)
  // ParserUtils::pprint(a.second);
  return e;
}

static Expr *expression() {
  Expr *e = new Expr();
  e->left = simpleExpression();
  // ParserUtils::pprint(e->left);
  // std::cout << "EP" << readPrevious() << std::endl;
  // std::cout << "EC" << readCurrent() << std::endl;
  if (isRelational()) {
    advance();
    RelationalOperator *op = new RelationalOperator();
    op->op = readPrevious();
    e->op = op;
    e->right = simpleExpression();
  }
  return e;
}

static Variable *variable(std::string s) {
  Variable *v = new Variable();
  v->id = s;
  if (isCurrent(T::LEFT_BRACKET)) {
    v->index = expression();
    consume(T::RIGHT_BRACKET, "Expected ']' after index");
  }
  return v;
}

static std::list<Expr *> arguments();

static Call *call(std::string s);
static Factor *factor() {
  // std::cout << "AA" << readPrevious() << std::endl;
  // std::cout << "BB" << readCurrent() << std::endl;
  //  ParserUtils::pprint(i);
  if (isCurrent(T::LEFT_PAREN)) {
    advance();
    Expr *e = expression();
    consume(T::RIGHT_PAREN, "Expected ')'");
    return e;
  }
  if (isCurrent(T::ID)) {
    advance();
    if (isCurrent(T::LEFT_PAREN)) {
      Call *c = call(readPrevious());
      consume(T::RIGHT_PAREN, "Expected ')' after function call");
      return c;
    }
    return variable(readPrevious());
  }
  if (isCurrent(T::NOT)) {
    Not *i = new Not();
    i->factor = factor();
    return i;
  }
  if (isCurrent(T::INT_LIT)) {
    IntegerLiteral *i = new IntegerLiteral();
    i->value = readCurrent();
    // ParserUtils::pprint(i);
    // usleep(1000000);
    // usleep(10000000);
    advance();
    return i;
  }
  if (isCurrent(T::REAL_LIT)) {
    RealLiteral *i = new RealLiteral();
    i->value = readCurrent();
    advance();
    return i;
  }
  if (isCurrent(T::STR_LIT)) {
    StringLiteral *i = new StringLiteral();
    i->value = readCurrent();
    advance();
    return i;
  }
  Size *i = new Size();
  i->factor = factor();
  consume(T::DOT, "Expected '.'");
  advance();
  if (!(readPrevious().compare("size") == 0))
    errorAt(parser.previous, "Expected 'size' after '.'");
  return i;
}

static Variable *variable() {
  Variable *v = new Variable();
  consume(T::ID, "Expected ID");
  v->id = readPrevious();
  if (isCurrent(T::LEFT_BRACKET)) {
    v->index = expression();
    consume(T::RIGHT_BRACKET, "Expected ']' after index");
  }
  return v;
}

static std::list<Expr *> arguments() {
  std::list<Expr *> as;
  if (isCurrent(T::RIGHT_PAREN))
    return as;
  as.push_back(expression());
  while (!isCurrent(T::RIGHT_PAREN)) {
    if (parser.panicMode) {
      exitPanic();
      return as;
    }
    consume(T::COMMA, "Expected ',' between expressions");
    as.push_back(expression());
  }
  return as;
}

static Write *write() {
  Write *w = new Write();
  consume(T::ID, "Expected writeln");
  consume(T::LEFT_PAREN, "Expected '('");
  w->arguments = arguments();
  consume(T::RIGHT_PAREN, "Expected ')'");
  return w;
}

static Read *read() {
  consume(T::ID, "Expected read");
  consume(T::LEFT_PAREN, "Expected '('");
  Read *r = new Read();
  r->variables.push_back(variable());
  while (!isCurrent(T::RIGHT_PAREN)) {
    if (parser.panicMode) {
      exitPanic();
      return r;
    }
    consume(T::COMMA, "Expected ','");
  }
  advance();
  return r;
}

static Assign *assign(std::string id) {
  consume(T::ASSIGN, "Expected :=");
  Assign *a = new Assign();
  a->id = id;
  a->expression = expression();
  return a;
}

static Call *call(std::string id) {
  Call *c = new Call();
  c->id = id;
  consume(T::LEFT_PAREN, "Expected '('");
  c->arguments = arguments();
  consume(T::RIGHT_PAREN, "Expected ')'");
  return c;
}
static Return *return_() {
  consume(T::RETURN, "Expected 'return'");
  Return *r = new Return();
  r->expression = expression();
  return r;
}
static Assert *assert() {
  consume(T::ASSERT, "Expected 'return'");
  consume(T::LEFT_PAREN, "Expected '('");
  Assert *a = new Assert();
  a->expression = expression();
  consume(T::RIGHT_PAREN, "Expected ')'");
  return a;
}

static SimpleStatement *simpleStatement() {
  if (isCurrent(T::ID)) {
    std::string s = readCurrent();
    if (s.compare("read") == 0)
      return read();
    if (s.compare("writeln") == 0)
      return write();
    advance();
    // std::cout << "P" << readPrevious() << std::endl;
    // std::cout << "C" << readCurrent() << std::endl;
    if (isCurrent(T::ASSIGN))
      return assign(readPrevious());
    return call(readPrevious());
  }
  if (isCurrent(T::RETURN)) {
    return return_();
  }
  if (isCurrent(T::ASSERT)) {
    return assert();
  }
  errorAt(parser.current, "Expected read,writeln,ID,return,assert");
  return new SimpleStatement();
}

static Block *block();

static Type *type();

static VarDecl *varDecl() {
  VarDecl *v = new VarDecl();
  advance();
  consume(T::ID, "Expected identifier");
  v->ids.push_back(readPrevious());
  while (!isCurrent(T::COLON)) {
    if (parser.panicMode) {
      exitPanic();
      break;
    }
    consume(T::COMMA, "Expected ','");
    consume(T::ID, "Expected identifier");
    v->ids.push_back(readPrevious());
  }
  consume(T::COLON, "Expected ':'");
  v->type = type();
  return v;
}

static Statement *statement();

static If *if_() {
  If *i = new If();
  consume(T::IF, "Expected 'if'");
  i->condition = expression();
  consume(T::THEN, "Expected 'then'");
  i->thenBranch = statement();
  if (isCurrent(T::ELSE)) {
    advance();
    i->elseBranch = statement();
  }
  return i;
}

static While *while_() {
  While *w = new While();
  consume(T::WHILE, "Expected 'while'");
  w->condition = expression();
  consume(T::DO, "Expected 'do'");
  w->statement = statement();
  return w;
}

static Statement *statement() {
  //
  if (isCurrent(T::VAR)) {
    return varDecl();
  }
  if (isCurrent(T::IF)) {
    return if_();
  }
  if (isCurrent(T::WHILE)) {
    return while_();
  }
  if (isCurrent(T::BEGIN)) {
    return block();
  }
  return simpleStatement();
}

static Block *block() {
  Block *b = new Block();
  consume(T::BEGIN, "Expected 'begin'");
  while (!isCurrent(T::END)) {
    if (isCurrent(T::SCAN_ERROR) || isCurrent(T::SCAN_EOF)) {
      errorAt(parser.current, parser.current->message);
      exitPanic();
      break;
    }
    b->statements.push_back(statement());
    advance();
    if (isPrevious(T::SEMICOLON) && isCurrent(T::END))
      break;
  }
  if (isCurrent(T::END))
    advance();
  return b;
}

static Parameter *parameter() {
  consume(T::LEFT_PAREN, "Expected (");
  Parameter *p = new Parameter();
  return p;
}

static Type *type() {
  Type *t = new Type();
  if (isCurrent(T::ARRAY)) {
    t->isArray = true;
    advance();
    consume(T::LEFT_BRACKET, "Expected '['");
    t->size = expression();
    consume(T::RIGHT_BRACKET, "Expected ']'");
  }
  consume(T::ID, "Expected identifier");
  t->type = readPrevious();
  return t;
}

static Type *voidType() {
  Type *t = new Type();
  t->type = "void";
  return t;
}

static std::list<Parameter *> parameters() {
  std::list<Parameter *> ps;
  consume(T::LEFT_PAREN, "Expected '('");
  while (!isCurrent(T::RIGHT_PAREN)) {
    ps.push_back(parameter());
  }
  return ps;
}

static Function *function() {
  Function *f = new Function();
  f->returnType = voidType();
  if (isCurrent(T::FUNCTION)) {
    advance();
    consume(T::ID, "Expected identifier");
    f->id = readPrevious();
    f->parameters = parameters();
    consume(T::COLON, "Expected ':'");
    f->returnType = type();
  }
  if (isCurrent(T::PROCEDURE)) {
    advance();
    consume(T::ID, "Expected identifier");
    f->id = readPrevious();
    f->parameters = parameters();
  }
  consume(T::SEMICOLON, "Expected ';'");
  f->block = block();
  consume(T::SEMICOLON, "Expected ';'");
  return f;
}

static std::list<Function *> functions() {
  std::list<Function *> fs;
  if (isCurrent(T::FUNCTION) || isCurrent(T::PROCEDURE)) {
    fs.push_back(function());
  }
  return fs;
}

static Program *program() {
  Program *p = new Program();
  for (;;) {
    // printCurrent("C:");
    // std::cout << std::endl;
    if (isCurrent(T::COMMENT)) {
      advance();
      continue;
    }
    if (isCurrent(T::SCAN_ERROR)) {
      errorAt(parser.current, parser.current->message);
      exitPanic();
      continue;
    }
    if (isCurrent(T::DOT)) {
      advance();
      while (isCurrent(T::COMMENT))
        advance();
      consume(T::SCAN_EOF, "Expected EOF after '.'");
      break;
    }
    if (consume(T::PROGRAM, "Expected 'program'")) {
      consume(T::ID, "Expected identifier");
      p->id = readPrevious();
      consume(T::SEMICOLON, "Expected ';'");
      p->functions = functions();
      p->block = block();
      break;
    }
    exitPanic();
  }
  return p;
}

bool parse(const std::string source) {
  Scanner::init(source);
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  prog = program();
  ParserUtils::pprint(prog);
  return !parser.hadError;
}

void parseAndWalk(const std::string source, TreeWalker *tw) {
  Scanner::init(source);
  parser.hadError = false;
  parser.panicMode = false;
  advance();
  prog = program();
  if (!parser.hadError)
    prog->accept(tw);
}

} // namespace Parser
