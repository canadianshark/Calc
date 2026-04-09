#include "Visitor.h"
#include "AST.h"

void Polish_visitor::visit(class Number_node &node){
    result += node.dump_data();
}

void Polish_visitor::visit(class Variable_node &node){
    result += node.dump_data();
}

void Polish_visitor::visit(class Binop_node &node){
    result += node.dump_data() + "(";
    node.first_op->accept(*this);
    result += ",";
    node.second_op->accept(*this);
    result += ")";
}

void Polish_visitor::visit(class Unop_node &node){
    result += node.dump_data() + "(";
    node.op->accept(*this);
    result += ")";
}

void Polish_visitor::visit(class Func_node &node){
    result += node.dump_data() + "(";
    node.arg->accept(*this);
    result += ")";
}

void Printing_visitor::visit(class Number_node &node) {
    result += node.dump_data();
}

void Printing_visitor::visit(class Variable_node &node){
    result += node.dump_data();
}

void Printing_visitor::visit(class Binop_node &node) {
    result += "(";
    node.first_op->accept(*this);
    result += node.dump_data();
    node.second_op->accept(*this);
    result += ")";
}

void Printing_visitor::visit(class Unop_node &node) {
    result += node.dump_data();
    result += "(";
    node.op->accept(*this);
    result += ")";
}

void Printing_visitor::visit(class Func_node &node) {
    result += node.dump_data();
    result += "(";
    node.arg->accept(*this);
    result += ")";
}
Calculating_visitor::Calculating_visitor(Calculating_context &ctx): context(ctx) {};

void Calculating_visitor::visit(class Number_node &node) {
    stack.push_back(std::stod(node.get_value()));
}

void Calculating_visitor::visit(class Variable_node &node) {
   stack.push_back(context.variables[node.get_name()]);
}

void Calculating_visitor::visit(class Binop_node &node) {
    node.first_op->accept(*this);
    node.second_op->accept(*this);

    double right = stack.back(); stack.pop_back();
    double left = stack.back(); stack.pop_back();

    double res = context.operation_rules[node.get_operation()](left, right);
    stack.push_back(res);
}

void Calculating_visitor::visit(class Func_node &node) {
    node.arg->accept(*this);

    double arg = stack.back(); stack.pop_back();
    double res = context.functions[node.get_name()](arg);
    stack.push_back(res);
}

void Calculating_visitor::visit(class Unop_node &node) {
    node.op->accept(*this);
    double arg = stack.back(); stack.pop_back();

    if(node.get_operation() == "+"){
        double res = +arg;
        stack.push_back(res);
    }
    if(node.get_operation() == "-"){
        double res = -arg;
        stack.push_back(res);
    }
}

double Calculating_visitor::get_result() {
    return stack.back();
}

void Derivative_visitor::visit(Number_node& n) { result = std::make_unique<Number_node>("0"); }

void Derivative_visitor::visit(Variable_node& n) {
    result = std::make_unique<Number_node>(n.get_name() == var ? "1" : "0");
}
void Derivative_visitor::visit(Binop_node& n) {
    if (n.get_operation() == "+" || n.get_operation() == "-") {
        auto node = std::make_unique<Binop_node>(n.get_operation());
        n.first_op->accept(*this);
        node->first_op = std::move(result);
        n.second_op->accept(*this);
        node->second_op = std::move(result);
        result = std::move(node);
    }
    if (n.get_operation() == "*") {
        // (u*v)' = u'v + uv'
        auto plus = std::make_unique<Binop_node>("+");
        auto left = std::make_unique<Binop_node>("*");
        auto right = std::make_unique<Binop_node>("*");

        n.first_op->accept(*this);
        left->first_op = std::move(result);
        left->second_op = n.second_op->clone();

        n.second_op->accept(*this);
        right->second_op = std::move(result);
        right->first_op = n.first_op->clone();

        plus->first_op = std::move(left);
        plus->second_op = std::move(right);
        result = std::move(plus);
    }
}

void Derivative_visitor::visit(class Func_node &node) {

}

void Derivative_visitor::visit(class Unop_node &node) {}