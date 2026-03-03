#pragma once
#include "string"

enum class Lexem{
    NUMBER,
    IDENTIFICATOR,
    BRACKET,
    BINOP
};

enum class Scan_state{
    DEFAULT, NUMBER, LETTER
};

struct Token{
    Lexem type;
    std::string data;
    Token(Lexem, std::string);

};


class Lexer{
    std::string expression;
    size_t position;
    // Scan_state state;
    char char_peek();
    char char_next();
    Token scan_number();
    Token scan_identificator();
 public:
    Lexer(std::string);
    Token peek();
    Token next();
};