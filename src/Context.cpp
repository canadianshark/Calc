#include "Context.h"

Parser_context::Parser_context() {
    functions = {
            "sin",
            "cos",
            "tan",
            "asin",
            "acos",
            "atan",
            "exp",
            "log",
            "sqrt"
    };
    operation_table = {
            {"+", 10},
            {"-", 10},
            {"*", 20},
            {"/", 20},
            {"^", -30}
    };

}

int Parser_context::get_priority(std::string op) const{
    auto operation = operation_table.find(op);
    if(operation != operation_table.end()){
        return abs(operation->second);
    }
    throw std::invalid_argument("ERROR: unknown operation, unable to access priority");
}

bool Parser_context::left_asociative(std::string op) const {
    auto operation = operation_table.find(op);
    if(operation != operation_table.end()){
        return (operation->second > 0);
    }
    throw std::invalid_argument("ERROR: unknown operation, unable to access asociativness");
}