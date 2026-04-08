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