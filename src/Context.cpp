#include "Context.h"
#include <math.h>
#include <memory>
#include "AST.h"

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
    functions["atan"] = [](double x) {return atan(x);};
    functions["exp"] = [](double x) {return exp(x);};
    operation_rules["+"] = [](double x, double y) {return x+y;};
    operation_rules["-"] = [](double x, double y) {return x-y;};
    operation_rules["*"] = [](double x, double y) {return x*y;};
    operation_rules["^"] = [](double x, double y) {return pow(x,y);};

    functions["sqrt"] = [](double x) {
        if (x < 0) throw std::domain_error("Math Error: sqrt of negative number");
        return sqrt(x);
    };
    functions["log"] = [](double x) {
        if (x <= 0) throw std::domain_error("Math Error: log of non-positive number");
        return log(x);
    };
    functions["asin"] = [](double x) {
        if (x < -1 || x > 1) throw std::domain_error("Math Error: asin out of [-1, 1]");
        return asin(x);
    };
    functions["acos"] = [](double x) {
        if (x < -1 || x > 1) throw std::domain_error("Math Error: acos out of [-1, 1]");
        return acos(x);
    };
    operation_rules["/"] = [](double x, double y) {
        if (y == 0) throw std::domain_error("Math Error: division by zero");
        return x / y;
    };

};

std::unique_ptr<AST_node> Derivative_context::get_deriv(std::string name, std::unique_ptr<AST_node> arg) {
    if(name == "sin"){
        auto res = std::make_unique<Func_node>("cos");
        res->arg = std::move(arg);
        return res;
    }
    if(name == "cos"){
        auto res = std::make_unique<Unop_node>("-");
        auto operand = std::make_unique<Func_node>("sin");
        operand->arg = std::move(arg);
        res->op = std::move(operand);
        return res;
    }
    if (name == "tan") {
        // 1 / cos(arg)^2
        auto power = std::make_unique<Binop_node>("^");
        auto cos_node = std::make_unique<Func_node>("cos");
        cos_node->arg = std::move(arg);
        power->first_op = std::move(cos_node);
        power->second_op = std::make_unique<Number_node>("2");

        auto res = std::make_unique<Binop_node>("/");
        res->first_op = std::make_unique<Number_node>("1");
        res->second_op = std::move(power);
        return res;
    }

    if (name == "asin") {
        // 1 / sqrt(1 - arg^2)
        auto minus = std::make_unique<Binop_node>("-");
        minus->first_op = std::make_unique<Number_node>("1");
        auto pwr = std::make_unique<Binop_node>("^");
        pwr->first_op = std::move(arg);
        pwr->second_op = std::make_unique<Number_node>("2");
        minus->second_op = std::move(pwr);

        auto sq = std::make_unique<Func_node>("sqrt");
        sq->arg = std::move(minus);

        auto res = std::make_unique<Binop_node>("/");
        res->first_op = std::make_unique<Number_node>("1");
        res->second_op = std::move(sq);
        return res;
    }

    if (name == "acos") {
        auto res = std::make_unique<Unop_node>("-");
        res->op = get_deriv("asin", std::move(arg));
        return res;
    }

    if (name == "atan") {
        auto plus = std::make_unique<Binop_node>("+");
        plus->first_op = std::make_unique<Number_node>("1");
        auto pwr = std::make_unique<Binop_node>("^");
        pwr->first_op = std::move(arg);
        pwr->second_op = std::make_unique<Number_node>("2");
        plus->second_op = std::move(pwr);

        auto res = std::make_unique<Binop_node>("/");
        res->first_op = std::make_unique<Number_node>("1");
        res->second_op = std::move(plus);
        return res;
    }

    if (name == "exp") {
        auto res = std::make_unique<Func_node>("exp");
        res->arg = std::move(arg);
        return res;
    }

    if (name == "log") {
        auto res = std::make_unique<Binop_node>("/");
        res->first_op = std::make_unique<Number_node>("1");
        res->second_op = std::move(arg);
        return res;
    }

    if (name == "sqrt") {
        auto mult = std::make_unique<Binop_node>("*");
        mult->first_op = std::make_unique<Number_node>("2");
        auto sq = std::make_unique<Func_node>("sqrt");
        sq->arg = std::move(arg);
        mult->second_op = std::move(sq);

        auto res = std::make_unique<Binop_node>("/");
        res->first_op = std::make_unique<Number_node>("1");
        res->second_op = std::move(mult);
        return res;
    }

}
