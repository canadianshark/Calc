#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "AST.h"
#include <iostream>

void log(const std::string& msg) {
    std::cerr << msg << std::flush;
}

TEST_CASE("Number node works correctly", "[AST][number]") {
    log("\n=== Testing Number Node ===\n");

    SECTION("Create number node") {
        log("  Creating number node...\n");
        Number_node num("123");
        REQUIRE(num.get_value() == "123");
        log("  OK\n");
    }

    SECTION("Clone number node") {
        log("  Cloning number node...\n");
        Number_node original("3.14");
        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        log("  OK\n");
    }
}

TEST_CASE("Variable node works correctly", "[AST][variable]") {
    log("\n=== Testing Variable Node ===\n");

    SECTION("Create variable node") {
        log("  Creating variable node...\n");
        Variable_node var("x");
        REQUIRE(var.get_name() == "x");
        log("  OK\n");
    }

    SECTION("Clone variable node") {
        log("  Cloning variable node...\n");
        Variable_node original("my_var");
        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        log("  OK\n");
    }
}

TEST_CASE("Binop node works correctly", "[AST][binop]") {
    log("\n=== Testing Binary Operation Node ===\n");

    SECTION("Create binop node with operands") {
        log("  Creating binop node...\n");
        auto left = std::make_unique<Number_node>("2");
        auto right = std::make_unique<Number_node>("3");

        Binop_node binop("+");
        binop.first_op = std::move(left);
        binop.second_op = std::move(right);

        REQUIRE(binop.get_operation() == "+");
        REQUIRE(binop.first_op != nullptr);
        REQUIRE(binop.second_op != nullptr);
        log("  OK\n");
    }

    SECTION("Clone binop node") {
        log("  Cloning binop node...\n");
        auto left = std::make_unique<Number_node>("5");
        auto right = std::make_unique<Variable_node>("x");

        Binop_node original("*");
        original.first_op = std::move(left);
        original.second_op = std::move(right);

        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        log("  OK\n");
    }
}

TEST_CASE("Unop node works correctly", "[AST][unop]") {
    log("\n=== Testing Unary Operation Node ===\n");

    SECTION("Create unop node with operand") {
        log("  Creating unop node...\n");
        auto operand = std::make_unique<Number_node>("42");

        Unop_node unop("-");
        unop.op = std::move(operand);

        REQUIRE(unop.get_operation() == "-");
        REQUIRE(unop.op != nullptr);
        log("  OK\n");
    }

    SECTION("Clone unop node") {
        log("  Cloning unop node...\n");
        auto operand = std::make_unique<Variable_node>("y");

        Unop_node original("sin");
        original.op = std::move(operand);

        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        log("  OK\n");
    }
}

TEST_CASE("Func node works correctly", "[AST][func]") {
    log("\n=== Testing Function Node ===\n");

    SECTION("Create function node with argument") {
        log("  Creating function node...\n");
        auto arg = std::make_unique<Number_node>("0");

        Func_node func("sqrt");
        func.arg = std::move(arg);

        REQUIRE(func.get_name() == "sqrt");
        REQUIRE(func.arg != nullptr);
        log("  OK\n");
    }

    SECTION("Clone function node") {
        log("  Cloning function node...\n");
        auto arg = std::make_unique<Number_node>("4");

        Func_node original("log");
        original.arg = std::move(arg);

        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        log("  OK\n");
    }
}

TEST_CASE("AST tree works correctly", "[AST][tree]") {
    log("\n=== Testing AST Tree ===\n");

    SECTION("Create AST with number root") {
        log("  Creating AST...\n");
        auto root = std::make_unique<Number_node>("123");
        AST ast(std::move(root));
        log("  OK\n");
    }

    SECTION("Clone entire AST") {
        log("  Cloning entire AST...\n");
        auto left = std::make_unique<Number_node>("2");
        auto right = std::make_unique<Number_node>("3");

        auto plus = std::make_unique<Binop_node>("+");
        plus->first_op = std::move(left);
        plus->second_op = std::move(right);

        auto var = std::make_unique<Variable_node>("x");

        auto multiply = std::make_unique<Binop_node>("*");
        multiply->first_op = std::move(plus);
        multiply->second_op = std::move(var);

        AST original(std::move(multiply));
        AST cloned = original.clone_tree();
        log("  OK\n");
    }
}

TEST_CASE("Complex AST structure", "[AST][complex]") {
    log("\n=== Testing Complex AST Structures ===\n");

    SECTION("Function with binary operation argument") {
        log("  Creating sin(x + 1)...\n");
        auto var = std::make_unique<Variable_node>("x");
        auto one = std::make_unique<Number_node>("1");

        auto plus = std::make_unique<Binop_node>("+");
        plus->first_op = std::move(var);
        plus->second_op = std::move(one);

        auto sin = std::make_unique<Func_node>("sin");
        sin->arg = std::move(plus);

        AST ast(std::move(sin));
        AST cloned = ast.clone_tree();
        log("  OK\n");
    }

    SECTION("Nested binary operations") {
        log("  Creating (1 + 2) * (3 + 4)...\n");
        auto left_plus = std::make_unique<Binop_node>("+");
        left_plus->first_op = std::make_unique<Number_node>("1");
        left_plus->second_op = std::make_unique<Number_node>("2");

        auto right_plus = std::make_unique<Binop_node>("+");
        right_plus->first_op = std::make_unique<Number_node>("3");
        right_plus->second_op = std::make_unique<Number_node>("4");

        auto multiply = std::make_unique<Binop_node>("*");
        multiply->first_op = std::move(left_plus);
        multiply->second_op = std::move(right_plus);

        AST ast(std::move(multiply));
        log("  OK\n");
    }
}