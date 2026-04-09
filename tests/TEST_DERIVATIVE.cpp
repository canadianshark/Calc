#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cmath>
#include <memory>
#include "Visitor.h"
#include "Parser.h"
#include "Lexer.h"

std::string differentiate(const std::string& expr,
                          const std::string& var = "x",
                          const std::vector<std::string>& vars = {"x", "y", "z"}) {
    Parser_context p_ctx;

    p_ctx.variables.insert(var);
    for (const auto& v : vars) {
        p_ctx.variables.insert(v);
    }

    Lexer l(expr);
    Parser p(l, p_ctx);
    AST tree = p.parse_all();

    Derivative_context d_ctx;
    d_ctx.var = var;

    Derivative_visitor deriv(d_ctx);
    tree.accept(deriv);

    Printing_visitor printer;
    deriv.result->accept(printer);
    return printer.result;
}

TEST_CASE("Derivative: Constants", "[derivative][basic]") {
    SECTION("Integer constant") {
        CHECK(differentiate("5", "x") == "0");
        CHECK(differentiate("42", "x") == "0");
        CHECK(differentiate("0", "x") == "0");
        CHECK(differentiate("-10", "x") == "-(0)");
    }

    SECTION("Decimal constant") {
        CHECK(differentiate("3.14", "x") == "0");
        CHECK(differentiate("-2.5", "x") == "-(0)");
        CHECK(differentiate("0.001", "x") == "0");
    }
}

TEST_CASE("Derivative: Variables", "[derivative][variables]") {
    SECTION("Derivative with respect to the variable") {
        CHECK(differentiate("x", "x") == "1");
        CHECK(differentiate("y", "y") == "1");
        CHECK(differentiate("var", "var") == "1");
    }

    SECTION("Derivative with respect to other variable") {
        CHECK(differentiate("y", "x") == "0");
        CHECK(differentiate("z", "x") == "0");
    }

    SECTION("Multiple variables in context") {
        CHECK(differentiate("y", "x", {"y", "z"}) == "0");
        CHECK(differentiate("x", "x", {"y", "z"}) == "1");
        CHECK(differentiate("z", "y", {"x", "z"}) == "0");
    }
}

TEST_CASE("Derivative: Addition and subtraction", "[derivative][sum]") {
    SECTION("Simple addition") {
        CHECK(differentiate("x+5", "x") == "(1+0)");
        CHECK(differentiate("5+x", "x") == "(0+1)");
        CHECK(differentiate("x+y", "x", {"y"}) == "(1+0)");
    }

    SECTION("Multiple additions") {
        CHECK(differentiate("x+x", "x") == "(1+1)");
        CHECK(differentiate("x+y+z", "x", {"y", "z"}) == "((1+0)+0)");
    }

    SECTION("Subtraction") {
        CHECK(differentiate("x-5", "x") == "(1-0)");
        CHECK(differentiate("5-x", "x") == "(0-1)");
        CHECK(differentiate("x-y", "x", {"y"}) == "(1-0)");
    }

    SECTION("Mixed addition and subtraction") {
        CHECK(differentiate("x+y-5", "x", {"y"}) == "((1+0)-0)");
        CHECK(differentiate("10-x+y", "x", {"y"}) == "((0-1)+0)");
    }
}

TEST_CASE("Derivative: Multiplication (product rule)", "[derivative][product]") {
    SECTION("Multiplication by constant") {
        CHECK(differentiate("5*x", "x") == "((0*x)+(5*1))");
        CHECK(differentiate("x*3", "x") == "((1*3)+(x*0))");
        CHECK(differentiate("2*x", "x") == "((0*x)+(2*1))");
    }

    SECTION("Product of two variables") {
        CHECK(differentiate("x*y", "x", {"y"}) == "((1*y)+(x*0))");
        CHECK(differentiate("x*x", "x") == "((1*x)+(x*1))");
    }


    SECTION("Product with functions") {
        std::string result = differentiate("x*sin(x)", "x");
        CHECK(result.find("((1*sin(x))+(x*(cos(x)*1)))") != std::string::npos);
    }
}


TEST_CASE("Derivative: Division (quotient rule)", "[derivative][division]") {
    SECTION("Division by constant") {
        CHECK(differentiate("x/5", "x") == "(((1*5)-(x*0))/(5^2))");
        CHECK(differentiate("x/2", "x") == "(((1*2)-(x*0))/(2^2))");
    }

    SECTION("Constant divided by variable") {
        CHECK(differentiate("1/x", "x") == "(((0*x)-(1*1))/(x^2))");
        CHECK(differentiate("5/x", "x") == "(((0*x)-(5*1))/(x^2))");
    }

    SECTION("Division of two variables") {
        CHECK(differentiate("x/y", "x", {"y"}) == "(((1*y)-(x*0))/(y^2))");
    }

    SECTION("Complex quotients") {
        // Производная (x+1)/(x-1)
        std::string result = differentiate("(x+1)/(x-1)", "x");
        CHECK(result.find("/") != std::string::npos);
        CHECK(result.find("(x+1)") != std::string::npos);
        CHECK(result.find("(x-1)") != std::string::npos);
    }
}

TEST_CASE("Derivative: Power function", "[derivative][power]") {
    SECTION("Power with constant exponent") {
        CHECK(differentiate("x^2", "x") == "((2*(x^(2-1)))*1)");
        CHECK(differentiate("x^3", "x") == "((3*(x^(3-1)))*1)");
        CHECK(differentiate("x^n", "x", {"n"}) == "((n*(x^(n-1)))*1)");
    }

    SECTION("Constant to variable power") {
        CHECK(differentiate("2^x", "x") == "(((2^x)*ln(2))*1)");
        CHECK(differentiate("e^x", "x") == "(((e^x)*ln(e))*1)");
    }

    SECTION("Variable to variable power") {
        std::string result = differentiate("x^x", "x");
        CHECK(result.find("ln(x)") != std::string::npos);
        CHECK(result.find("(x^x)") != std::string::npos);
    }

    SECTION("Complex power expressions") {
        CHECK(differentiate("x^(2*x)", "x") == "(((x^(2*x))*((0*x)+(2*1)))*1)");
    }
}

// Тест 7: Производная тригонометрических функций
TEST_CASE("Derivative: Trigonometric functions", "[derivative][trig]") {
    SECTION("Sine") {
        CHECK(differentiate("sin(x)", "x") == "(cos(x)*1)");
        CHECK(differentiate("sin(2*x)", "x") == "(cos((2*x))*((0*x)+(2*1)))");
    }

    SECTION("Cosine") {
        CHECK(differentiate("cos(x)", "x") == "((-sin(x))*1)");
        CHECK(differentiate("cos(3*x)", "x") == "((-sin((3*x)))*((0*x)+(3*1)))");
    }

    SECTION("Tangent") {
        CHECK(differentiate("tan(x)", "x") == "((1/(cos(x)^2))*1)");
        CHECK(differentiate("tan(5*x)", "x") == "((1/(cos((5*x))^2))*((0*x)+(5*1)))");
    }

    SECTION("Inverse trigonometric") {
        CHECK(differentiate("asin(x)", "x") == "((1/sqrt((1-(x^2))))*1)");
        CHECK(differentiate("acos(x)", "x") == "((-1/sqrt((1-(x^2))))*1)");
        CHECK(differentiate("atan(x)", "x") == "((1/(1+(x^2)))*1)");
    }
}

TEST_CASE("Derivative: Exponential and logarithmic functions", "[derivative][explog]") {
    SECTION("Exponential") {
        CHECK(differentiate("exp(x)", "x") == "(exp(x)*1)");
        CHECK(differentiate("exp(2*x)", "x") == "(exp((2*x))*((0*x)+(2*1)))");
        CHECK(differentiate("exp(x^2)", "x") == "(exp((x^2))*((2*(x^(2-1)))*1))");
    }

    SECTION("Natural logarithm") {
        CHECK(differentiate("ln(x)", "x") == "((1/x)*1)");
        CHECK(differentiate("ln(3*x)", "x") == "((1/(3*x))*((0*x)+(3*1)))");
        CHECK(differentiate("ln(x^2)", "x") == "((1/(x^2))*((2*(x^(2-1)))*1))");
    }

    SECTION("Logarithm base 10") {
        CHECK(differentiate("log(x)", "x") == "((1/(x*ln(10)))*1)");
        CHECK(differentiate("log(5*x)", "x") == "((1/((5*x)*ln(10)))*((0*x)+(5*1)))");
    }
}

TEST_CASE("Derivative: Square root", "[derivative][sqrt]") {
    SECTION("Simple square root") {
        CHECK(differentiate("sqrt(x)", "x") == "((1/(2*sqrt(x)))*1)");
    }

    SECTION("Square root of expression") {
        CHECK(differentiate("sqrt(2*x)", "x") == "((1/(2*sqrt((2*x))))*((0*x)+(2*1)))");
        CHECK(differentiate("sqrt(x^2+1)", "x") == "((1/(2*sqrt(((x^2)+1))))*(((2*(x^(2-1)))*1)+0))");
    }
}

TEST_CASE("Derivative: Chain rule with nested functions", "[derivative][chain]") {
    SECTION("Nested trigonometric") {
        CHECK(differentiate("sin(cos(x))", "x") == "(cos(cos(x))*((-sin(x))*1))");
        CHECK(differentiate("cos(sin(x))", "x") == "((-sin(sin(x)))*(cos(x)*1))");
    }

    SECTION("Exponential of trigonometric") {
        CHECK(differentiate("exp(sin(x))", "x") == "(exp(sin(x))*(cos(x)*1))");
        CHECK(differentiate("sin(exp(x))", "x") == "(cos(exp(x))*(exp(x)*1))");
    }

    SECTION("Logarithm of function") {
        CHECK(differentiate("ln(sin(x))", "x") == "((1/sin(x))*(cos(x)*1))");
        CHECK(differentiate("ln(exp(x))", "x") == "((1/exp(x))*(exp(x)*1))");
    }

    SECTION("Deep nesting") {
        CHECK(differentiate("sin(ln(x^2))", "x") == "(cos(ln((x^2)))*((1/(x^2))*((2*(x^(2-1)))*1)))");
        CHECK(differentiate("sqrt(exp(cos(x)))", "x") == "((1/(2*sqrt(exp(cos(x)))))*(exp(cos(x))*((-sin(x))*1)))");
    }
}

TEST_CASE("Derivative: Combined rules", "[derivative][combined]") {
    SECTION("Product and chain rule") {
        std::string result = differentiate("x*sin(x^2)", "x");
        CHECK(result.find("(1*sin((x^2)))") != std::string::npos);
        CHECK(result.find("(x*(cos((x^2))") != std::string::npos);
    }

    SECTION("Quotient and chain rule") {
        std::string result = differentiate("sin(x)/cos(x)", "x");
        CHECK(result.find("((cos(x)*1)*cos(x))") != std::string::npos);
        CHECK(result.find("(sin(x)*((-sin(x))*1))") != std::string::npos);
    }

    SECTION("Power and chain rule") {
        CHECK(differentiate("sin(x)^2", "x") == "((2*(sin(x)^(2-1)))*(cos(x)*1))");
    }
}

TEST_CASE("Derivative: Unary minus", "[derivative][unary]") {
    SECTION("Derivative of negative expression") {
        CHECK(differentiate("-x", "x") == "(-1)");
        CHECK(differentiate("-5", "x") == "0");
        CHECK(differentiate("-(x+2)", "x") == "(-(1+0))");
    }

    SECTION("Nested unary minus") {
        CHECK(differentiate("--x", "x") == "1");
        CHECK(differentiate("---x", "x") == "(-1)");
    }

    SECTION("Unary minus with functions") {
        CHECK(differentiate("-sin(x)", "x") == "(-(cos(x)*1))");
        CHECK(differentiate("sin(-x)", "x") == "(cos((-x))*(-1))");
    }
}


TEST_CASE("Derivative: Special cases", "[derivative][special]") {
    SECTION("Derivative of zero") {
        CHECK(differentiate("0*x", "x") == "((0*x)+(0*1))");
        CHECK(differentiate("x*0", "x") == "((1*0)+(x*0))");
    }

    SECTION("Derivative of one") {
        CHECK(differentiate("x/x", "x") == "(((1*x)-(x*1))/(x^2))");
        CHECK(differentiate("x^0", "x") == "((0*(x^(0-1)))*1)");
    }

    SECTION("Derivative with respect to variable not in expression") {
        CHECK(differentiate("a+b", "x", {"a", "b"}) == "(0+0)");
        CHECK(differentiate("sin(a)", "x", {"a"}) == "(cos(a)*0)");
    }
}
