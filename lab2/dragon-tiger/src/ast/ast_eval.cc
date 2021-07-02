#include "ast_eval.hh"
#include "../utils/errors.hh"




namespace ast {

int32_t ASTEval::visit(const IntegerLiteral &literal) {
    return literal.value;
}

int32_t ASTEval::visit(const StringLiteral &literal) {
    utils::error("StringLiterals are not supported");
    return -1;
}

int32_t ASTEval::visit(const BinaryOperator &binop) {
    auto binop_left = binop.get_left().accept(*this);
    auto binop_right = binop.get_right().accept(*this);
    
    switch (binop.op){
        case o_plus:
            return binop_left + binop_right;
        case o_minus:
            return binop_left - binop_right;
        case o_times:
            return binop_left * binop_right;
        case o_divide:
            if( binop_right == 0)
                utils::error("cannot divide by zero");
            return binop_left / binop_right;
        case o_eq:
            if(binop_left == binop_right )
                return 1;
            else
                return 0;
        case o_neq:
            if(binop_left != binop_right )
                return 1;
            else
                return 0;
        case o_lt:
            if(binop_left < binop_right )
                return 1;
            else
                return 0;
        case o_le:
            if(binop_left <= binop_right )
                return 1;
            else
                return 0;
        case o_gt:
            if(binop_left > binop_right )
                return 1;
            else
                return 0;
        case o_ge:
            if(binop_left >= binop_right )
                return 1;
            else
                return 0;
        default :
            utils::error("operator not supported");
    }

}

int32_t ASTEval::visit(const Sequence &seqExpr) {

    int32_t _int32_ = 0;
    const auto exprs = seqExpr.get_exprs();
    if( exprs.cbegin() == exprs.cend())
        utils::error("empty expressions are not supported");
    for (auto expr = exprs.cbegin(); expr != exprs.cend(); expr++) {
        _int32_ = (*expr)->accept(*this);
    }

    return _int32_ ;
}

int32_t ASTEval::visit(const Let &let) {
    utils::error("'Let' are not supported");
    return -1;
}

int32_t ASTEval::visit(const Identifier &id) {
    utils::error("Identifiers are not supported");
    return -1;
}

int32_t ASTEval::visit(const IfThenElse &ite) {
    if( ite.get_condition().accept(*this) == 1)
        return ite.get_then_part().accept(*this);
    else
        return ite.get_else_part().accept(*this);
}

int32_t ASTEval::visit(const VarDecl &decl) {
    utils::error("VarDecls are not supported");
    return -1;
}

int32_t ASTEval::visit(const FunDecl &decl) {
    utils::error("FunDecl are not supported");
    return -1;
}

int32_t ASTEval::visit(const FunCall &call) {
    utils::error("FunCall are not supported");
    return -1;

}

int32_t ASTEval::visit(const WhileLoop &loop) {
    utils::error("WhileLoops are not supported");
    return -1;
}

int32_t ASTEval::visit(const ForLoop &loop) {
    utils::error("ForLoops are not supported");
    return -1;
}

int32_t ASTEval::visit(const Break &brk) {
    utils::error("Breaks are not supported");
    return -1;
}

int32_t ASTEval::visit(const Assign &assign) {
    utils::error("Assign are not supported");
    return -1;
}

} // namespace ast
