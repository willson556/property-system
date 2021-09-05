//
// Created by Thomas Willson on 2021-09-04.
//

#include "PropertySet.h"

struct MyNestedClassProperties : public PropertySystem::IPropertyStruct {
    STORAGE_PROPERTY(int, p);
    STORAGE_PROPERTY_INIT(double, i, 0);

    const PropertySystem::PropertySet get_property_set() override {
        return PropertySystem::PropertySet{{&p, &i}};
    }
};


class MyClass {

};