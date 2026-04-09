#pragma once
#include "unordered_map"
#include "unordered_set"
#include "stdexcept"
#include <string>
#include <functional>
#include <memory>

class AST_node;

struct Parser_context{
    std::unordered_map<std::string , int> operation_table;
    std::unordered_set<std::string> functions;
    std::unordered_set<std::string> variables;

    Parser_context();
    int get_priority(std::string op) const;
    bool left_asociative(std::string op) const;
};

struct Calculating_context{
    std::unordered_map<std::string, double> variables;
    std::unordered_map<std::string, std::function<double(double)>> functions;
    std::unordered_map<std::string, std::function<double(double, double )>> operation_rules;
    Calculating_context();

};

struct Derivative_context{
    std::unique_ptr<AST_node> get_deriv(std::string name,std::unique_ptr<AST_node> arg_d);
    std::string var;

};