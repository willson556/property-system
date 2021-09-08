//
// Created by Thomas Willson on 2021-09-04.
//

#include "catch2/catch.hpp"

#include "PropertySet.h"

using namespace PropertySystem;

struct Level1ClassProperties : public IPropertyStruct {
    PropertySet get_property_set() override {
        return PropertySet{{&j, &k}};
    }

    StorageProperty<int> j{"j"};
    StorageProperty<double> k{"k", 0};
};

struct Level2ClassProperties : public IPropertyStruct {
    PropertySet get_property_set() override {
        return PropertySet{
            {NestedPropertySet{"Level1", level1.get_property_set()}}, {&p, &i}};
    }

    StorageProperty<int> p{"p"};
    StorageProperty<double> i{"i", 0};

    Level1ClassProperties level1;
};

struct Level3ClassProperties : public IPropertyStruct {
    PropertySet get_property_set() override {
        return PropertySet{
            {NestedPropertySet("Nested1", nested1.get_property_set()),
             NestedPropertySet("Nested2", nested2.get_property_set())},
            {&max}};
    }

    StorageProperty<int> max{"max"};

    Level2ClassProperties nested1;
    Level2ClassProperties nested2;
};

static void check_property(const FlattenedPropertyWrapper& wrapper,
                           const IReadOnlyProperty& expected_target,
                           std::string expected_name) {
    REQUIRE(wrapper.name() == expected_name);
    REQUIRE(&wrapper.property() == &expected_target);
}

TEST_CASE("FlattenPropertyHierarchy") {
    Level3ClassProperties cls;

    auto property_list{flatten(cls.get_property_set())};
    REQUIRE(property_list.size() == 9);

    check_property(property_list[0], cls.nested1.level1.j, "Nested1.Level1.j");
    check_property(property_list[1], cls.nested1.level1.k, "Nested1.Level1.k");
    check_property(property_list[2], cls.nested1.p, "Nested1.p");
    check_property(property_list[3], cls.nested1.i, "Nested1.i");
    check_property(property_list[4], cls.nested2.level1.j, "Nested2.Level1.j");
    check_property(property_list[5], cls.nested2.level1.k, "Nested2.Level1.k");
    check_property(property_list[6], cls.nested2.p, "Nested2.p");
    check_property(property_list[7], cls.nested2.i, "Nested2.i");
    check_property(property_list[8], cls.max, "max");
}