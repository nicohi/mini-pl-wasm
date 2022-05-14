#ifndef PARSER_H_
#define PARSER_H_

#include "scanner.h"
#include <list>
#include <string>

namespace Parser {

class Program;
class Procedure;
class Function;
class Type;
class Array;
class Block;
class Statement;
class SimpleStatement;
class StructuredStatement;
class VarDecl;
class Assign;
class Call;
class Return;
class Read;
class Write;
class Assert;
class If;
class While;
class Expr;
class SimpleExpr;
class RelationalOperator;
class AddingOperator;
class Term;
class Factor;
class MultiplyingOperator;
class Variable;
class IntegerLiteral;
class RealLiteral;
class StringLiteral;

class TreeWalker {
public:
  virtual void visitProgram(const Program *i) = 0;
  virtual void visitProcedure(const Procedure *i) = 0;
  virtual void visitFunction(const Function *i) = 0;
  virtual void visitType(const Type *i) = 0;
  virtual void visitArray(const Array *i) = 0;
  virtual void visitBlock(const Block *i) = 0;
  virtual void visitStatement(const Statement *i) = 0;
  virtual void visitSimpleStatement(const SimpleStatement *i) = 0;
  virtual void visitStructuredStatement(const StructuredStatement *i) = 0;
  virtual void visitVarDecl(const VarDecl *i) = 0;
  virtual void visitAssign(const Assign *i) = 0;
  virtual void visitCall(const Call *i) = 0;
  virtual void visitReturn(const Return *i) = 0;
  virtual void visitRead(const Read *i) = 0;
  virtual void visitWrite(const Write *i) = 0;
  virtual void visitAssert(const Assert *i) = 0;
  virtual void visitIf(const If *i) = 0;
  virtual void visitWhile(const While *i) = 0;
  virtual void visitExpr(const Expr *i) = 0;
  virtual void visitSimpleExpr(const SimpleExpr *i) = 0;
  virtual void visitRelationalOperator(const RelationalOperator *i) = 0;
  virtual void visitAddingOperator(const AddingOperator *i) = 0;
  virtual void visitTerm(const Term *i) = 0;
  virtual void visitFactor(const Factor *i) = 0;
  virtual void visitMultiplyingOperator(const MultiplyingOperator *i) = 0;
  virtual void visitVariable(const Variable *i) = 0;
  virtual void visitIntegerLiteral(const IntegerLiteral *i) = 0;
  virtual void visitRealLiteral(const RealLiteral *i) = 0;
  virtual void visitStringLiteral(const StringLiteral *i) = 0;
};

class TreeNode {
public:
  virtual void accept(TreeWalker *t) = 0;
};

class Program : public TreeNode {
public:
  void accept(TreeWalker *t) override { t->visitProgram(this); };
};

bool parse(const std::string source);
void parseAndWalk(const std::string source, TreeWalker *tw);

// Stmts *getProgram();

} // namespace Parser

#endif // COMPILER_H_
