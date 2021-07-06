#include <sstream>

#include "escaper.hh"

#include "../utils/errors.hh"
#include "../utils/nolocation.hh"

using utils::error;
using utils::non_fatal_error;



namespace ast {
namespace escaper {



Escaper::Escaper()  {}

void Escaper::visit(IntegerLiteral &literal) {
}

void Escaper::visit(StringLiteral &literal) {
}

void Escaper::visit(BinaryOperator &op) {
    op.get_left().accept(*this);
    op.get_right().accept(*this);

}

void Escaper::visit(Sequence &seq) {

    const auto exprs = seq.get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);

}

void Escaper::visit(Let &let) {

    std::vector<Decl *> decls;
    for (auto decl : let.get_decls())
        decl->accept(*this);

    const auto exprs = let.get_sequence().get_exprs();
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++)
        (*expr)->accept(*this);

}

void Escaper::visit(Identifier &id) {
}

void Escaper::visit(IfThenElse &ite) {
    ite.get_condition().accept(*this);
    ite.get_then_part().accept(*this);
    ite.get_else_part().accept(*this);

}

void Escaper::visit(VarDecl &decl) {
    if(decl.get_escapes()){
        std::cout << "Var decl, la variable "<< decl.name.get() << " echappe" << std::endl;
        current_funDecl->escaping_decls.push_back(&decl);
    }
    if (auto expr = decl.get_expr())
        expr->accept(*this);

}

void Escaper::visit(FunDecl &decl) {


    auto params = decl.get_params();
    for (auto param : decl.get_params())
        param->accept(*this);

    FunDecl* _current_funDecl_ = current_funDecl;
    current_funDecl = &decl;
    if(_current_funDecl_)
        std::cout << "rencontre de funDecl " << " current : "<< _current_funDecl_->name.get() << " On entre dans " << current_funDecl->name.get()  << std::endl;
    auto expr = decl.get_expr();
    expr->accept(*this);

    if(_current_funDecl_)
        std::cout << "fin de funDecl " << " current : "<< _current_funDecl_->name.get() << " On sort de  " << current_funDecl->name.get()  << std::endl;

    current_funDecl = _current_funDecl_;
    }

void Escaper::visit(FunCall &call) {

    auto args = call.get_args();
    for (auto arg = args.cbegin(); arg != args.cend(); arg++)
      (*arg)->accept(*this);
}

void Escaper::visit(WhileLoop &loop) {
    loop.get_condition().accept(*this);
    loop.get_body().accept(*this);

}

void Escaper::visit(ForLoop &loop) {
    loop.get_variable().accept(*this);
    loop.get_high().accept(*this);
    loop.get_body().accept(*this);
}

void Escaper::visit(Break &b) {
}

void Escaper::visit(Assign &assign) {
    assign.get_lhs().accept(*this);
    assign.get_rhs().accept(*this);

}

} // namespace type_checker
} // namespace ast
