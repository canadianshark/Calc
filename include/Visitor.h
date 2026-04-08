#pragma once
#include <memory>
#include <iostream>

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