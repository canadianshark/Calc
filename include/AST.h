#pragma once
#include "memory"
#include "Visitor.h"
#include <string>

class AST_node{
public:
   virtual ~AST_node() = default;
   virtual std::unique_ptr<AST_node> clone() = 0;
   virtual std::string dump_data() = 0;
   virtual void accept(Visitor& v) = 0;
};

class AST{
private:
    std::unique_ptr<AST_node> root;
public:
    AST(std::unique_ptr<AST_node> root_);
    AST clone_tree();
    void accept(Visitor& v);
};

class Number_node: public AST_node{
private:
    std::string value;
public:
    explicit Number_node(std::string str);
    std::string get_value();
    std::unique_ptr<AST_node> clone() override;
    std::string dump_data() override;
    void accept(Visitor& v) override;

};

class Variable_node: public AST_node{
private:
    std::string name;
public:
    explicit Variable_node(std::string str);
    std::string get_name();
    std::unique_ptr<AST_node> clone() override;
    std::string dump_data() override;
    void accept(Visitor& v) override;
};

class Binop_node: public AST_node{
private:
    std::string operation;
public:
    std::unique_ptr<AST_node> first_op;
    std::unique_ptr<AST_node> second_op;

    explicit Binop_node(std::string str);
    std::string get_operation();
    std::unique_ptr<AST_node> clone() override;
    std::string dump_data() override;
    void accept(Visitor& v) override;
};

class Unop_node: public AST_node{
private:
    std::string operation;
public:
    std::unique_ptr<AST_node> op;

    explicit Unop_node(std::string str);
    std::string get_operation();
    std::unique_ptr<AST_node> clone() override;
    std::string dump_data() override;
    void accept(Visitor& v) override;
};

class Func_node: public AST_node{
private:
    std::string name;
public:
    std::unique_ptr<AST_node> arg;
    explicit Func_node(std::string str);
    std::string get_name();
    std::unique_ptr<AST_node> clone() override;
    std::string dump_data() override;
    void accept(Visitor& v) override;
};




