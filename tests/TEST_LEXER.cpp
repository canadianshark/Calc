#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Lexer.h"
#include <stdexcept>
#include <iostream>

void log(const std::string& msg) {
    std::cerr << msg << std::flush;
}



TEST_CASE("Lexer handles numbers correctly", "[lexer][numbers]") {
    log("\n=== Testing number handling ===\n");

    SECTION("Integer number") {
        log("  Testing integer number...\n");
        Lexer lexer("123");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::NUMBER);
        CHECK(t.data == "123");
        log("  OK\n");
    }

    SECTION("Single digit zero is valid") {
        log("  Testing single digit zero...\n");
        Lexer lexer("0");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::NUMBER);
        CHECK(t.data == "0");
        log("  OK\n");
    }

    SECTION("Zero with decimal is valid") {
        log("  Testing zero with decimal...\n");
        Lexer lexer("0.5");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::NUMBER);
        CHECK(t.data == "0.5");
        log("  OK\n");
    }

    SECTION("Leading zero before digits - exception") {
        log("  Testing leading zero before digits (should throw)...\n");
        Lexer lexer("0123");
        REQUIRE_THROWS_AS(lexer.next(), std::invalid_argument);
        log("  OK\n");
    }
}

TEST_CASE("Lexer handles identifiers correctly", "[lexer][identifiers]") {
    log("\n=== Testing identifier handling ===\n");

    SECTION("Simple identifier") {
        log("  Testing simple identifier...\n");
        Lexer lexer("x");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::IDENTIFICATOR);
        CHECK(t.data == "x");
        log("  OK\n");
    }

    SECTION("Identifier with letters and numbers") {
        log("  Testing identifier with letters and numbers...\n");
        Lexer lexer("var123");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::IDENTIFICATOR);
        CHECK(t.data == "var123");
        log("  OK\n");
    }

    SECTION("Identifier with underscore") {
        log("  Testing identifier with underscore...\n");
        Lexer lexer("my_var_name");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::IDENTIFICATOR);
        CHECK(t.data == "my_var_name");
        log("  OK\n");
    }

    SECTION("Invalid identifier - starts with digit") {
        log("  Testing identifier starting with digit (should throw)...\n");
        Lexer lexer("123abc");
        REQUIRE_THROWS_AS(lexer.next(), std::invalid_argument);
        log("  OK\n");
    }
}

TEST_CASE("Lexer handles operators correctly", "[lexer][operators]") {
    log("\n=== Testing operator handling ===\n");

    SECTION("Plus operator") {
        log("  Testing plus operator...\n");
        Lexer lexer("+");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::PLUS);
        CHECK(t.data == "+");
        log("  OK\n");
    }

    SECTION("Minus operator") {
        log("  Testing minus operator...\n");
        Lexer lexer("-");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::MINUS);
        CHECK(t.data == "-");
        log("  OK\n");
    }

    SECTION("Multiplication operator") {
        log("  Testing multiplication operator...\n");
        Lexer lexer("*");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::MULTIPLICATION);
        CHECK(t.data == "*");
        log("  OK\n");
    }

    SECTION("Division operator") {
        log("  Testing division operator...\n");
        Lexer lexer("/");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::DIVISION);
        CHECK(t.data == "/");
        log("  OK\n");
    }

    SECTION("Power operator") {
        log("  Testing power operator...\n");
        Lexer lexer("^");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::POWER);
        CHECK(t.data == "^");
        log("  OK\n");
    }
}

TEST_CASE("Lexer handles brackets", "[lexer][brackets]") {
    log("\n=== Testing bracket handling ===\n");

    SECTION("Left bracket") {
        log("  Testing left bracket...\n");
        Lexer lexer("(");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::BRACKET);
        CHECK(t.data == "(");
        log("  OK\n");
    }

    SECTION("Right bracket") {
        log("  Testing right bracket...\n");
        Lexer lexer(")");
        Token t = lexer.next();
        CHECK(t.lexem == Lexem::BRACKET);
        CHECK(t.data == ")");
        log("  OK\n");
    }
}

TEST_CASE("Lexer ignores whitespace", "[lexer][whitespace]") {
    log("\n=== Testing whitespace handling ===\n");
    log("  Testing whitespace skipping...\n");

    Lexer lexer("123 + 456");

    Token t1 = lexer.next();
    CHECK(t1.lexem == Lexem::NUMBER);
    CHECK(t1.data == "123");

    Token t2 = lexer.next();
    CHECK(t2.lexem == Lexem::PLUS);
    CHECK(t2.data == "+");

    Token t3 = lexer.next();
    CHECK(t3.lexem == Lexem::NUMBER);
    CHECK(t3.data == "456");

    log("  OK\n");
}

TEST_CASE("Lexer next() consumes tokens sequentially", "[lexer][sequence]") {
    log("\n=== Testing sequential token consumption ===\n");
    log("  Testing next() sequence...\n");

    Lexer lexer("a+b*2");

    Token t1 = lexer.next();
    CHECK(t1.lexem == Lexem::IDENTIFICATOR);
    CHECK(t1.data == "a");

    Token t2 = lexer.next();
    CHECK(t2.lexem == Lexem::PLUS);
    CHECK(t2.data == "+");

    Token t3 = lexer.next();
    CHECK(t3.lexem == Lexem::IDENTIFICATOR);
    CHECK(t3.data == "b");

    Token t4 = lexer.next();
    CHECK(t4.lexem == Lexem::MULTIPLICATION);
    CHECK(t4.data == "*");

    Token t5 = lexer.next();
    CHECK(t5.lexem == Lexem::NUMBER);
    CHECK(t5.data == "2");

    log("  OK\n");
}

TEST_CASE("Lexer peek() does not consume tokens", "[lexer][peek]") {
    log("\n=== Testing peek() functionality ===\n");
    log("  Testing peek() without consumption...\n");

    Lexer lexer("42+x");

    Token peeked = lexer.peek();
    CHECK(peeked.lexem == Lexem::NUMBER);
    CHECK(peeked.data == "42");

    Token actual = lexer.next();
    CHECK(actual.lexem == Lexem::NUMBER);
    CHECK(actual.data == "42");

    Token second = lexer.next();
    CHECK(second.lexem == Lexem::PLUS);

    Token third_peek = lexer.peek();
    CHECK(third_peek.lexem == Lexem::IDENTIFICATOR);
    CHECK(third_peek.data == "x");

    Token third_actual = lexer.next();
    CHECK(third_actual.lexem == Lexem::IDENTIFICATOR);
    CHECK(third_actual.data == "x");

    log("  OK\n");
}

TEST_CASE("Lexer handles complex expressions", "[lexer][integration]") {
    log("\n=== Testing complex expressions ===\n");

    SECTION("Expression with numbers and operators") {
        log("  Testing expression with numbers and operators...\n");
        Lexer lexer("2+3*4");

        Token t1 = lexer.next();
        CHECK(t1.lexem == Lexem::NUMBER);
        CHECK(t1.data == "2");

        Token t2 = lexer.next();
        CHECK(t2.lexem == Lexem::PLUS);

        Token t3 = lexer.next();
        CHECK(t3.lexem == Lexem::NUMBER);
        CHECK(t3.data == "3");

        Token t4 = lexer.next();
        CHECK(t4.lexem == Lexem::MULTIPLICATION);

        Token t5 = lexer.next();
        CHECK(t5.lexem == Lexem::NUMBER);
        CHECK(t5.data == "4");

        log("  OK\n");
    }

    SECTION("Expression with parentheses") {
        log("  Testing expression with parentheses...\n");
        Lexer lexer("(a+3.14)*x");

        Token t1 = lexer.next();
        CHECK(t1.lexem == Lexem::BRACKET);

        Token t2 = lexer.next();
        CHECK(t2.lexem == Lexem::IDENTIFICATOR);
        CHECK(t2.data == "a");

        Token t3 = lexer.next();
        CHECK(t3.lexem == Lexem::PLUS);

        Token t4 = lexer.next();
        CHECK(t4.lexem == Lexem::NUMBER);
        CHECK(t4.data == "3.14");

        Token t5 = lexer.next();
        CHECK(t5.lexem == Lexem::BRACKET);

        Token t6 = lexer.next();
        CHECK(t6.lexem == Lexem::MULTIPLICATION);

        Token t7 = lexer.next();
        CHECK(t7.lexem == Lexem::IDENTIFICATOR);
        CHECK(t7.data == "x");

        log("  OK\n");
    }
}

TEST_CASE("Edge cases", "[lexer][edge]") {
    log("\n=== Testing edge cases ===\n");

    SECTION("Empty expression throws") {
        log("  Testing empty expression...\n");
        Lexer lexer("");
        Token t1 = lexer.next();
        CHECK(t1.lexem == Lexem::END);
        CHECK(t1.data == "");
        log("  OK\n");
    }

    SECTION("Expression with only whitespace") {
        log("  Testing expression with only whitespace...\n");
        Lexer lexer("   ");
        REQUIRE_THROWS_AS(lexer.next(), std::runtime_error);
        log("  OK\n");
    }
}