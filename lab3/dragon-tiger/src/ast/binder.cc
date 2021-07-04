#include <sstream>

#include "binder.hh"
#include "../utils/errors.hh"
#include "../utils/nolocation.hh"

using utils::error;
using utils::non_fatal_error;

namespace ast {
namespace binder {

/* Returns the current scope */
scope_t &Binder::current_scope() { return scopes.back(); }

/* Pushes a new scope on the stack */
void Binder::push_scope() { scopes.push_back(scope_t()); }

/* Pops the current scope from the stack */
void Binder::pop_scope() { scopes.pop_back(); }

/* Enter a declaration in the current scope. Raises an error if the declared name
 * is already defined */
void Binder::enter(Decl &decl) {
  scope_t &scope = current_scope();
  auto previous = scope.find(decl.name);
  if (previous != scope.end()) {
    non_fatal_error(decl.loc,
                    decl.name.get() + " is already defined in this scope");
    error(previous->second->loc, "previous declaration was here");
  }
  scope[decl.name] = &decl;
}

/* Finds the declaration for a given name. The scope stack is traversed
 * front to back starting from the current scope. The first matching
 * declaration is returned. Raises an error, if no declaration matches. */
Decl &Binder::find(const location loc, const Symbol &name) {
  for (auto scope = scopes.crbegin(); scope != scopes.crend(); scope++) {
    auto decl_entry = scope->find(name);
    if (decl_entry != scope->cend()) {
      return *decl_entry->second;
    }
  }
  error(loc, name.get() + " cannot be found in this scope");
}

Binder::Binder() : scopes() {
  /* Create the top-level scope */
  push_scope();

  /* Populate the top-level scope with all the primitive declarations */
  const Symbol s_int = Symbol("int");
  const Symbol s_string = Symbol("string");
  enter_primitive("print_err", boost::none, {s_string});
  enter_primitive("print", boost::none, {s_string});
  enter_primitive("print_int", boost::none, {s_int});
  enter_primitive("flush", boost::none, {});
  enter_primitive("getchar", s_string, {});
  enter_primitive("ord", s_int, {s_string});
  enter_primitive("chr", s_string, {s_int});
  enter_primitive("size", s_int, {s_string});
  enter_primitive("substring", s_string, {s_string, s_int, s_int});
  enter_primitive("concat", s_string, {s_string, s_string});
  enter_primitive("strcmp", s_int, {s_string, s_string});
  enter_primitive("streq", s_int, {s_string, s_string});
  enter_primitive("not", s_int, {s_int});
  enter_primitive("exit", boost::none, {s_int});
}

/* Declares a new primitive into the current scope*/
void Binder::enter_primitive(
    const std::string &name, const boost::optional<Symbol> &type_name,
    const std::vector<Symbol> &argument_typenames) {
  std::vector<VarDecl *> args;
  int counter = 0;
  for (const Symbol &tn : argument_typenames) {
    std::ostringstream argname;
    argname << "a_" << counter++;
    args.push_back(
        new VarDecl(utils::nl, Symbol(argname.str()), tn, nullptr));
  }

  boost::optional<Symbol> type_name_symbol = boost::none;
  FunDecl *fd = new FunDecl(utils::nl, Symbol(name), type_name, std::move(args), nullptr,
                            true);
  fd->set_external_name(Symbol("__" + name));
  enter(*fd);
}

/* Sets the parent of a function declaration and computes and sets
 * its unique external name */
void Binder::set_parent_and_external_name(FunDecl &decl) {
  auto parent = functions.empty() ? nullptr : functions.back();
  Symbol external_name;
  if (parent) {
    decl.set_parent(parent);
    external_name = parent->get_external_name().get() + '.' + decl.name.get();
    std::cout << "external name 1: "<< external_name << std::endl ;
} else{
    external_name = decl.name;
    std::cout << "external name 2: "<< external_name << std::endl ;
    }
  while (external_names.find(external_name) != external_names.end())
    external_name = Symbol(external_name.get() + '_');
  external_names.insert(external_name);
  decl.set_external_name(external_name);
}

/* Binds a whole program. This method wraps the program inside a top-level main
 * function.  Then, it visits the programs with the Binder visitor; binding
 * each identifier to its declaration and computing depths.*/
FunDecl *Binder::analyze_program(Expr &root) {
  std::vector<VarDecl *> main_params;
  Sequence *const main_body = new Sequence(
      utils::nl,
      std::vector<Expr *>({&root, new IntegerLiteral(utils::nl, 0)}));
  FunDecl *const main = new FunDecl(utils::nl, Symbol("main"), Symbol("int"),
                                    main_params, main_body, true);
  main->accept(*this);
  std::cout << "return main" << std::endl ;
  return main;
}

void Binder::visit(IntegerLiteral &literal) {
    std::cout << "IntegerLiteral" << std::endl ;
}

void Binder::visit(StringLiteral &literal) {
    std::cout << "StringLiteral" << std::endl ;
}

void Binder::visit(BinaryOperator &op) {
    std::cout << "BinaryOperator_1" << std::endl ;
    op.get_left().accept(*this);

    std::cout << "BinaryOperator_2" << std::endl ;
    op.get_right().accept(*this);
}

void Binder::visit(Sequence &seq) {
    std::cout << "Sequence" << std::endl ;
    const auto exprs = seq.get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++) {
        (*expr)->accept(*this);
    }
}


void Binder::visit(Let &let) {
    push_scope();
    std::cout << "Let" << std::endl ;

    std::vector<Decl *> decls;
    for (auto decl : let.get_decls()){

        if(dynamic_cast<FunDecl*> (decl) ){
            enter(*decl);
            std::cout << "hello" << std::endl ;
            decls.push_back(decl);
        }
        else{
            for (auto funDecl : decls)
                funDecl->accept(*this);
            decls.clear();
            decl->accept(*this);
        }
    }

    for (auto funDecl : decls){ //if the decls vector is not empty
        funDecl->accept(*this);
    }

    std::cout << "IN" << std::endl ;
    const auto exprs = let.get_sequence().get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);
    std::cout << "END" << std::endl ;
    pop_scope();
}

void Binder::visit(Identifier &id) {
    std::cout << "Identifier" << std::endl ;
    VarDecl& decl = dynamic_cast<VarDecl&> ( find(id.loc, id.name));

    id.set_decl(&decl);

}

void Binder::visit(IfThenElse &ite) {
    std::cout << "IF" << std::endl ;
    ite.get_condition().accept(*this);
    std::cout << "THEN" << std::endl ;
    ite.get_then_part().accept(*this);
    std::cout << "ELSE" << std::endl ;
    ite.get_else_part().accept(*this);
}

void Binder::visit(VarDecl &decl) {
    std::cout << "VarDecl" << std::endl ;
    enter(decl);
    if (auto expr = decl.get_expr())
        expr->accept(*this);

}

void Binder::visit(FunDecl &decl) {
    std::cout << "FunDecl" << std::endl ;

    //enter(decl);

    push_scope();
    set_parent_and_external_name(decl);

    functions.push_back(&decl);

    auto params = decl.get_params();
    for (auto param = params.cbegin(); param != params.cend(); param++)
      (*param)->accept(*this);

    decl.get_expr()->accept(*this);

    functions.pop_back();
    pop_scope();

}

void Binder::visit(FunCall &call) {
    std::cout << "FunCall" << std::endl ;

    FunDecl& decl = dynamic_cast<FunDecl&> ( find(call.loc, call.func_name));
    call.set_decl(&decl);

    auto args = call.get_args();
    for (auto arg = args.cbegin(); arg != args.cend(); arg++) {
        (*arg)->accept(*this);
    }
}

void Binder::visit(WhileLoop &loop) {
    std::cout << "WhileLoop" << std::endl ;
    loop.get_condition().accept(*this);
    std::cout << "DO" << std::endl ;
    loop.get_body().accept(*this);
}

void Binder::visit(ForLoop &loop) {
    std::cout << "ForLoop" << std::endl ;
    // if (verbose && loop.get_variable().get_escapes())
    //   *ostream << "/*e*/";
    // *ostream << " := ";
    // loop.get_variable().name
    loop.get_variable().accept(*this);

    std::cout << "TO" << std::endl ;
    loop.get_high().accept(*this);

    std::cout << "DO" << std::endl ;
    loop.get_body().accept(*this);
}

void Binder::visit(Break &b) {
    std::cout << "Break" << std::endl ;
}

void Binder::visit(Assign &assign) {
    std::cout << "Assign left" << std::endl ;
    assign.get_lhs().accept(*this);
    std::cout << "Assign right" << std::endl ;
    assign.get_rhs().accept(*this);
}

} // namespace binder
} // namespace ast
