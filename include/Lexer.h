#pragma once
#include "string"
#include "stdexcept"
#include "iostream"
#include "vector"

enum class Lexem{
    NUMBER,
    IDENTIFICATOR,
    BRACKET,
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION,
    POWER
};

enum class Scan_state{
    DEFAULT, NUMBER, LEAD_ZERO, POINT, FRACTIONAl,ID, EXIT
};

enum class Char_type{
    NUMBER, ZERO, LETTER, POINT, MISC, UNDERSCORE, UNKNOWN
};

struct Token{
    const Lexem type;
    const std::string data;
    Token(Lexem, std::string);

};
struct Token_dumpster{
    std::vector<Token> dumped;
};

class Lexer{
    std::string expression;
    size_t position;
    Scan_state state = Scan_state::DEFAULT;
    char char_peek();
    char char_next();
    Char_type get_char_type(char c);
    Token scan_number();
    Token scan_identificator();
    Token misc_to_token(char c);
 public:
    Lexer(std::string);
    Token peek();
    Token next();
    void dump(Token_dumpster d);
};

