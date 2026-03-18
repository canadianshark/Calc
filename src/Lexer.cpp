#include "Lexer.h"

Token::Token(Lexem type_, std::string str_): data(str_), type(type_) {}

Lexer::Lexer(std::string str): expression(str), position(0) {}

char Lexer::char_peek() {
    if(position < expression.length()){
        return expression.at(position);
    }
    position = 0;
    return expression.at(position);

}

char Lexer::char_next() {
    if(position < expression.length()-1){
       char res = expression.at(position);
       position++;
       return  res;
    }
    position = 0;
    return expression.at(position);
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
    std::cout << "We are scanning damn number" << std::endl;
    std::string data{};
    char cur_char = Lexer::char_peek();
    size_t count = 0;
    while(state != Scan_state::EXIT){

        switch (get_char_type(cur_char)) {

            case Char_type::NUMBER:
                if(state == Scan_state::DEFAULT){
                    state = Scan_state::NUMBER;
                }

                if(state == Scan_state::NUMBER){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }

                if(state == Scan_state::LEAD_ZERO){
                   throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Leading zeros are not allowed!");
                }

                if(state == Scan_state::POINT){
                    state = Scan_state::FRACTIONAl;
                }

                if(state == Scan_state::FRACTIONAl){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }
                break;

            case Char_type::LETTER:
                throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Letter appeared in a middle of a number! ");
                break;

            case Char_type::ZERO:
                if(state == Scan_state::DEFAULT){
                    state = Scan_state::LEAD_ZERO;
                }

                if(state == Scan_state::NUMBER){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }

                if(state == Scan_state::LEAD_ZERO){
                    throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Leading zeros are not allowed!");
                }
                if(state == Scan_state::POINT){
                    state = Scan_state::FRACTIONAl;
                }

                if(state == Scan_state::FRACTIONAl){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }
                break;

            case Char_type::POINT:
                if(state == Scan_state::DEFAULT){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }

                if(state == Scan_state::NUMBER){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }

                if(state == Scan_state::LEAD_ZERO){
                    state = Scan_state::POINT;
                }

                if(state == Scan_state::POINT){
                    throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Two points in a row(\"..\") undefined for a number");
                }

                if(state == Scan_state::FRACTIONAl){
                    throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Point is not allowed in already fractional part of a number");
                }
                break;

            case Char_type::MISC:
                if(state == Scan_state::DEFAULT){
                    // blank(Состояние не меняется, но так смотреть как на автомат удобнее)
                }

                if(state == Scan_state::NUMBER){
                    state = Scan_state::EXIT;
                }

                if(state == Scan_state::LEAD_ZERO){
                   state = Scan_state::EXIT;
                }
                if(state == Scan_state::POINT){
                    throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Point without fractional part after it is not allowed");
                }
                if(state == Scan_state::FRACTIONAl){
                    state = Scan_state::EXIT;
                }

                break;

            default:
                throw std::invalid_argument("ERROR: problem occurred while tokenizing a number! Unknown type of character appeared!");
                break;


        }
        if(state != Scan_state::EXIT){
            data += cur_char;
            cur_char = char_next();
            count++;
        }
        else
        {
            Token result(Lexem::NUMBER, data);
            state = Scan_state::DEFAULT;
            return result;
        }
   }
}


Token Lexer::scan_identificator(){
    std::string data{};
    char cur_char = Lexer::char_peek();
    size_t count = 0;
    while(state != Scan_state::EXIT){

        switch (get_char_type(cur_char)) {

            case Char_type::LETTER:
            case Char_type::NUMBER:
            case Char_type::ZERO:
            case Char_type::UNDERSCORE:
                break;

            case Char_type::MISC:
                state = Scan_state::EXIT;
                break;

            default:
                throw std::invalid_argument("ERROR: problem occurred while tokenizing an identificator! Unknown type of character appeared!");

        }
        if(state != Scan_state::EXIT){
            data += cur_char;
            cur_char = char_next();
            count++;
        }
        else
        {
            Token result(Lexem::IDENTIFICATOR, data);
            state = Scan_state::DEFAULT;
            return result;
        }
    }
}




Token Lexer::peek() {
    if(position > expression.length()){
        throw std::out_of_range("position of lexing jumped out of expression");
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

Token Lexer::next() {
    Token res = peek();
    return res;
}

void Lexer::dump(Token_dumpster d) {
    while (position < expression.length()){
        d.dumped.push_back(next());
    }
}