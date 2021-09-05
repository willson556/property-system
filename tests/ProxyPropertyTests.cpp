//
// Created by Thomas Willson on 2021-08-25.
//

#include "catch2/catch.hpp"
#include <array>

#include "PropertyProxy.h"

using namespace PropertySystem;

class TestReadOnlyProxy : public ReadOnlyProxyProperty<int32_t> {
public:
    TestReadOnlyProxy()
        : ReadOnlyProxyProperty<int32_t>{"test"} {}

	void get(gsl::span<std::byte> dst) const override {
			check_size(dst);
			for(size_t i{0}; i < size(); ++i) {
				dst.data()[i] = storage[i];
			}
	}
    using ReadOnlyProxyProperty::get;

    std::array<std::byte, sizeof(int32_t)> storage{};
    int32_t& storage_int{*reinterpret_cast<int32_t*>(storage.data())};
};

class TestProxy : public ProxyProperty<int32_t> {
public:
    TestProxy()
        : ProxyProperty<int32_t>{"test"} {}

	void get(gsl::span<std::byte> dst) const override {
			check_size(dst);
			for(size_t i{0}; i < size(); ++i) {
				dst.data()[i] = storage[i];
			}
	}
	using ReadOnlyProxyProperty::get;
	
	void set(gsl::span<const std::byte> src) override {
		check_size(src);
		for(size_t i{0}; i < size(); ++i) {
			storage[i] = src.data()[i];
		}
	}
	
	using ProxyProperty::set;

	std::array<std::byte, sizeof(int32_t)> storage{};
	int32_t& storage_int{*reinterpret_cast<int32_t*>(storage.data())};
};

TEST_CASE("ReadOnlyProxyProperty") {
    TestReadOnlyProxy test;
    test.storage_int = 2;

    int32_t var{test.get()};

    REQUIRE(var == 2);
}

TEST_CASE("ProxyProperty") {
	TestProxy test;
	
	SECTION("get") {
		test.storage_int = 2;
		int32_t var{test.get()};
		
		REQUIRE(var == 2);
	}
	
	SECTION("set") {
		test.set(5);
		REQUIRE(test.storage_int == 5);
	}
}
