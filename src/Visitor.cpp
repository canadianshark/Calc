#include "Visitor.h"
#include "AST.h"
#include "Context.h"

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
    result = std::make_unique<Number_node>(n.get_name() == context.var ? "1" : "0");
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
    if(n.get_operation() == "^"){
        n.first_op->accept(*this);
        auto du = std::move(result);
        n.second_op->accept(*this);
        auto dv = std::move(result);

        auto v_mult_du = std::make_unique<Binop_node>("*");
        v_mult_du->first_op = n.second_op->clone();
        v_mult_du->second_op = std::move(du);

        auto fraction = std::make_unique<Binop_node>("/");
        fraction->first_op = std::move(v_mult_du);
        fraction->second_op = n.first_op->clone();

        // 3. Строим левую часть скобки: v' * ln(u)
        auto ln_u = std::make_unique<Func_node>("log");
        ln_u->arg = n.first_op->clone();

        auto dv_mult_lnu = std::make_unique<Binop_node>("*");
        dv_mult_lnu->first_op = std::move(dv);
        dv_mult_lnu->second_op = std::move(ln_u);

        // 4. Складываем части в скобку: (v' * ln(u) + (v * u') / u)
        auto bracket = std::make_unique<Binop_node>("+");
        bracket->first_op = std::move(dv_mult_lnu);
        bracket->second_op = std::move(fraction);

        // 5. Итоговый результат: u^v * (скобка)
        auto final_res = std::make_unique<Binop_node>("*");
        final_res->first_op = n.clone(); // u^v в исходном виде
        final_res->second_op = std::move(bracket);

        result = std::move(final_res);
    }
}

void Derivative_visitor::visit(class Func_node &n) {
   n.arg->accept(*this);
   auto arg_d = std::move(result);

   auto mult = std::make_unique<Binop_node>("*");
   mult->first_op = std::move(context.get_deriv(n.get_name(), std::move(n.arg)));
   mult->second_op = std::move(arg_d);
}

void Derivative_visitor::visit(class Unop_node &n) {
    auto unop = std::make_unique<Unop_node>(n.get_operation());
    n.op->accept(*this);
    unop->op = std::move(result);
    result = std::move(unop);
}

Derivative_visitor::~Derivative_visitor() = default;

Derivative_visitor::Derivative_visitor(Derivative_context& ctx): context(ctx){};