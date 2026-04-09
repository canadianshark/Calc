#include "AST.h"

AST::AST(std::unique_ptr<AST_node> root_): root(std::move(root_)){};

AST AST::clone_tree() {
    auto root_cloned = root->clone();
    return AST{std::move(root_cloned)};
}


Number_node::Number_node(std::string str): value(str){};

std::string Number_node::get_value() {
    return value;
}

std::unique_ptr<AST_node> Number_node::clone() {
    std::unique_ptr<AST_node> res = std::make_unique<Number_node>(value);
    return res;
}
std::string Number_node::dump_data() {
    return get_value();
}

void Number_node::accept(Visitor &v) {
    v.visit(*this);
}


Variable_node::Variable_node(std::string str): name(str) {};

std::string Variable_node::get_name() {
    return name;
}

std::unique_ptr<AST_node> Variable_node::clone() {
    std::unique_ptr<AST_node> res = std::make_unique<Variable_node>(name);
    return res;
}

std::string Variable_node::dump_data() {
    return get_name();
}

void Variable_node::accept(Visitor &v) {
    v.visit(*this);
}


Binop_node::Binop_node(std::string str): operation(str) {};

std::string Binop_node::get_operation() {
    return operation;
}

std::unique_ptr<AST_node> Binop_node::clone() {
    Binop_node res = Binop_node(operation);
    res.first_op = first_op->clone();
    res.second_op = second_op->clone();
    return std::make_unique<Binop_node>(std::move(res));
}

std::string Binop_node::dump_data() {
    return get_operation();
}

void Binop_node::accept(Visitor &v) {
    v.visit(*this);
}

Unop_node::Unop_node(std::string str): operation(str) {};

std::string Unop_node::get_operation() {
    return operation;
}

std::unique_ptr<AST_node> Unop_node::clone() {
    Unop_node res = Unop_node(operation);
    res.op = op->clone();
    return std::make_unique<Unop_node>(std::move(res));
}

std::string Unop_node::dump_data() {
    return get_operation();
}

void Unop_node::accept(Visitor &v) {
    v.visit(*this);
}

Func_node::Func_node(std::string str): name(str) {};

std::string Func_node::get_name() {
    return name;
}

std::unique_ptr<AST_node> Func_node::clone() {
    auto res = std::make_unique<Func_node>(name);
    if (arg) {
        res->arg = arg->clone();
    }
    return res;
}

std::string Func_node::dump_data() {
    return get_name();
}

void Func_node::accept(Visitor &v) {
    v.visit(*this);
}

void AST::accept(Visitor& v) {
    if (root) {
        root->accept(v);
    }
}