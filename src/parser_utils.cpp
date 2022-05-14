#include "parser_utils.h"
#include <iostream>

namespace ParserUtils {
class PrintWalker : public Parser::TreeWalker {
public:
  void printToken(Scanner::Token *t) { printf("%.*s", t->length, t->start); }
  void visitOpnd(const Parser::Opnd *i) override { std::cout << "DUMMYOPND"; }
  void visitInt(const Parser::Int *i) override { printToken(i->value); }
  void visitBool(const Parser::Bool *b) override { printToken(b->value); }
  void visitString(const Parser::String *s) override { printToken(s->value); }
  void visitIdent(const Parser::Ident *i) override { printToken(i->ident); }
  void visitExpr(const Parser::Expr *e) override { std::cout << "DUMMYEXPR"; }
  void visitBinary(const Parser::Binary *b) override {
    std::cout << "BIN";
    std::cout << "(";
    printToken(b->op);
    std::cout << " ";
    b->left->accept(this);
    std::cout << " ";
    b->right->accept(this);
    std::cout << ")";
  }
  void visitUnary(const Parser::Unary *u) override {
    std::cout << "UNA";
    std::cout << "(";
    printToken(u->op);
    std::cout << " ";
    u->right->accept(this);
    std::cout << ")";
  }
  void visitSingle(const Parser::Single *s) override {
    std::cout << "SIN";
    std::cout << "(";
    s->right->accept(this);
    std::cout << ")";
  }
  void visitStmt(const Parser::Stmt *s) override {
    std::cout << "(stmt ";
    std::cout << s->info;
    std::cout << ")";
  }
  void visitStmts(const Parser::Stmts *s) override {
    std::cout << "(stmts\n";
    for (Parser::TreeNode *n : s->stmts) {
      n->accept(this);
      std::cout << std::endl;
    }
    std::cout << ")\n";
  }
  void visitVar(const Parser::Var *v) override {
    std::cout << "(var ident:";
    printToken(v->ident);
    std::cout << " ";
    std::cout << "type:" << Scanner::getName(v->type) << " ";
    if (v->expr) {
      std::cout << "expr:";
      v->expr->accept(this);
    }
    std::cout << ")";
  }
  void visitAssign(const Parser::Assign *a) override {
    std::cout << "(";
    std::cout << "assign ident:";
    printToken(a->ident);
    std::cout << " expr:";
    a->expr->accept(this);
    std::cout << ")";
  }
  void visitFor(const Parser::For *f) override {
    std::cout << "(";
    std::cout << "for ";
    std::cout << "ident:";
    printToken(f->ident);
    std::cout << " from:";
    f->from->accept(this);
    std::cout << " to:";
    f->to->accept(this);
    std::cout << " body:\n";
    f->body->accept(this);
    std::cout << "end for";
    std::cout << ")";
  }
  void visitRead(const Parser::Read *r) override {
    std::cout << "(";
    std::cout << "read expr:";
    printToken(r->ident);
    std::cout << ")";
  }
  void visitPrint(const Parser::Print *p) override {
    std::cout << "(";
    std::cout << "print expr:";
    p->expr->accept(this);
    std::cout << ")";
  }
  void visitAssert(const Parser::Assert *a) override {
    std::cout << "(";
    std::cout << "assert expr:";
    a->expr->accept(this);
    std::cout << ")";
  }
};

void pprint(Parser::Stmts *ss) {
  PrintWalker *pw = new PrintWalker();
  ss->accept(pw);
}

} // namespace ParserUtils
