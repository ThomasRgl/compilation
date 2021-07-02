#ifndef AST_EVAL_HH
#define AST_EVAL_HH

#include <ostream>

#include "nodes.hh"

namespace ast {

class ASTEval : public ConstASTIntVisitor {
  std::ostream *ostream;
  bool verbose;
  unsigned indent_level = 0;
  void inc() { indent_level++; }
  void inl() {
    inc();
    nl();
  };
  void dec() { indent_level--; }
  void dnl() {
    dec();
    nl();
  };

public:
  ASTEval(std::ostream *_ostream, bool _verbose)
      : ostream(_ostream), verbose(_verbose) {}
  void nl() {
    *ostream << std::endl;
    for (unsigned i = 0; i < indent_level; i++)
      *ostream << "  ";
  };
  virtual int32_t visit(const IntegerLiteral &);
  virtual int32_t visit(const StringLiteral &);
  virtual int32_t visit(const BinaryOperator &);
  virtual int32_t visit(const Sequence &);
  virtual int32_t visit(const Let &);
  virtual int32_t visit(const Identifier &);
  virtual int32_t visit(const IfThenElse &);
  virtual int32_t visit(const VarDecl &);
  virtual int32_t visit(const FunDecl &);
  virtual int32_t visit(const FunCall &);
  virtual int32_t visit(const WhileLoop &);
  virtual int32_t visit(const ForLoop &);
  virtual int32_t visit(const Break &);
  virtual int32_t visit(const Assign &);
  virtual ~ASTEval(){}
};

} // namespace ast

#endif // _AST_EVAL_HH
