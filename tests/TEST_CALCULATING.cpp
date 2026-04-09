#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Visitor.h"
#include "Parser.h"
#include "Lexer.h"

double calculate(const std::string& expr, Calculating_context& calc_ctx) {
    Parser_context p_ctx;

    for (const auto& [name, _] : calc_ctx.variables) {
        p_ctx.variables.insert(name);
    }

    Lexer l(expr);
    Parser p(l, p_ctx);
    AST tree = p.parse_all();

    Calculating_visitor calc(calc_ctx);
    tree.accept(calc);
    return calc.get_result();
}

TEST_CASE("Calculator: Numbers and unary minus", "[calculator][basic]") {
    Calculating_context ctx;

    SECTION("Integer numbers") {
        CHECK(calculate("0", ctx) == Approx(0.0));
        CHECK(calculate("42", ctx) == Approx(42.0));
        CHECK(calculate("-42", ctx) == Approx(-42.0));
        CHECK(calculate("+42", ctx) == Approx(42.0));
    }

    SECTION("Decimal numbers") {
        CHECK(calculate("3.14", ctx) == Approx(3.14));
        CHECK(calculate("-3.14", ctx) == Approx(-3.14));
        CHECK(calculate("0.001", ctx) == Approx(0.001));
    }

    SECTION("Multiple unary minuses") {
        CHECK(calculate("-5", ctx) == Approx(-5.0));
        CHECK(calculate("--5", ctx) == Approx(5.0));
        CHECK(calculate("---5", ctx) == Approx(-5.0));
        CHECK(calculate("----5", ctx) == Approx(5.0));
    }

    SECTION("Unary minus with expressions") {
        CHECK(calculate("-(2+3)", ctx) == Approx(-5.0));
        CHECK(calculate("-2*3", ctx) == Approx(-6.0));
        CHECK(calculate("-(2*3)", ctx) == Approx(-6.0));
        CHECK(calculate("2*-3", ctx) == Approx(-6.0));
        CHECK(calculate("2*(-3)", ctx) == Approx(-6.0));
    }
}

TEST_CASE("Calculator: Basic binary operations", "[calculator][binary]") {
    Calculating_context ctx;

    SECTION("Addition") {
        CHECK(calculate("2+3", ctx) == Approx(5.0));
        CHECK(calculate("2+3+4", ctx) == Approx(9.0));
        CHECK(calculate("2+3+4+5", ctx) == Approx(14.0));
        CHECK(calculate("-2+5", ctx) == Approx(3.0));
        CHECK(calculate("5+(-2)", ctx) == Approx(3.0));
    }

    SECTION("Subtraction") {
        CHECK(calculate("5-3", ctx) == Approx(2.0));
        CHECK(calculate("10-3-2", ctx) == Approx(5.0));
        CHECK(calculate("5-10", ctx) == Approx(-5.0));
        CHECK(calculate("0-5", ctx) == Approx(-5.0));
    }

    SECTION("Multiplication") {
        CHECK(calculate("3*4", ctx) == Approx(12.0));
        CHECK(calculate("2*3*4", ctx) == Approx(24.0));
        CHECK(calculate("(-3)*4", ctx) == Approx(-12.0));
        CHECK(calculate("3*(-4)", ctx) == Approx(-12.0));
        CHECK(calculate("0*5", ctx) == Approx(0.0));
    }

    SECTION("Division") {
        CHECK(calculate("10/2", ctx) == Approx(5.0));
        CHECK(calculate("10/2/5", ctx) == Approx(1.0));
        CHECK(calculate("1/2", ctx) == Approx(0.5));
        CHECK(calculate("-6/2", ctx) == Approx(-3.0));
        CHECK(calculate("6/(-2)", ctx) == Approx(-3.0));
    }

    SECTION("Power") {
        CHECK(calculate("2^3", ctx) == Approx(8.0));
        CHECK(calculate("2^3^2", ctx) == Approx(512.0));
        CHECK(calculate("(2^3)^2", ctx) == Approx(64.0));
        CHECK(calculate("4^0.5", ctx) == Approx(2.0));
        CHECK(calculate("(-2)^3", ctx) == Approx(-8.0));
        CHECK(calculate("(-2)^4", ctx) == Approx(16.0));
        CHECK(calculate("0^5", ctx) == Approx(0.0));
        CHECK(calculate("5^0", ctx) == Approx(1.0));
    }
}

TEST_CASE("Calculator: Operator precedence", "[calculator][precedence]") {
    Calculating_context ctx;

    SECTION("Multiplication before addition") {
        CHECK(calculate("2+3*4", ctx) == Approx(14.0));
        CHECK(calculate("3*4+2", ctx) == Approx(14.0));
        CHECK(calculate("2+3*4+5", ctx) == Approx(19.0));
    }

    SECTION("Division before subtraction") {
        CHECK(calculate("10-6/2", ctx) == Approx(7.0));
        CHECK(calculate("6/2-1", ctx) == Approx(2.0));
    }

    SECTION("Power highest precedence") {
        CHECK(calculate("2*3^2", ctx) == Approx(18.0));
        CHECK(calculate("2^3*4", ctx) == Approx(32.0));
        CHECK(calculate("2+3^2*2", ctx) == Approx(20.0));
    }

    SECTION("Mixed precedence") {
        CHECK(calculate("2+3*4^2", ctx) == Approx(50.0));
        CHECK(calculate("2^2+3*4", ctx) == Approx(16.0));
        CHECK(calculate("10-2^3/2", ctx) == Approx(6.0));
    }
}

TEST_CASE("Calculator: Parentheses", "[calculator][parentheses]") {
    Calculating_context ctx;

    SECTION("Simple parentheses") {
        CHECK(calculate("(2+3)*4", ctx) == Approx(20.0));
        CHECK(calculate("2*(3+4)", ctx) == Approx(14.0));
        CHECK(calculate("(2+3)*(4+5)", ctx) == Approx(45.0));
    }

    SECTION("Nested parentheses") {
        CHECK(calculate("((2+3)*4)", ctx) == Approx(20.0));
        CHECK(calculate("(2+(3*4))", ctx) == Approx(14.0));
        CHECK(calculate("((2+3)*(4+5))/3", ctx) == Approx(15.0));
        CHECK(calculate("2*(3+(4*(5+1)))", ctx) == Approx(54.0));
    }

    SECTION("Redundant parentheses") {
        CHECK(calculate("(((2+3)))", ctx) == Approx(5.0));
        CHECK(calculate("((((2))))", ctx) == Approx(2.0));
    }

    SECTION("Unary minus with parentheses") {
        CHECK(calculate("-(2+3)", ctx) == Approx(-5.0));
        CHECK(calculate("-(2*3+4)", ctx) == Approx(-10.0));
        CHECK(calculate("-(-(2+3))", ctx) == Approx(5.0));
    }
}

TEST_CASE("Calculator: Variables", "[calculator][variables]") {
    SECTION("Single variable") {
        Calculating_context ctx;
        ctx.variables["x"] = 10.0;
        CHECK(calculate("x", ctx) == Approx(10.0));
    }

    SECTION("Variables in expressions") {
        Calculating_context ctx;
        ctx.variables["x"] = 5.0;
        ctx.variables["y"] = 3.0;

        CHECK(calculate("x+y", ctx) == Approx(8.0));
        CHECK(calculate("x-y", ctx) == Approx(2.0));
        CHECK(calculate("x*y", ctx) == Approx(15.0));
        CHECK(calculate("x/y", ctx) == Approx(5.0/3.0));
    }

    SECTION("Complex expressions with variables") {
        Calculating_context ctx;
        ctx.variables["a"] = 2.0;
        ctx.variables["b"] = 3.0;
        ctx.variables["c"] = 4.0;

        CHECK(calculate("a+b*c", ctx) == Approx(14.0));
        CHECK(calculate("(a+b)*c", ctx) == Approx(20.0));
        CHECK(calculate("a^b", ctx) == Approx(8.0));
        CHECK(calculate("-a+b", ctx) == Approx(1.0));
    }

    SECTION("Multiple variables") {
        Calculating_context ctx;
        ctx.variables["x1"] = 1.0;
        ctx.variables["x2"] = 2.0;
        ctx.variables["x3"] = 3.0;
        ctx.variables["y"] = 10.0;

        CHECK(calculate("x1+x2+x3", ctx) == Approx(6.0));
        CHECK(calculate("y/(x1+x2)", ctx) == Approx(10.0/3.0));
    }

    SECTION("Variable names with underscores and digits") {
        Calculating_context ctx;
        ctx.variables["var_1"] = 100.0;
        ctx.variables["_var2"] = 200.0;
        ctx.variables["x123"] = 50.0;

        CHECK(calculate("var_1", ctx) == Approx(100.0));
        CHECK(calculate("_var2", ctx) == Approx(200.0));
        CHECK(calculate("x123", ctx) == Approx(50.0));
        CHECK(calculate("var_1+_var2", ctx) == Approx(300.0));
    }
}

TEST_CASE("Calculator: Functions", "[calculator][functions]") {
    Calculating_context ctx;

    SECTION("Basic functions") {
        CHECK(calculate("sqrt(25)", ctx) == Approx(5.0));
        CHECK(calculate("sqrt(2)", ctx) == Approx(std::sqrt(2)));
        CHECK(calculate("exp(0)", ctx) == Approx(1.0));
        CHECK(calculate("exp(1)", ctx) == Approx(std::exp(1.0)));
        CHECK(calculate("log(100)", ctx) == Approx(std::log((double)100.0)));
    }

    SECTION("Trigonometric functions") {
        const double pi = 3.141592653589793;
        ctx.variables["pi"] = pi;

        CHECK(calculate("sin(0)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("sin(pi/2)", ctx) == Approx(1.0));
        CHECK(calculate("sin(pi)", ctx) == Approx(0.0).margin(1e-9));

        CHECK(calculate("cos(0)", ctx) == Approx(1.0));
        CHECK(calculate("cos(pi/2)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("cos(pi)", ctx) == Approx(-1.0));

        CHECK(calculate("tan(0)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("tan(pi/4)", ctx) == Approx(1.0));
    }

    SECTION("Inverse trigonometric functions") {
        ctx.variables["pi"] = 3.141592653589793;

        CHECK(calculate("asin(0)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("asin(1)", ctx) == Approx(1.57079632679)); // pi/2
        CHECK(calculate("acos(1)", ctx) == Approx(0.0));
        CHECK(calculate("acos(0)", ctx) == Approx(1.57079632679)); // pi/2
        CHECK(calculate("atan(0)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("atan(1)", ctx) == Approx(0.785398163397)); // pi/4
    }

    SECTION("Functions with expressions as arguments") {
        ctx.variables["x"] = 3.0;

        CHECK(calculate("sqrt(x+1)", ctx) == Approx(2.0));
        CHECK(calculate("exp(x-1)", ctx) == Approx(std::exp(2.0)));
        CHECK(calculate("log(x*10)", ctx) == Approx(std::log(30.0)));
    }

    SECTION("Nested functions") {
        CHECK(calculate("sqrt(16)", ctx) == Approx(4.0));
        CHECK(calculate("sin(3.14159)", ctx) == Approx(0.0).margin(1e-9));
        CHECK(calculate("exp(log(5))", ctx) == Approx(5.0));
        CHECK(calculate("sqrt(sqrt(16))", ctx) == Approx(2.0));
        CHECK(calculate("sin(asin(0.5))", ctx) == Approx(0.5));
    }
}

TEST_CASE("Calculator: Complex expressions", "[calculator][complex]") {
    Calculating_context ctx;
    ctx.variables["x"] = 2.0;
    ctx.variables["y"] = 3.0;
    ctx.variables["z"] = 4.0;

    SECTION("Long expressions") {
        CHECK(calculate("1+2+3+4+5+6+7+8+9+10", ctx) == Approx(55.0));
        CHECK(calculate("1*2*3*4*5", ctx) == Approx(120.0));
        CHECK(calculate("2^2^2", ctx) == Approx(16.0));
    }

    SECTION("Mixed everything") {
        CHECK(calculate("x^2 + y^2", ctx) == Approx(13.0));
        CHECK(calculate("sqrt(x^2 + y^2 + z^2)", ctx) == Approx(std::sqrt(29.0)));
        CHECK(calculate("sin(x)^2 + cos(x)^2", ctx) == Approx(1.0));
        CHECK(calculate("(x+y)*z/(x-1)", ctx) == Approx(20.0));
    }

    SECTION("Scientific calculations") {
        ctx.variables["a"] = 9.8;
        ctx.variables["t"] = 2.0;
        ctx.variables["v0"] = 5.0;

        CHECK(calculate("v0*t + 0.5*a*t^2", ctx) == Approx(5.0*2.0 + 0.5*9.8*4.0));

        ctx.variables["m"] = 2.0;
        ctx.variables["c"] = 3.0;
        CHECK(calculate("m*c^2", ctx) == Approx(18.0));
    }
}


TEST_CASE("Calculator: Error handling", "[calculator][errors]") {
    Calculating_context ctx;
    ctx.variables["x"] = 5.0;

    SECTION("Division by zero") {
        double res = calculate("1/0", ctx);
        CHECK(std::isinf(res));

        res = calculate("5/0", ctx);
        CHECK(std::isinf(res));

        res = calculate("(2+3)/0", ctx);
        CHECK(std::isinf(res));
    }

    SECTION("Invalid mathematical operations") {
        CHECK_THROWS(calculate("sqrt(-1)", ctx));
    }
}