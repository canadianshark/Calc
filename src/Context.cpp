#include "Context.h"
#include <math.h>

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

Calculating_context::Calculating_context(){
    functions["sin"] = [](double x) {return sin(x);};
    functions["cos"] = [](double x) {return cos(x);};
    functions["tan"] = [](double x) {return tan(x);};
    functions["asin"] = [](double x) {return asin(x);};
    functions["acos"] = [](double x) {return acos(x);};
    functions["atan"] = [](double x) {return atan(x);};
    functions["exp"] = [](double x) {return exp(x);};
    functions["log"] = [](double x) {return log(x);};
    functions["sqrt"] = [](double x) {return sqrt(x);};
    operation_rules["+"] = [](double x, double y) {return x+y;};
    operation_rules["-"] = [](double x, double y) {return x-y;};
    operation_rules["*"] = [](double x, double y) {return x*y;};
    operation_rules["/"] = [](double x, double y) {return x/y;};
    operation_rules["^"] = [](double x, double y) {return pow(x,y);};
}
