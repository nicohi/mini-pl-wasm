#include "parser_utils.h"
#include <iostream>

namespace ParserUtils {
class PrintWalker : public Parser::TreeWalker {
public:
  void visitProgram(const Parser::Program *i) override {
    std::cout << "(PROGRAM " << i->id;
    std::cout << std::endl << "(FUNCTIONS\n";
    for (Parser::TreeNode *n : i->functions) {
      n->accept(this);
      // std::cout << std::endl;
    }
    std::cout << ")\n";
    std::cout << "MAIN:\n";
    i->block->accept(this);
    std::cout << ")\n";
  }
  void visitFunction(const Parser::Function *i) override {
    std::cout << "DUMMY";
  }
  void visitParameter(const Parser::Parameter *i) override {
    std::cout << "DUMMY";
  }
  void visitType(const Parser::Type *i) override {
    std::cout << "(TYPE " << i->type;
    if (i->isArray) {
      std::cout << " SIZE:";
      i->size->accept(this);
    }
    std::cout << ")";
  }
  void visitBlock(const Parser::Block *i) override {
    std::cout << "(BLOCK\n"
              << "(STATEMENTS\n\n";
    for (Parser::TreeNode *n : i->statements) {
      n->accept(this);
      // std::cout << std::endl;
    }
    std::cout << "\n)\n";
    std::cout << ")\n";
  }
  void visitStatement(const Parser::Statement *i) override {
    std::cout << "DUMMY";
  }
  void visitSimpleStatement(const Parser::SimpleStatement *i) override {
    std::cout << "DUMMY";
  }
  void visitStructuredStatement(const Parser::StructuredStatement *i) override {
    std::cout << "DUMMY";
  }
  void visitVarDecl(const Parser::VarDecl *i) override {
    std::cout << "(DECLARE";
    for (auto s : i->ids)
      std::cout << " " << s;
    std::cout << " ";
    i->type->accept(this);
    std::cout << ")\n";
  }

  void visitAssign(const Parser::Assign *i) override {
    std::cout << "(ASSIGN " << i->id << " ";
    i->expression->accept(this);
    std::cout << ")\n";
  }
  void visitCall(const Parser::Call *i) override { std::cout << "CALL"; }
  void visitReturn(const Parser::Return *i) override { std::cout << "RETURN"; }
  void visitRead(const Parser::Read *i) override { std::cout << "READ"; }
  void visitWrite(const Parser::Write *i) override {
    std::cout << "(WRITE";
    for (auto a : i->arguments)
      a->accept(this);
    std::cout << ")\n";
  }
  void visitAssert(const Parser::Assert *i) override { std::cout << "ASSERT"; }
  void visitIf(const Parser::If *i) override { std::cout << "IF"; }
  void visitWhile(const Parser::While *i) override { std::cout << "WHILE"; }
  void visitExpr(const Parser::Expr *i) override {
    std::cout << "(EXPR ";
    std::cout << "(";
    i->left->accept(this);
    std::cout << ")";
    if (i->op) {
      i->op->accept(this);
      std::cout << "(";
      i->right->accept(this);
      std::cout << ")";
    }
    std::cout << ") ";
  }
  void visitSimpleExpr(const Parser::SimpleExpr *i) override {
    std::cout << "(SEXPR ";
    i->term->accept(this);
    for (auto t : i->terms) {
      t.first->accept(this);
      t.second->accept(this);
    }
    std::cout << ") ";
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
    std::cout << "(TERM ";
    i->factor->accept(this);
    for (auto f : i->factors) {
      f.first->accept(this);
      f.second->accept(this);
    }
    std::cout << ") ";
  }
  void visitFactor(const Parser::Factor *i) override {
    std::cout << "(FACTOR";
    std::cout << ") ";
  }
  void visitMultiplyingOperator(const Parser::MultiplyingOperator *i) override {
    std::cout << "MO" << i->op << " ";
  }
  void visitVariable(const Parser::Variable *i) override {
    std::cout << "VAR:" << i->id << " ";
  }
  void visitLiteral(const Parser::Literal *i) override { std::cout << "LIT"; }
  void visitIntegerLiteral(const Parser::IntegerLiteral *i) override {
    std::cout << "INT:" << i->value << " ";
  }
  void visitRealLiteral(const Parser::RealLiteral *i) override {
    std::cout << "REAL:" << i->value << " ";
  }
  void visitStringLiteral(const Parser::StringLiteral *i) override {
    std::cout << "STR:" << i->value << " ";
  }
};

void pprint(Parser::TreeNode *p) {
  PrintWalker *pw = new PrintWalker();
  p->accept(pw);
}

} // namespace ParserUtils
