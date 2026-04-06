#include "Lexer.h"


Token::Token(Lexem type_, std::string str_): data(str_), type(type_) {}

Lexer::Lexer(std::string str): expression(str), position(0) {}

char Lexer::char_peek() {
    if(position < expression.length()){
        return expression.at(position);
    }
    else{
        throw std::runtime_error("ERROR: Lexer jumped out of bounds of the expression");
    }
}

char Lexer::char_next() {
    if(position < expression.length()-1){
       char res = char_peek();
       position++;
    }
    else{
        if(position < expression.length()){
            return char_peek();
        }
        throw std::runtime_error("ERROR: Lexer jumped out of bounds of the expression");
    }
}

Char_type Lexer::get_char_type(char c) {
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
        return Char_type::LETTER;
    }
    if(c >=  '0' && c <= '9'){
        return c == '0' ? Char_type::ZERO : Char_type::NUMBER;
    }
    switch (c) {
        case '.': return Char_type::POINT;
        case '+': case '-': case '*': case '/': case '^': case '(': case ')': case ' ': return Char_type::MISC;
        default: return Char_type::UNKNOWN;
    }
}

Token Lexer::misc_to_token(char c) {
    switch (c) {
        case '+':
            return Token(Lexem::PLUS, "+");
            break;
        case '-':
            return Token(Lexem::MINUS, "-");
            break;
        case '*':
            return Token(Lexem::MULTIPLICATION, "*");
            break;
        case '/':
            return Token(Lexem::DIVISION, "/");
            break;
        case '^':
            return Token(Lexem::POWER, "^");
            break;
        default: throw std::invalid_argument("ERROR: lexing problem, unknown singular symbol");
    }

}

Token Lexer::scan_number(){
    std::string data{};
    char cur_char = Lexer::char_next();
    state = Scan_state::DEFAULT;
    while (state != Scan_state::EXIT) {
        Char_type cur_char_type = get_char_type(cur_char);
        switch (state) {
            case Scan_state::DEFAULT:{
                if(cur_char_type == Char_type::NUMBER){
                    state = Scan_state::NUMBER;
                    break;
                }
                if(cur_char_type == Char_type::ZERO){
                    state = Scan_state::LEAD_ZERO;
                    break;
                }
                throw std::runtime_error("ERROR: started tokenizing non-number lexem as number");
                break;
            }
            case Scan_state::NUMBER:{
                if(cur_char_type == Char_type::POINT){
                    state = Scan_state::POINT;
                    break;
                }
                if(cur_char_type == Char_type::MISC){
                    state = Scan_state::EXIT;
                    break;
                }
                throw std::invalid_argument("ERROR: invalid character inside the lexem that seemed to be a number");
                break;
            }

            case Scan_state::LEAD_ZERO:{
                if(cur_char_type == Char_type::POINT){
                    state = Scan_state::POINT;
                    break;
                }
                if(cur_char_type == Char_type::MISC){
                    state = Scan_state::EXIT;
                    break;
                }
                throw std::invalid_argument("ERROR: invalid character after the leading zero");
                break;
            }

            case Scan_state::POINT:{
                if(cur_char_type == Char_type::NUMBER || cur_char_type == Char_type::ZERO){
                    state = Scan_state::FRACTIONAl;
                    break;
                }
                throw std::runtime_error("ERROR: invalid character for a fractional part");
                break;
            }


            case Scan_state::FRACTIONAl:
                if(cur_char_type == Char_type::NUMBER || cur_char_type == Char_type::ZERO){
                    state = Scan_state::FRACTIONAl;
                    break;
                }
                if(cur_char_type == Char_type::MISC){
                    state = Scan_state::EXIT;
                    break;
                }
                throw std::runtime_error("ERROR: invalid character for a fractional part");
                break;
            default:
                throw std::invalid_argument("invalid character inside the lexem that seemed to be a number");
                break;
        }
        if(state == Scan_state::EXIT){
            data += cur_char;
            cur_char = char_next();
        }
        else{
            Token result(Lexem::NUMBER, data);
            return result;
        }
    }





}


Token Lexer::scan_identificator(){
    std::string data{};
    state = Scan_state::DEFAULT;
    char cur_char = Lexer::char_next();
    Char_type cur_char_type = get_char_type(cur_char);
    while(cur_char_type != Char_type::MISC){
        if(cur_char_type == Char_type::UNKNOWN){
            throw std::invalid_argument("ERROR: invalind character appeared while was tokenizing an ifentificator");
        }
        if(cur_char_type == Char_type::MISC){
            Token result(Lexem::IDENTIFICATOR, data);
            state = Scan_state::DEFAULT;
            return result;
        }
        data += cur_char;
        cur_char = char_next();

    }
}




Token Lexer::peek() {
    if(position > expression.length()){
        throw std::runtime_error("ERROR: Lexer jumped out of bounds of the expression");
    }
    size_t starting_pos = position;
    Token result = next();
    position = starting_pos;
    return result;
}

Token Lexer::next() {
    if(position > expression.length()){
        throw std::runtime_error("ERROR: Lexer jumped out of bounds of the expression");,
    }
    char first_char = char_peek();

    switch (get_char_type(first_char)) {
        case Char_type::NUMBER: case Char_type::ZERO:
            try{
                return scan_number();
            }
            catch(...){
                throw;
            }
            break;

        case Char_type::LETTER: case Char_type::UNDERSCORE:
            try {
                return scan_identificator();
            }
            catch(...){
                throw;
            }
            break;
        case Char_type::MISC:
            try {
                return misc_to_token(first_char);
            }
            catch(...){
                throw;
            }
            break;

        default: throw std::invalid_argument("ERROR: problem occurred while lexing! Unknown character!");

    }
}
