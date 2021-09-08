//
// Created by Thomas Willson on 2021-09-07.
//

#include "catch2/catch.hpp"

#include "Property.h"

using namespace PropertySystem;

TEST_CASE("StorageProperty") {
    StorageProperty<int> uut{"var", 1};

    SECTION("get") {
        REQUIRE(uut.get() == 1);
        REQUIRE(uut.size() == sizeof(int));

        int buffer{0};
        uut.get(as_writable_bytes(buffer));
        REQUIRE(buffer == 1);

        char small_buffer;
        REQUIRE_THROWS(uut.get(as_writable_bytes(small_buffer)));

        int buffer2{uut};
        REQUIRE(buffer2 == 1);
    }

    SECTION("set") {
        uut.set(2);
        REQUIRE(uut.get() == 2);

        int buffer{3};
        uut.set(as_bytes(buffer));
        REQUIRE(uut.get() == 3);

        uut = 4;
        REQUIRE(uut.get() == 4);
    }
}
