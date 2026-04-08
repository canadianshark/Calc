#pragma once
#include "AST.h"
#include "Lexer.h"
#include "Context.h"

class Parser{
private:
    const Parser_context& context;
    Lexer token_stream;
    std::unique_ptr<AST_node> parse_atom();
    std::unique_ptr<Binop_node> parse_binop();
    std::unique_ptr<AST_node> parse_right_side(std::unique_ptr<AST_node> left_side);
    std::unique_ptr<AST_node> parse_expression();
public:
    Parser(Lexer l, Parser_context ctx);
    AST parse_all();

};