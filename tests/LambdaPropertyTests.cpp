//
// Created by Thomas Willson on 2021-09-04.
//

#include "catch2/catch.hpp"

#include "Property.h"

using namespace PropertySystem;

TEST_CASE("LambdaReadOnlyProperty") {
    int var{5};

    const LambdaReadOnlyProperty uut{[&var]() { return var; }};

    REQUIRE(uut.get() == 5);
    REQUIRE(uut.size() == sizeof(var));

    var = 3;

    REQUIRE(uut.get() == 3);

    int buffer{0};
    uut.get(as_writable_bytes(buffer));
    REQUIRE(buffer == 3);
}

TEST_CASE("LambdaProperty") {
    double var{5};
    LambdaProperty uut{[&var]() { return var; }, [&var](double v) { var = v; }};

    SECTION("get") {
        REQUIRE(uut.get() == 5);
        REQUIRE(uut.size() == sizeof(var));

        var = 3;

        REQUIRE(uut.get() == 3);

        double buffer{0};
        uut.get(as_writable_bytes(buffer));
        REQUIRE(buffer == 3);
    }

    SECTION("set") {
        uut.set(2);
        REQUIRE(var == 2);

        double buffer{3};
        uut.set(as_bytes(buffer));
        REQUIRE(var == 3);
    }
}
