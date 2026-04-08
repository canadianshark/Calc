#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdexcept>
#include <iostream>
#include "lexer.h"
#include "Parser.h"
#include "AST.h"

void log(const std::string& msg) {
    std::cerr << msg << std::flush;
}

std::string parse_to_polish(const std::string& expression, Parser_context& context) {
    Lexer lexer(expression);
    Parser parser(lexer, context);
    AST tree = parser.parse_all();
    Polish_visitor visitor;
    tree.accept(visitor);
    return visitor.result;
}

std::string parse_to_polish(const std::string& expression) {
    Parser_context context;
    return parse_to_polish(expression, context);
}

TEST_CASE("Parser handles basic numbers", "[parser][numbers]") {
    log("\n=== Testing basic number parsing ===\n");

    SECTION("Single integer") {
        log("  Testing single integer...\n");
        std::string result = parse_to_polish("42");
        REQUIRE(result == "42");
        log("  OK\n");
    }

    SECTION("Single decimal") {
        log("  Testing single decimal...\n");
        std::string result = parse_to_polish("3.14");
        REQUIRE(result == "3.14");
        log("  OK\n");
    }

    SECTION("Zero") {
        log("  Testing zero...\n");
        std::string result = parse_to_polish("0");
        REQUIRE(result == "0");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles basic arithmetic", "[parser][arithmetic]") {
    log("\n=== Testing basic arithmetic operations ===\n");

    SECTION("Simple addition") {
        log("  Testing addition...\n");
        std::string result = parse_to_polish("2+3");
        REQUIRE(result == "+(2,3)");
        log("  OK\n");
    }

    SECTION("Simple subtraction") {
        log("  Testing subtraction...\n");
        std::string result = parse_to_polish("5-2");
        REQUIRE(result == "-(5,2)");
        log("  OK\n");
    }

    SECTION("Simple multiplication") {
        log("  Testing multiplication...\n");
        std::string result = parse_to_polish("4*6");
        REQUIRE(result == "*(4,6)");
        log("  OK\n");
    }

    SECTION("Simple division") {
        log("  Testing division...\n");
        std::string result = parse_to_polish("10/2");
        REQUIRE(result == "/(10,2)");
        log("  OK\n");
    }

    SECTION("Power operation") {
        log("  Testing power...\n");
        std::string result = parse_to_polish("2^3");
        REQUIRE(result == "^(2,3)");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles operator precedence", "[parser][precedence]") {
    log("\n=== Testing operator precedence ===\n");

    SECTION("Multiplication before addition") {
        log("  Testing multiplication precedence...\n");
        std::string result = parse_to_polish("2+3*4");
        REQUIRE(result == "+(2,*(3,4))");
        log("  OK\n");
    }

    SECTION("Division before subtraction") {
        log("  Testing division precedence...\n");
        std::string result = parse_to_polish("10-6/2");
        REQUIRE(result == "-(10,/(6,2))");
        log("  OK\n");
    }

    SECTION("Power before multiplication") {
        log("  Testing power precedence...\n");
        std::string result = parse_to_polish("2*3^4");
        REQUIRE(result == "*(2,^(3,4))");
        log("  OK\n");
    }

    SECTION("Power before addition") {
        log("  Testing power vs addition...\n");
        std::string result = parse_to_polish("5+2^3");
        REQUIRE(result == "+(5,^(2,3))");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles parentheses", "[parser][parentheses]") {
    log("\n=== Testing parentheses handling ===\n");

    SECTION("Parentheses override precedence") {
        log("  Testing parentheses override...\n");
        std::string result = parse_to_polish("(2+3)*4");
        REQUIRE(result == "*(+(2,3),4)");
        log("  OK\n");
    }

    SECTION("Nested parentheses") {
        log("  Testing nested parentheses...\n");
        std::string result = parse_to_polish("((2+3)*4)/2");
        REQUIRE(result == "/(*(+(2,3),4),2)");
        log("  OK\n");
    }

    SECTION("Multiple parentheses") {
        log("  Testing multiple parentheses...\n");
        std::string result = parse_to_polish("(1+2)*(3+4)");
        REQUIRE(result == "*(+(1,2),+(3,4))");
        log("  OK\n");
    }

    SECTION("Parentheses with power") {
        log("  Testing parentheses with power...\n");
        std::string result = parse_to_polish("(2+3)^2");
        REQUIRE(result == "^(+(2,3),2)");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles unary operations", "[parser][unary]") {
    log("\n=== Testing unary operations ===\n");

    SECTION("Unary minus with number") {
        log("  Testing unary minus...\n");
        std::string result = parse_to_polish("-5");
        REQUIRE(result == "-(5)");
        log("  OK\n");
    }

    SECTION("Unary plus with number") {
        log("  Testing unary plus...\n");
        std::string result = parse_to_polish("+3");
        REQUIRE(result == "+(3)");
        log("  OK\n");
    }

    SECTION("Unary minus in expression") {
        log("  Testing unary minus in expression...\n");
        std::string result = parse_to_polish("2+-3");
        REQUIRE(result == "+(2,-(3))");
        log("  OK\n");
    }

    SECTION("Double unary minus") {
        log("  Testing double unary minus...\n");
        std::string result = parse_to_polish("--5");
        REQUIRE(result == "-(-(5))");
        log("  OK\n");
    }

    SECTION("Unary minus with parentheses") {
        log("  Testing unary minus with parentheses...\n");
        std::string result = parse_to_polish("-(2+3)");
        REQUIRE(result == "-(+(2,3))");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles variables", "[parser][variables]") {
    log("\n=== Testing variable handling ===\n");

    SECTION("Single variable") {
        log("  Testing single variable...\n");
        Parser_context context;
        context.variables.insert("x");
        std::string result = parse_to_polish("x", context);
        REQUIRE(result == "x");
        log("  OK\n");
    }

    SECTION("Variable in expression") {
        log("  Testing variable in expression...\n");
        Parser_context context;
        context.variables.insert("x");
        context.variables.insert("y");
        std::string result = parse_to_polish("x+y", context);
        REQUIRE(result == "+(x,y)");
        log("  OK\n");
    }

    SECTION("Variable with operations") {
        log("  Testing variable with operations...\n");
        Parser_context context;
        context.variables.insert("x");
        std::string result = parse_to_polish("2*x+1", context);
        REQUIRE(result == "+(*(2,x),1)");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles functions", "[parser][functions]") {
    log("\n=== Testing function handling ===\n");

    SECTION("Single function with number") {
        log("  Testing sin function...\n");
        std::string result = parse_to_polish("sin(0)");
        REQUIRE(result == "sin(0)");
        log("  OK\n");
    }

    SECTION("Function with expression") {
        log("  Testing sqrt with expression...\n");
        std::string result = parse_to_polish("sqrt(2+2)");
        REQUIRE(result == "sqrt(+(2,2))");
        log("  OK\n");
    }

    SECTION("Nested functions") {
        log("  Testing nested functions...\n");
        std::string result = parse_to_polish("sin(cos(0))");
        REQUIRE(result == "sin(cos(0))");
        log("  OK\n");
    }

    SECTION("Function in arithmetic expression") {
        log("  Testing function in arithmetic...\n");
        std::string result = parse_to_polish("sin(3.14)+1");
        REQUIRE(result == "+(sin(3.14),1)");
        log("  OK\n");
    }

    SECTION("Function with variable") {
        log("  Testing function with variable...\n");
        Parser_context context;
        context.variables.insert("x");
        std::string result = parse_to_polish("exp(x)", context);
        REQUIRE(result == "exp(x)");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles complex expressions", "[parser][complex]") {
    log("\n=== Testing complex expressions ===\n");

    SECTION("Multiple operators with precedence") {
        log("  Testing multiple operators...\n");
        std::string result = parse_to_polish("2+3*4-5/2");
        REQUIRE(result == "-(+(2,*(3,4)),/(5,2))");
        log("  OK\n");
    }

    SECTION("Power with multiplication") {
        log("  Testing power with multiplication...\n");
        std::string result = parse_to_polish("2*3^2+1");
        REQUIRE(result == "+(*(2,^(3,2)),1)");
        log("  OK\n");
    }

    SECTION("Right associativity of power") {
        log("  Testing power associativity...\n");
        std::string result = parse_to_polish("2^3^2");
        REQUIRE(result == "^(2,^(3,2))");
        log("  OK\n");
    }

    SECTION("Complex with parentheses") {
        log("  Testing complex parentheses...\n");
        std::string result = parse_to_polish("(2+3)*(4-1)/(5+2)");
        REQUIRE(result == "/(*(+(2,3),-(4,1)),+(5,2))");
        log("  OK\n");
    }

    SECTION("Expression with unary and binary") {
        log("  Testing unary and binary together...\n");
        std::string result = parse_to_polish("-2+3*-4");
        REQUIRE(result == "+(-(2),*(3,-(4)))");
        log("  OK\n");
    }
}

TEST_CASE("Parser handles expression with all features", "[parser][integration]") {
    log("\n=== Testing full integration ===\n");

    SECTION("All features combined") {
        log("  Testing all features together...\n");
        Parser_context context;
        context.variables.insert("x");
        context.variables.insert("y");
        std::string result = parse_to_polish("sin(x+1)*2+sqrt(4)-y", context);
        REQUIRE(result == "-+(*(sin(+(x,1)),2),sqrt(4),y)");
        log("  OK\n");
    }

    SECTION("Deep nesting") {
        log("  Testing deep nesting...\n");
        std::string result = parse_to_polish("(1+(2+(3+(4+5))))");
        REQUIRE(result == "+(1,+(2,+(3,+(4,5))))");
        log("  OK\n");
    }

    SECTION("Multiple functions") {
        log("  Testing multiple functions...\n");
        std::string result = parse_to_polish("sin(cos(tan(0)))");
        REQUIRE(result == "sin(cos(tan(0)))");
        log("  OK\n");
    }
}

TEST_CASE("Parser error handling", "[parser][errors]") {
    log("\n=== Testing error handling ===\n");

    SECTION("Missing closing parenthesis") {
        log("  Testing missing closing parenthesis...\n");
        Lexer lexer("(2+3");
        Parser_context context;
        Parser parser(lexer, context);
        REQUIRE_THROWS_AS(parser.parse_all(), std::invalid_argument);
        log("  OK\n");
    }

    SECTION("Extra closing parenthesis") {
        log("  Testing extra closing parenthesis...\n");
        REQUIRE_THROWS_AS(parse_to_polish("2+3)"), std::invalid_argument);
        log("  OK\n");
    }

    SECTION("Missing function argument parentheses") {
        log("  Testing missing function parentheses...\n");
        Lexer lexer("sin 0");
        Parser_context context;
        Parser parser(lexer, context);
        REQUIRE_THROWS_AS(parser.parse_all(), std::invalid_argument);
        log("  OK\n");
    }

    SECTION("Unknown variable") {
        log("  Testing unknown variable...\n");
        REQUIRE_THROWS_AS(parse_to_polish("x+1"), std::invalid_argument);
        log("  OK\n");
    }

    SECTION("Invalid operator sequence") {
        log("  Testing invalid operator sequence...\n");
        Lexer lexer("2++3");
        Parser_context context;
        Parser parser(lexer, context);
        REQUIRE_THROWS_AS(parser.parse_all(), std::invalid_argument);
        log("  OK\n");
    }

    SECTION("Empty expression after operator") {
        log("  Testing empty expression after operator...\n");
        Lexer lexer("2+");
        Parser_context context;
        Parser parser(lexer, context);
        REQUIRE_THROWS_AS(parser.parse_all(), std::invalid_argument);
        log("  OK\n");
    }
}

TEST_CASE("Parser handles edge cases", "[parser][edge]") {
    log("\n=== Testing edge cases ===\n");

    SECTION("Single number with whitespace") {
        log("  Testing number with whitespace...\n");
        std::string result = parse_to_polish("  123  ");
        REQUIRE(result == "123");
        log("  OK\n");
    }

    SECTION("Expression with spaces") {
        log("  Testing expression with spaces...\n");
        std::string result = parse_to_polish(" 2 + 3 * 4 ");
        REQUIRE(result == "+(2,*(3,4))");
        log("  OK\n");
    }

    SECTION("Multiple unary operators") {
        log("  Testing multiple unary operators...\n");
        std::string result = parse_to_polish("---5");
        REQUIRE(result == "-(-(-(5)))");
        log("  OK\n");
    }

    SECTION("Zero in operations") {
        log("  Testing zero in operations...\n");
        std::string result = parse_to_polish("0+0*0");
        REQUIRE(result == "+(0,*(0,0))");
        log("  OK\n");
    }
}

TEST_CASE("Parser preserves expression structure", "[parser][structure]") {
    log("\n=== Testing structure preservation ===\n");

    SECTION("Same expression different parentheses") {
        log("  Testing parentheses equivalence...\n");
        std::string result1 = parse_to_polish("2+3+4");
        std::string result2 = parse_to_polish("(2+3)+4");
        REQUIRE(result1 == result2);
        log("  OK\n");
    }

    SECTION("Commutative operations structure") {
        log("  Testing commutative operations...\n");
        std::string result1 = parse_to_polish("2+3");
        std::string result2 = parse_to_polish("3+2");
        REQUIRE(result1 != result2); // Structure differs, which is fine
        log("  OK\n");
    }
}

// Performance test for complex expressions
TEST_CASE("Parser handles large expressions", "[parser][performance]") {
    log("\n=== Testing large expressions ===\n");

    SECTION("Long chain of additions") {
        log("  Testing long addition chain...\n");
        std::string expr = "1";
        for (int i = 2; i <= 10; ++i) {
            expr += "+" + std::to_string(i);
        }
        // Should not throw
        REQUIRE_NOTHROW(parse_to_polish(expr));
        log("  OK\n");
    }

    SECTION("Deeply nested parentheses") {
        log("  Testing deeply nested parentheses...\n");
        std::string expr = "1";
        for (int i = 0; i < 20; ++i) {
            expr = "(" + expr + "+1)";
        }
        REQUIRE_NOTHROW(parse_to_polish(expr));
        log("  OK\n");
    }
}