#include <sstream>

#include "type_checker.hh"

#include "../utils/errors.hh"
#include "../utils/nolocation.hh"

using utils::error;
using utils::non_fatal_error;



namespace ast {
namespace type_checker {



TypeChecker::TypeChecker()  {}
Type string_to_type(const std::string& str){
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
    
    op.set_type(op.get_left().get_type());

    if(op.get_left().get_type() != op.get_right().get_type())
        utils::error("conflicting types in BinaryOperator");
}

void TypeChecker::visit(Sequence &seq) {
    const auto exprs = seq.get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);

    Type type = exprs.back()->get_type();
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
    let.set_type( type == t_undef ? t_void : type);
}

void TypeChecker::visit(Identifier &id) {
    id.set_type(id.get_decl()->get_type());
}

void TypeChecker::visit(IfThenElse &ite) {
    ite.get_condition().accept(*this);
    ite.get_then_part().accept(*this);
    ite.get_else_part().accept(*this);

    if(ite.get_then_part().get_type() != ite.get_else_part().get_type())
        utils::error("conflicting types in IfThenElse");
    ite.set_type(ite.get_then_part().get_type() );
}

void TypeChecker::visit(VarDecl &decl) {

    if( auto type_name = decl.type_name)
        decl.set_type(string_to_type(type_name->get()));

    if (auto expr = decl.get_expr()){
        expr->accept(*this);
        Type type_expr = expr->get_type();
        if(decl.get_type() == t_undef)
            decl.set_type(type_expr);

        if(decl.get_type() != type_expr)
            utils::error("conflicting types in var declaration");
    }
}

void TypeChecker::visit(FunDecl &decl) {

    if(decl.get_type() != t_undef){
        std::cout << decl.name.get() << " est déja typé ! on return !  (type : "<< decl.get_type() <<std::endl;
        return ;
        }

    if(decl.is_visited){
        std::cout << decl.name.get() << " deja visité, return !"<< std::endl;
        return ;
        }
    std::cout << " On entre dans  : "<<  decl.name.get() <<std::endl;
    decl.is_visited = true;


    auto params = decl.get_params();
    for (auto param : decl.get_params())
        param->accept(*this);


    if( decl.is_external && decl.name.get() != "main"){ //fonction primaire
        decl.set_type( decl.type_name ? string_to_type(decl.type_name.get()) : t_void);
        return;
    }

    boost::optional<ast::types::Symbol>  symbol_decl = decl.type_name;
    if( symbol_decl ){
        decl.set_type(string_to_type(symbol_decl.get()));
    }

    auto expr = decl.get_expr();
    expr->accept(*this);

    if( symbol_decl ){
        if( string_to_type(symbol_decl->get()) != expr->get_type() )
            utils::error("conflicting types in FunDecl");
    }
    else
        decl.set_type(decl.get_expr()->get_type());


}

void TypeChecker::visit(FunCall &call) {

    if(auto decl = call.get_decl()){
        decl->accept(*this);

        if (decl->get_type() != t_undef  )
            call.set_type(decl->get_type());

        auto args = call.get_args();
        auto params = decl->get_params();

        if(args.size() > params.size())
            utils::error("to much arguments in FunCall");
        else if (args.size() < params.size())
            utils::error("to few arguments in FunCall");
        for (size_t id = 0; id != args.size(); id++){
            args[id]->accept(*this);
            if(args[id]->get_type() != params[id]->get_type() )
                utils::error("conflicting types in FunCall");
        }

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
