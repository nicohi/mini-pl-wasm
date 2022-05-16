#ifndef COMPILER_H_
#define COMPILER_H_

#include "parser.h"
#include <list>
#include <map>
#include <string>
#include <vector>

namespace Compiler {

class Program;
class Function;
class Statement;
class Scope;
class If;
class While;
class Expr;
class Declare;
class Assign;
class Call;
class Return;
class Read;
class Assert;
class UnaryOp;
class BinaryOp;
class Variable;
class Literal;

class IRVisitor {
public:
  virtual void visitProgram(const Program *i) = 0;
  virtual void visitFunction(const Function *i) = 0;
  virtual void visitStatement(const Statement *i) = 0;
  virtual void visitScope(const Scope *i) = 0;
  virtual void visitIf(const If *i) = 0;
  virtual void visitWhile(const While *i) = 0;
  virtual void visitExpr(const Expr *i) = 0;
  virtual void visitDeclare(const Declare *i) = 0;
  virtual void visitAssign(const Assign *i) = 0;
  virtual void visitCall(const Call *i) = 0;
  virtual void visitReturn(const Return *i) = 0;
  virtual void visitRead(const Read *i) = 0;
  virtual void visitAssert(const Assert *i) = 0;
  virtual void visitUnaryOp(const UnaryOp *i) = 0;
  virtual void visitBinaryOp(const BinaryOp *i) = 0;
  virtual void visitVariable(const Variable *i) = 0;
  virtual void visitLiteral(const Literal *i) = 0;
};

class IRNode {
public:
  mutable std::string type = "void";
  mutable std::string name;
  mutable std::list<std::string> errors;
  mutable std::list<std::string> errors_in;
  mutable std::list<std::string> errors_out;
  mutable std::map<std::string, std::string> symtab; // name,type
  virtual void accept(IRVisitor *v) = 0;
  void appendError(const std::string s) const { errors.emplace_back(s); }
  void appendError_in(const std::string s) const { errors_in.emplace_back(s); }
  void appendError_out(const std::string s) const {
    errors_out.emplace_back(s);
  }
};

class Program : public IRNode {
public:
  std::list<Function *> functions;
  Scope *scope;
  void accept(IRVisitor *v) override { v->visitProgram(this); }
};

class Function : public IRNode {
public:
  Scope *scope;
  void accept(IRVisitor *v) override { v->visitFunction(this); }
};

class Statement : public IRNode {
public:
  void accept(IRVisitor *v) override { v->visitStatement(this); }
};

class Scope : public IRNode {
public:
  std::list<Statement *> statements;
  void accept(IRVisitor *v) override { v->visitScope(this); }
};

class If : public Statement {
public:
  Scope *scope1;
  Scope *scope2;
  void accept(IRVisitor *v) override { v->visitIf(this); }
};

class While : public Statement {
public:
  Scope *scope;
  void accept(IRVisitor *v) override { v->visitWhile(this); }
};

class Declare : public Statement {
public:
  std::list<std::string> names;
  void accept(IRVisitor *v) override { v->visitDeclare(this); }
};

class Assign : public Statement {
public:
  Expr *expr;
  void accept(IRVisitor *v) override { v->visitAssign(this); }
};

class Call : public Statement {
public:
  std::list<Expr *> args;
  void accept(IRVisitor *v) override { v->visitCall(this); }
};

class Return : public Statement {
public:
  Expr *expr;
  void accept(IRVisitor *v) override { v->visitReturn(this); }
};

class Read : public Statement {
public:
  std::list<std::string> names;
  void accept(IRVisitor *v) override { v->visitRead(this); }
};

class Assert : public Statement {
public:
  Expr *expr;
  void accept(IRVisitor *v) override { v->visitAssert(this); }
};

class Expr : public IRNode {
public:
  void accept(IRVisitor *v) override { v->visitExpr(this); }
};

class Literal : public Expr {
public:
  std::string value;
  void accept(IRVisitor *v) override { v->visitLiteral(this); }
};

class BinaryOp : public Expr {
public:
  std::string op;
  Expr *left;
  Expr *right;
  void accept(IRVisitor *v) override { v->visitBinaryOp(this); }
};

class UnaryOp : public Expr {
public:
  std::string op;
  Expr *left;
  void accept(IRVisitor *v) override { v->visitUnaryOp(this); }
};

class Variable : public Expr {
public:
  Expr *index;
  void accept(IRVisitor *v) override { v->visitVariable(this); }
};

void runScanner(const std::string source);
void runParser(const std::string source);
void compile(const std::string source);

} // namespace Compiler

#endif // COMPILER_H_
