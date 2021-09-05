//
// Created by Thomas Willson on 2021-08-24.
//

#include "catch2/catch.hpp"

#include "Property.h"

using namespace PropertySystem;

TEST_CASE("ReferenceReadOnlyProperty") {
    int var{0};

    const ReferenceReadOnlyProperty<int> uut{"var", var};

    REQUIRE(uut.get() == 0);
    REQUIRE(uut.size() == sizeof(int));

    var = 1;
    REQUIRE(uut.get() == 1);

    int buffer{0};
    uut.get(as_writable_bytes(buffer));
    REQUIRE(buffer == 1);
}

TEST_CASE("ReferenceProperty") {
    double var{0};
    ReferenceProperty<double> uut{"var", var};

    SECTION("get") {
        REQUIRE(uut.get() == 0);
        REQUIRE(uut.size() == sizeof(double));

        var = 1;
        REQUIRE(uut.get() == 1);

        double buffer{0};
        uut.get(as_writable_bytes(buffer));
        REQUIRE(buffer == 1);

        float buffer2{0};
        REQUIRE_THROWS(uut.get(as_writable_bytes(buffer2)));
    }

    SECTION("set") {
        uut.set(2);
        REQUIRE(var == 2);

        double buffer{3};
        uut.set(as_bytes(buffer));
        REQUIRE(var == 3);
    }
}
