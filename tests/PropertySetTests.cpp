//
// Created by Thomas Willson on 2021-09-04.
//

#include "catch2/catch.hpp"

#define PROPERTY_SYSTEM_INCLUDE_NAMES

#include "PropertySet.h"

using namespace PropertySystem;

struct MyNestedClassProperties : public IPropertyStruct {
    PropertySet get_property_set() override {
        return PropertySystem::PropertySet{{&p, &i}};
    }

    StorageProperty<int> p{"p"};
    StorageProperty<double> i{"i", 0};
};

struct MyClassProperties : public IPropertyStruct {
    PropertySet get_property_set() override {
        return PropertySet{{
                                   NestedPropertySet("Nested1",
                                                     nested1.get_property_set()),
                                   NestedPropertySet("Nested2",
                                                     nested2.get_property_set())},
                           {
                                   &max}
        };
    }

    PropertySystem::StorageProperty<int> max{"max"};

    MyNestedClassProperties nested1;
    MyNestedClassProperties nested2;
};

class MyNestedClass {
    MyNestedClass(MyNestedClassProperties &properties)
            : properties{properties} {}

private:
    MyNestedClassProperties &properties;
};

class MyClass {
    MyClass(MyClassProperties &properties)
            : properties{properties} {}

private:
    MyClassProperties &properties;
};

TEST_CASE("FlattenPropertyHierarchy") {
    MyClassProperties cls;

    auto property_list{flatten(cls.get_property_set())};
    REQUIRE(property_list.size() == 5);
}