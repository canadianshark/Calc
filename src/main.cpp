#include <iostream>
#include <windows.h>
#include "lexer.h"

int main() {
   std::cout << "Give an expression" << "\n";
   std::string expr;
   std::cin >> expr;
   Lexer l{expr};
   std::cout << "Printing token by token:" << "\n";
   l.dump_tokens();


}
