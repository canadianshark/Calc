#include <iostream>
#include <windows.h>
#include "lexer.h"
#include "Parser.h"
#include "AST.h"

int main() {
   std::cout << "Give an expression" << "\n";
   std::string expr;
   std::getline(std::cin, expr);  // ✅ читаем всю строку, включая пробелы

   if (expr.empty()) {
      std::cerr << "Empty expression!" << std::endl;
      return 1;
   }
   Lexer l{expr};
   Parser_context c;
   Parser p{l,c};
   Printing_visitor vis;
   AST tree = p.parse_all();
   tree.accept(vis);
   std::cout << vis.result << "\n";
}
