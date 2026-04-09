#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Parser.h"
#include "Visitor.h"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Parser.h"
#include "Visitor.h"

int main() {
    Parser_context p_ctx;
    try {
        std::cerr << "Enter command (evaluate, derivative, evaluate_derivative): ";
        std::string command;
        if (!(std::cin >> command)) return 0;

        if (command != "evaluate" && command != "derivative" && command != "evaluate_derivative") {
            throw std::invalid_argument("Unknown command: " + command);
        }

        std::cerr << "Enter number of variables: ";
        int n;
        if (!(std::cin >> n)) return 0;

        std::vector<std::string> var_names(n);
        if (n > 0) std::cerr << "Enter " << n << " variable names: ";
        for (int i = 0; i < n; ++i) {
            std::cin >> var_names[i];
            if (p_ctx.functions.contains(var_names[i])) {
                throw std::invalid_argument("Variable name conflicts with function name: " + var_names[i]);
            }
            p_ctx.variables.insert(var_names[i]);
        }

        Calculating_context c_ctx;
        if (n > 0) std::cerr << "Enter " << n << " variable values: ";
        for (int i = 0; i < n; ++i) {
            double val;
            if (!(std::cin >> val)) throw std::invalid_argument("Invalid value for variable " + var_names[i]);
            c_ctx.variables[var_names[i]] = val;
        }

        std::cerr << "Enter expression: ";
        std::cin.ignore(1000, '\n');
        std::string expression;
        if (!std::getline(std::cin, expression) || expression.empty()) {
            throw std::invalid_argument("Empty expression");
        }

        Lexer l(expression);
        Parser p(l, p_ctx);
        AST tree = p.parse_all();

        std::cout << std::fixed << std::setprecision(6);

        if (command == "evaluate") {
            Calculating_visitor cv(c_ctx);
            tree.accept(cv);
            std::cout << cv.get_result() << std::endl;
        }
        else {
            if (var_names.empty() && (expression.find('x') != std::string::npos)) {

                var_names.push_back("x");
            } else if (var_names.empty()) {
                throw std::invalid_argument("No variables provided for differentiation");
            }

            Derivative_context d_ctx;
            d_ctx.var = var_names[0];
            Derivative_visitor dv(d_ctx);
            tree.accept(dv);

            if (command == "derivative") {
                Printing_visitor pv;
                dv.result->accept(pv);
                std::cout << pv.result << std::endl;
            } else {
                Calculating_visitor cv(c_ctx);
                dv.result->accept(cv);
                std::cout << cv.get_result() << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cout << "ERROR " << e.what() << std::endl;
    }

    return 0;
}

