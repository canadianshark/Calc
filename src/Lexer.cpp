#include "Lexer.h"

Token::Token(Lexem type_, std::string str_): data(str_), type(type_) {}

Lexer::Lexer(std::string str): expression(str), position(0) {}

char Lexer::char_peek() {
    if(position < expression.length()){
        return expression.at(position);
    }
}

char Lexer::char_next() {
    if(position < expression.length()-1){
        position++;
        return expression.at(position);
    }
}

