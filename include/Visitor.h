#pragma once
#include <memory>
#include <iostream>
#include "Context.h"
#include "AST.h"

class AST_node;

class Visitor{
public:
    virtual ~Visitor() = default;
    virtual void visit(class Number_node& node) = 0;
    virtual void visit(class Variable_node& node) = 0;
    virtual void visit(class Binop_node& node) = 0;
    virtual void visit(class Unop_node& node) = 0;
    virtual void visit(class Func_node& node) = 0;
};

class Polish_visitor: public Visitor{
public:
    std::string result;
   void visit(class Number_node& node) override;
   void visit(class Variable_node& node) override;
   void visit(class Binop_node& node) override;
   void visit(class Unop_node& node) override;
   void visit(class Func_node& node) override;
};

class Printing_visitor: public Visitor{
public:
    std::string result;
    void visit(class Number_node& node) override;
    void visit(class Variable_node& node) override;
    void visit(class Binop_node& node) override;
    void visit(class Unop_node& node) override;
    void visit(class Func_node& node) override;
};

class Calculating_visitor: public Visitor{
public:
    Calculating_context context;
    std::vector<double> stack;
    Calculating_visitor(Calculating_context& ctx);
    double get_result();
    void visit(class Number_node& node) override;
    void visit(class Variable_node& node) override;
    void visit(class Binop_node& node) override;
    void visit(class Unop_node& node) override;
    void visit(class Func_node& node) override;
};

class Derivative_visitor: public Visitor{
public:
    std::unique_ptr<AST_node> result;
    std::string var;
    void visit(class Number_node& node) override;
    void visit(class Variable_node& node) override;
    void visit(class Binop_node& node) override;
    void visit(class Unop_node& node) override;
    void visit(class Func_node& node) override;
};