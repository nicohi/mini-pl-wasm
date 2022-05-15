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
  void visitType(const Parser::Type *i) override { std::cout << "DUMMY"; }
  void visitArray(const Parser::Array *i) override { std::cout << "DUMMY"; }
  void visitBlock(const Parser::Block *i) override {
    std::cout << "(BLOCK\n"
              << "(STATEMENTS\n";
    for (Parser::TreeNode *n : i->statements) {
      n->accept(this);
      // std::cout << std::endl;
    }
    std::cout << ")\n";
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
  void visitVarDecl(const Parser::VarDecl *i) override { std::cout << "DUMMY"; }
  void visitAssign(const Parser::Assign *i) override { std::cout << "DUMMY"; }
  void visitCall(const Parser::Call *i) override { std::cout << "DUMMY"; }
  void visitReturn(const Parser::Return *i) override { std::cout << "DUMMY"; }
  void visitRead(const Parser::Read *i) override { std::cout << "DUMMY"; }
  void visitWrite(const Parser::Write *i) override { std::cout << "DUMMY"; }
  void visitAssert(const Parser::Assert *i) override { std::cout << "DUMMY"; }
  void visitIf(const Parser::If *i) override { std::cout << "DUMMY"; }
  void visitWhile(const Parser::While *i) override { std::cout << "DUMMY"; }
  void visitExpr(const Parser::Expr *i) override { std::cout << "DUMMY"; }
  void visitSimpleExpr(const Parser::SimpleExpr *i) override {
    std::cout << "DUMMY";
  }
  void visitRelationalOperator(const Parser::RelationalOperator *i) override {
    std::cout << "DUMMY";
  }
  void visitAddingOperator(const Parser::AddingOperator *i) override {
    std::cout << "DUMMY";
  }
  void visitTerm(const Parser::Term *i) override { std::cout << "DUMMY"; }
  void visitFactor(const Parser::Factor *i) override { std::cout << "DUMMY"; }
  void visitMultiplyingOperator(const Parser::MultiplyingOperator *i) override {
    std::cout << "DUMMY";
  }
  void visitVariable(const Parser::Variable *i) override {
    std::cout << "DUMMY";
  }
  void visitIntegerLiteral(const Parser::IntegerLiteral *i) override {
    std::cout << "DUMMY";
  }
  void visitRealLiteral(const Parser::RealLiteral *i) override {
    std::cout << "DUMMY";
  }
  void visitStringLiteral(const Parser::StringLiteral *i) override {
    std::cout << "DUMMY";
  }
};

void pprint(Parser::Program *p) {
  PrintWalker *pw = new PrintWalker();
  p->accept(pw);
}

} // namespace ParserUtils
