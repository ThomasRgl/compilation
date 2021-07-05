#include <sstream>

#include "type_checker.hh"

#include "../utils/errors.hh"
#include "../utils/nolocation.hh"

using utils::error;
using utils::non_fatal_error;



namespace ast {
namespace type_checker {

/* Returns the current scope */
// scope_t &Binder::current_scope() { return scopes.back(); }

/* Pushes a new scope on the stack */
// void Binder::push_scope() { scopes.push_back(scope_t()); }

/* Pops the current scope from the stack */
// void Binder::pop_scope() { scopes.pop_back(); }

/* Enter a declaration in the current scope. Raises an error if the declared name
 * is already defined */
// void Binder::enter(Decl &decl) {
//   scope_t &scope = current_scope();
//   auto previous = scope.find(decl.name);
//   if (previous != scope.end()) {
//     non_fatal_error(decl.loc,
//                     decl.name.get() + " is already defined in this scope");
//     error(previous->second->loc, "previous declaration was here");
//   }
//   scope[decl.name] = &decl;
// }

/* Finds the declaration for a given name. The scope stack is traversed
 * front to back starting from the current scope. The first matching
 * declaration is returned. Raises an error, if no declaration matches. */
// Decl &Binder::find(const location loc, const Symbol &name) {
//   for (auto scope = scopes.crbegin(); scope != scopes.crend(); scope++) {
//     auto decl_entry = scope->find(name);
//     if (decl_entry != scope->cend()) {
//       return *decl_entry->second;
//     }
//   }
//   error(loc, name.get() + " cannot be found in this scope");
// }

TypeChecker::TypeChecker()  {
  /* Create the top-level scope */
  // push_scope();

  /* Populate the top-level scope with all the primitive declarations */
  // const Symbol s_int = Symbol("int");
  // const Symbol s_string = Symbol("string");
  // enter_primitive("print_err", boost::none, {s_string});
  // enter_primitive("print", boost::none, {s_string});
  // enter_primitive("print_int", boost::none, {s_int});
  // enter_primitive("flush", boost::none, {});
  // enter_primitive("getchar", s_string, {});
  // enter_primitive("ord", s_int, {s_string});
  // enter_primitive("chr", s_string, {s_int});
  // enter_primitive("size", s_int, {s_string});
  // enter_primitive("substring", s_string, {s_string, s_int, s_int});
  // enter_primitive("concat", s_string, {s_string, s_string});
  // enter_primitive("strcmp", s_int, {s_string, s_string});
  // enter_primitive("streq", s_int, {s_string, s_string});
  // enter_primitive("not", s_int, {s_int});
  // enter_primitive("exit", boost::none, {s_int});
}

/* Declares a new primitive into the current scope*/
// void Binder::enter_primitive(
//     const std::string &name, const boost::optional<Symbol> &type_name,
//     const std::vector<Symbol> &argument_typenames) {
//   std::vector<VarDecl *> args;
//   int counter = 0;
//   for (const Symbol &tn : argument_typenames) {
//     std::ostringstream argname;
//     argname << "a_" << counter++;
//     args.push_back(
//         new VarDecl(utils::nl, Symbol(argname.str()), tn, nullptr));
//   }
//
//   boost::optional<Symbol> type_name_symbol = boost::none;
//   FunDecl *fd = new FunDecl(utils::nl, Symbol(name), type_name, std::move(args), nullptr,
//                             true);
//   fd->set_external_name(Symbol("__" + name));
//   enter(*fd);
// }

/* Sets the parent of a function declaration and computes and sets
 * its unique external name */
// void Binder::set_parent_and_external_name(FunDecl &decl) {
//   auto parent = functions.empty() ? nullptr : functions.back();
//   Symbol external_name;
//   if (parent) {
//     decl.set_parent(parent);
//     external_name = parent->get_external_name().get() + '.' + decl.name.get();
// } else{
//     external_name = decl.name;
//     }
//   while (external_names.find(external_name) != external_names.end())
//     external_name = Symbol(external_name.get() + '_');
//   external_names.insert(external_name);
//   decl.set_external_name(external_name);
// }

/* Binds a whole program. This method wraps the program inside a top-level main
 * function.  Then, it visits the programs with the Binder visitor; binding
 * each identifier to its declaration and computing depths.*/
// FunDecl *Binder::analyze_program(Expr &root) {
//   std::vector<VarDecl *> main_params;
//   Sequence *const main_body = new Sequence(
//       utils::nl,
//       std::vector<Expr *>({&root, new IntegerLiteral(utils::nl, 0)}));
//   FunDecl *const main = new FunDecl(utils::nl, Symbol("main"), Symbol("int"),
//                                     main_params, main_body, true);
//   main->accept(*this);
//   return main;
// }
Type string_to_type(const std::string& str)
{
    // switch ( str)
    // {
    //     case "string" :
    //         return t_string;
    //     case "int" :
    //         return t_int;
    // }
    static std::unordered_map<std::string, Type> map = {{"string", t_string}, {"int", t_int}};

    return map[str];
}

void TypeChecker::visit(IntegerLiteral &literal) {
    literal.set_type( t_int) ;
}

void TypeChecker::visit(StringLiteral &literal) {
    literal.set_type( t_string );
}

void TypeChecker::visit(BinaryOperator &op) {
    op.get_left().accept(*this);
    op.get_right().accept(*this);
    std::cout << "left" << op.get_left().get_type() <<"right" << op.get_right().get_type()  << std::endl;
    if(op.get_left().get_type() != op.get_right().get_type())
        utils::error("conflicting types in BinaryOperator");
}

void TypeChecker::visit(Sequence &seq) {
    const auto exprs = seq.get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);
    // std::cout << "ee"<<exprs.back()->get_type()<< std::endl;
    Type type = exprs.back()->get_type();
    // if(type == t_undef)
    //     type = t_void;
    seq.set_type( type == t_undef ? t_void : type);

}


void TypeChecker::visit(Let &let) {


    std::vector<Decl *> decls;
    for (auto decl : let.get_decls())
        decl->accept(*this);


    const auto exprs = let.get_sequence().get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);

    Type type = exprs.back()->get_type();
    // if(type == t_undef)
    //     type = t_void;
    // let.set_type(type);
    let.set_type( type == t_undef ? t_void : type);
}

void TypeChecker::visit(Identifier &id) {
    id.set_type(id.get_decl()->get_type());
}

void TypeChecker::visit(IfThenElse &ite) {
    std::cout << "if then else tamere"<< std::endl;
    ite.get_condition().accept(*this);
    ite.get_then_part().accept(*this);
    ite.get_else_part().accept(*this);
    if(ite.get_then_part().get_type() != ite.get_else_part().get_type())
        utils::error("conflicting types in IfThenElse");
    ite.set_type(ite.get_then_part().get_type() );
    std::cout << "if then else type : "<< ite.get_type() <<std::endl;
}

void TypeChecker::visit(VarDecl &decl) {


    if( auto type_name = decl.type_name){
        decl.set_type(string_to_type(type_name->get()));
        // if(type_name->get() == "string")
        //     decl.set_type(t_string);
        // else if(type_name->get() == "int")
        //     decl.set_type(t_int);
    }//type_name == "string" ? t_string : t_int

    if (auto expr = decl.get_expr()){
        expr->accept(*this);
        Type type_expr = expr->get_type();
        if(decl.get_type() != type_expr)
            utils::error("conflicting types in var ceclaration");
    }


}

void TypeChecker::visit(FunDecl &decl) {

    std::cout <<"avFundecl"<< std::endl;
    if(decl.get_type() == t_undef){
        std::cout <<"Fundecl"<< std::endl;
        std::cout << "fundecl type"<< decl.get_type()<< std::endl;
        auto params = decl.get_params();
        for (auto param = params.cbegin(); param != params.cend(); param++)
            (*param)->accept(*this);

        decl.get_expr()->accept(*this);

        if(auto symbol_decl = decl.type_name ){
            if( !((symbol_decl->get() == "string" && decl.get_expr()->get_type() == t_string) || (symbol_decl->get() == "int" && decl.get_expr()->get_type() == t_int)))
                utils::error("conflicting types in FunDecl");
            // std::cout << decl.type_name->get() << std::endl;
            // std::cout << decl.get_expr()->get_type()<< std::endl;
            //
        }
        decl.set_type(decl.get_expr()->get_type());
        std::cout << "fundecl type"<< decl.get_type()<< std::endl;

    }

}

void TypeChecker::visit(FunCall &call) {

    std::cout << "funcall" << std::endl;

    if(auto decl = call.get_decl()){
        if (decl->get_type() == t_undef){
            decl->accept(*this);
        }


        call.set_type(decl->get_type());

        auto args = call.get_args();
        for (auto arg = args.cbegin(); arg != args.cend(); arg++)
            (*arg)->accept(*this);

    }
}

void TypeChecker::visit(WhileLoop &loop) {
    loop.get_condition().accept(*this);
    loop.get_body().accept(*this);

    if(loop.get_condition().get_type() != t_int )
        utils::error("while condition must be int");
    if(loop.get_body().get_type() != t_void )
        utils::error("while body  must be void");
    loop.set_type(t_void);
}

void TypeChecker::visit(ForLoop &loop) {

    loop.get_variable().accept(*this);

    loop.get_high().accept(*this);

    loop.get_body().accept(*this);

    if(loop.get_variable().get_type() != t_int )
        utils::error("for variable must be int");
    if(loop.get_high().get_type() != t_int )
        utils::error("for bound must be int");
    if(loop.get_body().get_type() != t_void )
        utils::error("for body must be void");

    loop.set_type(t_void);
}

void TypeChecker::visit(Break &b) {
    b.set_type(t_void);
}

void TypeChecker::visit(Assign &assign) {
    assign.get_lhs().accept(*this);
    assign.get_rhs().accept(*this);

    if( assign.get_lhs().get_type() != assign.get_rhs().get_type())
        utils::error("conflicting types in assign");
    assign.set_type(t_void);
}

} // namespace type_checker
} // namespace ast
