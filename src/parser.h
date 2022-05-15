#ifndef PARSER_H_
#define PARSER_H_

#include "scanner.h"
#include <list>
#include <string>

namespace Parser {

class Program;
class Function;
class Parameter;
class Type;
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
class Not;
class Size;
class Term;
class Factor;
class MultiplyingOperator;
class Variable;
class Literal;
class IntegerLiteral;
class RealLiteral;
class StringLiteral;

class TreeWalker {
public:
  virtual void visitProgram(const Program *i) = 0;
  virtual void visitFunction(const Function *i) = 0;
  virtual void visitParameter(const Parameter *i) = 0;
  virtual void visitType(const Type *i) = 0;
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
  virtual void visitNot(const Not *i) = 0;
  virtual void visitSize(const Size *i) = 0;
  virtual void visitTerm(const Term *i) = 0;
  virtual void visitFactor(const Factor *i) = 0;
  virtual void visitMultiplyingOperator(const MultiplyingOperator *i) = 0;
  virtual void visitVariable(const Variable *i) = 0;
  virtual void visitLiteral(const Literal *i) = 0;
  virtual void visitIntegerLiteral(const IntegerLiteral *i) = 0;
  virtual void visitRealLiteral(const RealLiteral *i) = 0;
  virtual void visitStringLiteral(const StringLiteral *i) = 0;
};

class TreeNode {
public:
  virtual void accept(TreeWalker *t) = 0;
};

class Statement : public TreeNode {
public:
  void accept(TreeWalker *t) override { t->visitStatement(this); };
};

class StructuredStatement : public Statement {
public:
  void accept(TreeWalker *t) override { t->visitStructuredStatement(this); };
};

class If : public StructuredStatement {
public:
  Expr *condition;
  Statement *thenBranch;
  Statement *elseBranch;
  void accept(TreeWalker *t) override { t->visitIf(this); };
};

class While : public StructuredStatement {
public:
  Expr *condition;
  Statement *statement;
  void accept(TreeWalker *t) override { t->visitWhile(this); };
};

class VarDecl : public Statement {
public:
  std::list<std::string> ids;
  Type *type;
  void accept(TreeWalker *t) override { t->visitVarDecl(this); };
};

class SimpleStatement : public Statement {
public:
  void accept(TreeWalker *t) override { t->visitSimpleStatement(this); };
};

class Factor : public TreeNode {
public:
  void accept(TreeWalker *t) override { t->visitFactor(this); };
};

class Not : public Factor {
public:
  Factor *factor;
  void accept(TreeWalker *t) override { t->visitNot(this); };
};

class Size : public Factor {
public:
  Factor *factor;
  void accept(TreeWalker *t) override { t->visitSize(this); };
};

class Variable : public Factor {
public:
  std::string id;
  Expr *index;
  void accept(TreeWalker *t) override { t->visitVariable(this); };
};

class Literal : public Factor {
public:
  void accept(TreeWalker *t) override { t->visitLiteral(this); };
};

class IntegerLiteral : public Literal {
public:
  std::string value;
  void accept(TreeWalker *t) override { t->visitIntegerLiteral(this); };
};

class RealLiteral : public Literal {
public:
  std::string value;
  void accept(TreeWalker *t) override { t->visitRealLiteral(this); };
};

class StringLiteral : public Literal {
public:
  std::string value;
  void accept(TreeWalker *t) override { t->visitStringLiteral(this); };
};

class Call : public Factor, public SimpleStatement {
public:
  std::string id;
  std::list<Expr *> arguments;
  void accept(TreeWalker *t) override { t->visitFactor(this); };
};

class Assign : public SimpleStatement {
public:
  std::string id;
  Expr *expression;
  void accept(TreeWalker *t) override { t->visitAssign(this); };
};

class Return : public SimpleStatement {
public:
  Expr *expression;
  void accept(TreeWalker *t) override { t->visitReturn(this); };
};

class Read : public SimpleStatement {
public:
  std::list<Variable *> variables;
  void accept(TreeWalker *t) override { t->visitRead(this); };
};

class Write : public SimpleStatement {
public:
  std::list<Expr *> arguments;
  void accept(TreeWalker *t) override { t->visitWrite(this); };
};

class Assert : public SimpleStatement {
public:
  Expr *expression;
  void accept(TreeWalker *t) override { t->visitAssert(this); };
};

class Term : public TreeNode {
public:
  Factor *factor;
  std::list<std::pair<MultiplyingOperator *, Factor *>> terms;
  void accept(TreeWalker *t) override { t->visitTerm(this); };
};

class AddingOperator : public TreeNode {
public:
  std::string op;
  void accept(TreeWalker *t) override { t->visitAddingOperator(this); };
};

class MultiplyingOperator : public TreeNode {
public:
  std::string op;
  void accept(TreeWalker *t) override { t->visitMultiplyingOperator(this); };
};

class RelationalOperator : public TreeNode {
public:
  std::string op;
  void accept(TreeWalker *t) override { t->visitRelationalOperator(this); };
};

class SimpleExpr : public TreeNode {
public:
  std::string sign = "+";
  Term *term;
  std::list<std::pair<AddingOperator *, Term *>> terms;
  void accept(TreeWalker *t) override { t->visitSimpleExpr(this); };
};

class Expr : public Factor {
public:
  SimpleExpr *left;
  RelationalOperator *op;
  SimpleExpr *right;
  void accept(TreeWalker *t) override { t->visitExpr(this); };
};

class Type : public TreeNode {
public:
  std::string type;
  Expr *size;
  void accept(TreeWalker *t) override { t->visitType(this); };
};

class Block : public StructuredStatement {
public:
  std::list<Statement *> statements;
  void accept(TreeWalker *t) override { t->visitBlock(this); };
};

class Parameter : public TreeNode {
public:
  std::string id;
  Type *type;
  void accept(TreeWalker *t) override { t->visitParameter(this); };
};

class Function : public TreeNode {
public:
  std::string id;
  Type *returnType;
  std::list<Parameter *> parameters;
  Block *block;
  void accept(TreeWalker *t) override { t->visitFunction(this); };
};

class Program : public TreeNode {
public:
  std::string id;
  std::list<Function *> functions;
  Block *block;
  // void appendFunction(Function *f) { functions.push_back(f); }
  void accept(TreeWalker *t) override { t->visitProgram(this); };
};

bool parse(const std::string source);
void parseAndWalk(const std::string source, TreeWalker *tw);

// Stmts *getProgram();

} // namespace Parser

#endif // COMPILER_H_
