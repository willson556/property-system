//
// Created by Thomas Willson on 2021-08-25.
//

#include <array>
#include "catch2/catch.hpp"

#include "PropertyProxy.h"

using namespace PropertySystem;

class TestProxy : public ReadOnlyProxyProperty<int32_t> {
public:
    void get(gsl::span<std::byte> dst) const;
    using ReadOnlyProxyProperty::get;

    std::array<std::byte, sizeof(int32_t)> storage{};
    int32_t& storage_int{*reinterpret_cast<int32_t*>(storage.data())};
};

void TestProxy::get(gsl::span<std::byte> dst) const {
    check_size(dst);
    for(size_t i{0}; i < this->size(); ++i) {
        dst.data()[i] = storage[i];
    }
}

TEST_CASE("ReadOnlyProxyProperty") {
    TestProxy test;
    test.storage_int = 2;

    int32_t var{test.get()};

    REQUIRE(var == 2);
}
