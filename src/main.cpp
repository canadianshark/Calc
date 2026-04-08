#include <iostream>
#include <windows.h>
#include "lexer.h"
#include "Parser.h"
#include "AST.h"

int main() {
   std::cout << "Give an expression" << "\n";
   std::string expr;
   std::cin >> expr;
   Lexer l{expr};
   Parser_context c;
   Parser p{l,c};
   Polish_visitor vis;
   AST tree = p.parse_all();
   tree.accept(vis);

}
