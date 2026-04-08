#include "Parser.h"

Parser::Parser(Lexer l, Parser_context ctx): token_stream(l), context(ctx){};

std::unique_ptr<AST_node> Parser::parse_atom() {
    Token cur_token = token_stream.peek();
    switch(cur_token.lexem){
        case Lexem::NUMBER:{
            token_stream.next();
            Number_node node{cur_token.data};
            return std::make_unique<Number_node>(std::move(node));
            break;
        }
        case Lexem::IDENTIFICATOR: {
            if(context.functions.contains(cur_token.data)){
                token_stream.next();
                Func_node node{cur_token.data};
                if(token_stream.peek().lexem == Lexem::BRACKET && token_stream.peek().data == "("){
                   node.arg = parse_atom();
                }
                else{
                    throw std::invalid_argument("ERROR: function call should be followed by arguments in ()");
                }

                return std::make_unique<Func_node>(std::move(node));
                break;
            }
            if(context.variables.contains(cur_token.data)){
                token_stream.next();
                Variable_node node{cur_token.data};
                return std::make_unique<Variable_node>(std::move(node));
                break;
            }
            throw std::invalid_argument("ERROR:" + cur_token.data + "is not a variable or function");
        }
        case Lexem::PLUS:
        case Lexem::MINUS:{
            token_stream.next();
            Unop_node node{cur_token.data};
            node.op = parse_atom();
            return std::make_unique<Unop_node>(std::move(node));
            break;
        }
        case Lexem::BRACKET:{
            if(cur_token.data == "("){
                token_stream.next();
                auto res = parse_expression();
                if(token_stream.peek().data != ")"){
                    throw std::invalid_argument("ERROR: \")\" expected");
                }
                token_stream.next();
                return res;
            }
            throw std::invalid_argument("ERROR: unexpected bracket");
            break;
        }
        default:
            throw std::invalid_argument("ERROR: invalid operand or unexpected end of the expression");
    }
}

std::unique_ptr<Binop_node> Parser::parse_binop(){
    Token cur_token = token_stream.next();
    if(context.operation_table.contains(cur_token.data)){
        std::unique_ptr res = std::make_unique<Binop_node>(cur_token.data);
        return res;

    } else{
        throw std::invalid_argument("ERROR: invalid token have been interpreted like a binary operation(In expression atom(func,var, num or unop with arg) should be followed by a binop)");
    }

}

std::unique_ptr<AST_node> Parser::parse_expression() {
    std::unique_ptr<AST_node> atom = parse_atom();
    std::unique_ptr<AST_node> full_tree = parse_right_side(std::move(atom));
    return full_tree;
}

std::unique_ptr<AST_node> Parser::parse_right_side(std::unique_ptr<AST_node> left_side) {
    if(token_stream.peek().lexem == Lexem::END || token_stream.peek().data == ")") return left_side;
    std::unique_ptr<Binop_node> binop = parse_binop();
    int priority_1 = context.get_priority(binop->get_operation());
    binop->first_op = std::move(left_side);

    std::unique_ptr<AST_node> atom = parse_atom();
    if(token_stream.peek().lexem == Lexem::END || token_stream.peek().data == ")"){
       binop->second_op = std::move(atom);
       return std::move(binop);
    }
    else{
       Token new_op = token_stream.peek();
       int priority_2 = context.get_priority(new_op.data);
       if(priority_1 > priority_2){
           binop->second_op = std::move(atom);
           return parse_right_side(std::move(binop));
       }
       if(priority_1 < priority_2){
           binop->second_op = parse_right_side(std::move(atom));
           return std::move(binop);
       }
       if(priority_1 == priority_2 && !context.left_asociative(binop->get_operation())){
           binop->second_op = parse_right_side(std::move(atom));
           return std::move(binop);
       }
        if(priority_1 == priority_2 && context.left_asociative(binop->get_operation())){
            binop->second_op = std::move(atom);
            return parse_right_side(std::move(binop));
        }

    }
    throw std::runtime_error("I am not supposed to appear here");
}

AST Parser::parse_all() {
    AST result = parse_expression();

    if(token_stream.peek().lexem != Lexem::END){
        throw std::invalid_argument("ERROR: not all expression parsed probably due to unexpected \")\"");
    }
    return result;
}

