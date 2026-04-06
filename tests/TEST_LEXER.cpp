#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Lexer.h"

TEST_CASE("Basic lexer testing", "[lexer]"){
    Lexer l("2+2");
    auto t = l.peek();
    INFO(t.data);
    REQUIRE(t.data == "2");


}