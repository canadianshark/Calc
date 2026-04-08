#pragma once
#include "unordered_map"
#include "unordered_set"
#include "stdexcept"
#include <string>

struct Parser_context{
    std::unordered_map<std::string , int> operation_table;
    std::unordered_set<std::string> functions;
    std::unordered_set<std::string> variables;

    Parser_context();
    int get_priority(std::string op) const;
    bool left_asociative(std::string op) const;
};