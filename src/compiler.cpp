#include "compiler.h"
#include "parser.h"
#include "parser_utils.h"
#include "scanner.h"
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

namespace Compiler {
Program *ir;
std::string out;

std::string toTypeStr(Parser::Type *t) {
  return t->type + (t->isArray ? "_arr" : "");
}
// visitor used for creating IR from parse tree
class ParseTreeWalker : public Parser::TreeWalker {
public:
  IRNode *previous;
  IRNode *next;
  void visitProgram(const Parser::Program *i) override {
    ir->name = i->id;
    for (Parser::Function *f : i->functions) {
      f->accept(this);
      ir->functions.push_back((Function *)next);
    }
    i->block->accept(this);
    ir->scope = (Scope *)next;
  }
  void visitFunction(const Parser::Function *i) override {
    Function *f = new Function();
    f->type = toTypeStr(i->returnType);
    f->name = i->id;
    for (Parser::Statement *s : i->block->statements) {
      s->accept(this);
      f->scope->statements.push_back((Statement *)next);
    }
    next = f;
  }
  void visitParameter(const Parser::Parameter *i) override {
    std::cout << "PARAMETER";
  }
  void visitType(const Parser::Type *i) override { std::cout << "TYPE"; }
  void visitBlock(const Parser::Block *i) override {
    Scope *scope = new Scope();
    for (Parser::Statement *s : i->statements) {
      s->accept(this);
      scope->statements.push_back((Statement *)next);
    }
    next = scope;
  }
  void visitStatement(const Parser::Statement *i) override {
    std::cout << "STATEMENT";
  }
  void visitSimpleStatement(const Parser::SimpleStatement *i) override {
    std::cout << "SIMPLESTATEMENT";
  }
  void visitStructuredStatement(const Parser::StructuredStatement *i) override {
    std::cout << "STRUCTUREDSTATEMENT";
  }
  void visitVarDecl(const Parser::VarDecl *i) override {
    Declare *d = new Declare();
    d->names = i->ids;
    d->type = toTypeStr(i->type);
    next = d;
  }

  void visitAssign(const Parser::Assign *i) override {
    Assign *a = new Assign();
    a->name = i->id;
    i->expression->accept(this);
    a->expr = (Expr *)next;
    next = a;
  }
  void visitCall(const Parser::Call *i) override {
    Call *c = new Call();
    c->name = i->id;
    for (auto a : i->arguments) {
      a->accept(this);
      c->args.push_back((Expr *)next);
    }
    next = c;
  }
  void visitReturn(const Parser::Return *i) override { std::cout << "RETURN"; }
  void visitRead(const Parser::Read *i) override { std::cout << "READ"; }
  void visitWrite(const Parser::Write *i) override {
    Call *c = new Call();
    c->name = "writeln";
    for (auto a : i->arguments) {
      a->accept(this);
      c->args.push_back((Expr *)next);
    }
    next = c;
  }
  void visitAssert(const Parser::Assert *i) override { std::cout << "ASSERT"; }
  void visitIf(const Parser::If *i) override { std::cout << "IF"; }
  void visitWhile(const Parser::While *i) override { std::cout << "WHILE"; }
  void visitExpr(const Parser::Expr *i) override {
    if (i->op) {
      BinaryOp *b = new BinaryOp();
      b->op = i->op->op;
      i->left->accept(this);
      b->left = (Expr *)next;
      i->right->accept(this);
      b->right = (Expr *)next;
      next = b;
      return;
    }
    i->left->accept(this);
  }
  void visitSimpleExpr(const Parser::SimpleExpr *i) override {
    if (i->terms.size() == 0) {
      i->term->accept(this);
      return;
    }
    i->term->accept(this);
    Expr *prev = (Expr *)next;
    for (auto t : i->terms) {
      BinaryOp *b = new BinaryOp();
      b->left = prev;
      b->op = t.first->op;
      t.second->accept(this);
      b->right = (Expr *)next;
      prev = b;
    }
    next = prev;
  }
  void visitRelationalOperator(const Parser::RelationalOperator *i) override {
    std::cout << "RO" << i->op << " ";
  }
  void visitAddingOperator(const Parser::AddingOperator *i) override {
    std::cout << "AO" << i->op << " ";
  }
  void visitNot(const Parser::Not *i) override { std::cout << "NOT"; }
  void visitSize(const Parser::Size *i) override { std::cout << "SIZE"; }
  void visitTerm(const Parser::Term *i) override {
    if (i->factors.size() == 0) {
      i->factor->accept(this);
      return;
    }
    i->factor->accept(this);
    Expr *prev = (Expr *)next;
    for (auto t : i->factors) {
      BinaryOp *b = new BinaryOp();
      b->left = prev;
      b->op = t.first->op;
      t.second->accept(this);
      b->right = (Expr *)next;
      prev = b;
    }
    next = prev;
  }
  void visitFactor(const Parser::Factor *i) override { std::cout << "FACTOR"; }
  void visitMultiplyingOperator(const Parser::MultiplyingOperator *i) override {
    std::cout << "MO" << i->op << " ";
  }
  void visitVariable(const Parser::Variable *i) override {
    Variable *v = new Variable();
    v->name = i->id;
    if (i->index) {
      i->index->accept(this);
      v->index = (Expr *)next;
    }
    next = v;
  }
  void visitLiteral(const Parser::Literal *i) override { std::cout << "LIT"; }
  void visitIntegerLiteral(const Parser::IntegerLiteral *i) override {
    Literal *l = new Literal();
    l->value = i->value;
    l->type = "integer";
    next = l;
  }
  void visitRealLiteral(const Parser::RealLiteral *i) override {
    Literal *l = new Literal();
    l->value = i->value;
    l->type = "real";
    next = l;
  }
  void visitStringLiteral(const Parser::StringLiteral *i) override {
    Literal *l = new Literal();
    l->value = i->value;
    l->type = "string";
    next = l;
  }
};

// Runs only the scanner and prints to stdout
void runScanner(const std::string source) {
  Scanner::init(source);
  int line = -1;
  for (;;) {
    Scanner::Token *token = Scanner::scanToken();
    if (token->line != line) {
      printf("%4d ", token->line);
      line = token->line;
    } else {
      printf("   | ");
    }
    printf("%-12s '%.*s' %s\n", Scanner::getName(token).c_str(), token->length,
           token->start, token->message);

    if (token->type == Scanner::TokenType::SCAN_EOF)
      break;
  }
}

class Printer : public IRVisitor {
public:
  void visitProgram(const Program *i) override {
    std::cout << "PROGRAM\n";
    std::cout << "FUNCTIONS\n";
    for (auto f : i->functions)
      f->accept(this);
    std::cout << "END_FUNCTIONS\n";
    std::cout << "MAIN\n";
    i->scope->accept(this);
    std::cout << "END_MAIN\n";
    std::cout << "END_PROGRAM:\n";
  }
  void visitFunction(const Function *i) override { std::cout << "FUNCTION\n"; }
  void visitStatement(const Statement *i) override {
    std::cout << "STATEMENT\n";
  }
  void visitScope(const Scope *i) override {
    std::cout << "SCOPE\n";
    for (auto f : i->statements) {
      std::cout << "\t";
      f->accept(this);
    }
    std::cout << "END_SCOPE\n";
  }
  void visitIf(const If *i) override { std::cout << "IF\n"; }
  void visitWhile(const While *i) override { std::cout << "WHILE\n"; }
  void visitExpr(const Expr *i) override { std::cout << "EXPR\n"; }
  void visitDeclare(const Declare *i) override {
    std::cout << "DECLARE:" << i->type << ":";
    for (auto n : i->names) {
      std::cout << n << ",";
    }
    std::cout << "\n";
  }
  void visitAssign(const Assign *i) override {
    std::cout << "ASSIGN " << i->name << ":"
              << " ";
    i->expr->accept(this);
    std::cout << "\n";
  }
  void visitCall(const Call *i) override { std::cout << "CALL\n"; }
  void visitReturn(const Return *i) override { std::cout << "RETURN\n"; }
  void visitRead(const Read *i) override { std::cout << "READ\n"; }
  void visitAssert(const Assert *i) override { std::cout << "ASSERT\n"; }
  void visitUnaryOp(const UnaryOp *i) override { std::cout << "UNARYOP\n"; }
  void visitBinaryOp(const BinaryOp *i) override {
    std::cout << "BINARYOP:" << i->type << ":";
    i->left->accept(this);
    std::cout << i->op;
    i->right->accept(this);
    //
  }
  void visitVariable(const Variable *i) override { std::cout << "VARIABLE"; }
  void visitLiteral(const Literal *i) override { std::cout << "LITERAL"; }
};

std::string toArgType(std::list<Expr *> args) {
  std::string s = args.front()->type;
  int skip = 0;
  for (auto a : args) {
    if (skip < 1) {
      skip++;
      continue;
    }
    s = s + "_" + a->type;
  }
  return s;
}

std::list<std::string> vars;
class Decorator : public IRVisitor {
public:
  std::map<std::string, std::string> tab;
  void visitProgram(const Program *i) override {
    for (Function *f : i->functions) {
      f->accept(this);
      for (auto e : f->errors_out)
        i->appendError(e);
    }
    i->scope->accept(this);
    for (auto e : i->scope->errors_out)
      i->appendError(e);
    i->symtab = tab;
  }
  void visitFunction(const Function *i) override { std::cout << "FUNCTION\n"; }
  void visitStatement(const Statement *i) override {
    std::cout << "STATEMENT\n";
  }
  void visitScope(const Scope *i) override {
    std::map<std::string, std::string> outer(i->symtab);
    tab = i->symtab;
    for (auto s : i->statements) {
      s->accept(this);
    }
    tab = outer;
  }
  void visitIf(const If *i) override { std::cout << "IF\n"; }
  void visitWhile(const While *i) override { std::cout << "WHILE\n"; }
  void visitExpr(const Expr *i) override { std::cout << i->type << "EXPR\n"; }
  void visitDeclare(const Declare *i) override {
    for (auto n : i->names) {
      if (tab.count(n)) {
        i->appendError_out("At declare" + n + " already in scope.\n");
      }
      tab[n] = i->type;
      vars.push_back(n);
    }
  }
  void visitAssign(const Assign *i) override {
    if (!tab.count(i->name)) {
      i->appendError_out("At assign" + i->name + " not in scope.\n");
    }
    i->expr->accept(this);
    for (auto e : i->expr->errors_out)
      i->appendError_out(e);
    if (i->expr->type.compare(tab[i->name]) != 0) {
      i->appendError_out("At assign" + i->name + " is of type" + i->type +
                         " not " + i->expr->type + ".\n");
    }
  }
  void visitCall(const Call *i) override {
    for (auto a : i->args) {
      a->accept(this);
      for (auto e : a->errors_out)
        i->appendError_out(e);
    }
    i->type = toArgType(i->args);
  }
  void visitReturn(const Return *i) override { std::cout << "RETURN\n"; }
  void visitRead(const Read *i) override { std::cout << "READ\n"; }
  void visitAssert(const Assert *i) override { std::cout << "ASSERT\n"; }
  void visitUnaryOp(const UnaryOp *i) override { std::cout << "UNARYOP\n"; }
  void visitBinaryOp(const BinaryOp *i) override {
    i->left->accept(this);
    i->right->accept(this);
    if (i->left->type.compare(i->right->type) != 0) {
      i->appendError_out("At binaryOP" + i->right->name + " is of type" +
                         i->right->type + " not " + i->left->type + ".\n");
    }
    i->type = i->left->type;
    for (auto e : i->left->errors_out)
      i->appendError_out(e);
    for (auto e : i->right->errors_out)
      i->appendError_out(e);
  }
  void visitVariable(const Variable *i) override {
    if (!tab.count(i->name))
      i->appendError_out("Variable " + i->name + " not in scope");
  }
  void visitLiteral(const Literal *i) override {}
};

void emitLine(std::string l) { out = out + l + "\n"; }

class Generator : public IRVisitor {
public:
  int free = 0;
  std::map<std::string, int> addr;

  void claimAddr(std::string name, std::string type) {
    if (type.compare("integer") == 0) {
      int a = free;
      free++;
      emitLine("(data (i32.const " + std::to_string(a) + "))");
      addr[name] = a;
    } else if (type.compare("string") == 0) {
      int a = free;
      free += 20;
      emitLine("(data (i32.const " + std::to_string(a) +
               ") \"                    \")");
      addr[name] = a;
    } else
      std::cout << "Unknown type:" << type << std::endl;
  }

  void visitProgram(const Program *i) override {
    for (auto f : i->functions)
      f->accept(this);
    emitLine("(func (export \"main\") ");
    // emitLine(" i32.const 10");
    // int in = 0;
    for (auto n : vars) {
      // claimAddr(s.first, s.second);
      std::cout << n << i->symtab[n] << std::endl;
      if (i->symtab[n].compare("integer") == 0)
        emitLine("(local $" + n + " i32)");
      // in++;
    }
    emitLine(" i32.const 10");
    emitLine(" call $writeln_string");
    i->scope->accept(this);
    emitLine(")");
  }
  void visitFunction(const Function *i) override { std::cout << "FUNCTION\n"; }
  void visitStatement(const Statement *i) override {
    std::cout << "STATEMENT\n";
  }
  void visitScope(const Scope *i) override {
    std::cout << "SCOPE\n";
    for (auto f : i->statements) {
      std::cout << "\t";
      f->accept(this);
    }
    std::cout << "END_SCOPE\n";
  }
  void visitIf(const If *i) override { std::cout << "IF\n"; }
  void visitWhile(const While *i) override { std::cout << "WHILE\n"; }
  void visitExpr(const Expr *i) override { std::cout << "EXPR\n"; }
  void visitDeclare(const Declare *i) override {
    std::cout << "DECLARE:" << i->type << ":";
    for (auto n : i->names) {
      std::cout << n << ",";
    }
    std::cout << "\n";
  }
  void visitAssign(const Assign *i) override {
    std::cout << "ASSIGN " << i->name << ":"
              << " ";
    i->expr->accept(this);
    std::cout << "\n";
  }
  void visitCall(const Call *i) override { std::cout << "CALL\n"; }
  void visitReturn(const Return *i) override { std::cout << "RETURN\n"; }
  void visitRead(const Read *i) override { std::cout << "READ\n"; }
  void visitAssert(const Assert *i) override { std::cout << "ASSERT\n"; }
  void visitUnaryOp(const UnaryOp *i) override { std::cout << "UNARYOP\n"; }
  void visitBinaryOp(const BinaryOp *i) override {
    std::cout << "BINARYOP:" << i->type << ":";
    i->left->accept(this);
    std::cout << i->op;
    i->right->accept(this);
    //
  }
  void visitVariable(const Variable *i) override { std::cout << "VARIABLE"; }
  void visitLiteral(const Literal *i) override { std::cout << "LITERAL"; }
};

// uses visitor to traverse IR and do semantic checks
void decorateIR() {
  Decorator *d = new Decorator();
  ir->accept(d);
  Printer *p = new Printer();
  // ir->accept(p);
}

// converts parse tree into AST/IR
void createIR(Parser::Program *p) {
  ir = new Program();
  ParseTreeWalker *ptw = new ParseTreeWalker();
  p->accept(ptw);
}

std::string read_lib() {
  std::string path = "src/wasmlib/wasmlib.wat";
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return (contents);
  }
  throw(errno);
}
void write_out(std::string s) {
  std::ofstream myfile;
  myfile.open("out.wat");
  myfile << s;
  myfile.close();
}
void generate() {
  std::string lib = read_lib();
  Generator *g = new Generator();
  ir->accept(g);
  std::string fin = "(module \n" + lib + "\n" + out + ")";
  write_out(fin);
}

void runParser(const std::string source) { Parser::parse(source); }
void compile(const std::string source) {
  //
  Parser::Program *p;
  if (Parser::parse(source, &p)) {
    // ParserUtils::pprint(p);
    createIR(p);
    decorateIR();
    if (ir->errors.size() == 0)
      generate();
  } else {
    std::cout << "PARSE ERROR, NO OUTPUT\n";
  }
}

} // namespace Compiler
